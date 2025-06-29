#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"

#include "hud.h"
#include "globals.h"
#include "engine/gameobject.h"
#include "engine/background.h"
#include "engine/utils.h"
#include "engine/level.h"
#include "entities/player.h"
#include "entities/enemy.h"
#include "engine/objects_pool.h"


typedef enum { GAME_STATE_MENU, GAME_STATE_PLAY, GAME_STATE_CONTROLS, GAME_STATE_CREDITS, GAME_STATE_EXIT, GAME_STATE_LEVEL_CLEAR, GAME_STATE_RETRY } GameState;
GameState gameState = GAME_STATE_MENU;

extern bool pegou_queijo;
u8 current_room = 0;
u8 level = 1;

// index for tiles in VRAM (first tile reserved for SGDK)
// Mudei para ser uma variável que controla o próximo índice disponível,
// e será resetada em game_init().
u16 next_vram_index;

// glow color effect
u8 bg_colors_delay = 5;
const u16 bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};

// enemies pool
#define MAX_BALLS 30
GameObject enemy_array[MAX_BALLS];
u16 enemy_tiles_ind;
ObjectsPool enemy_pool;

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void init_enemies() {
	// init_enemy_lookup_table(level1_objects, LEN(level1_objects));
	MAPOBJ_init_mapobjects(level1_objects, LEN(level1_objects));
	OBJPOOL_init(&enemy_pool, enemy_array, LEN(enemy_array));

	// load enemy tiles
	// 'enemy_tiles_ind' agora é setado ANTES de chamar esta função, em game_init()
	// E ENEMY_load_tiles deve retornar o número de tiles que consumiu.
	next_vram_index += ENEMY_load_tiles(enemy_tiles_ind);
}

void spawn_enemies() {
	// spawn enemies in current room
	u8 room = LEVEL_current_room();

	// Get Map Data Object from level_objects for the current room
	MapObject* mapobj = MAPOBJ_loop_init(level1_objects, LEN(level1_objects), room);
	while(mapobj) {
		GameObject* enemy = OBJPOOL_get_available(&enemy_pool);
		if (!enemy) return;

		// Enemy factory function: It gets the needed data from MapObject
		ENEMY_init(enemy, mapobj, enemy_tiles_ind);
		mapobj = MAPOBJ_loop_next(level1_objects, LEN(level1_objects), room);
	}

}

void clear_enemies() {
	OBJPOOL_clear(&enemy_pool);
}

void game_init() {
    pegou_queijo = false;
    VDP_setScreenWidth320();
    SPR_init(); // Limpa e reinicia o sistema de sprites

    // Reinicia o índice de tiles da VRAM para o início do espaço do usuário
    next_vram_index = TILE_USER_INDEX;

    // Inicializar sistemas na ordem correta, atualizando 'next_vram_index'
    // Cada função de init deve carregar seus tiles e retornar o número de tiles consumidos
    next_vram_index += BACKGROUND_init(next_vram_index);
    next_vram_index += LEVEL_init(next_vram_index, level);
    
    // PLAYER_init deve usar 'next_vram_index' para carregar seus tiles
    // e esperamos que ele também retorne o número de tiles que consumiu.
    // Se não retornar, você precisará saber o tamanho dos tiles do player e adicionar manualmente aqui.
    PLAYER_init(next_vram_index);
    // Exemplo se PLAYER_init não retorna o offset:
    // next_vram_index += PLAYER_TILE_COUNT; // Defina PLAYER_TILE_COUNT em algum lugar

    // Define o índice de início para os tiles dos inimigos
    enemy_tiles_ind = next_vram_index;
    init_enemies(); // init_enemies agora incrementa next_vram_index internamente.
    spawn_enemies();

    //LEVEL_draw_collision_map(); 
}

	

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

static inline void color_effects() {
	--bg_colors_delay;
	if (bg_colors_delay == 0) {
		// rotate_colors_left(PAL_BACKGROUND*16, PAL_BACKGROUND*16+15);
		glow_color(PAL_BACKGROUND*16+8, bg_color_glow, 5);

		bg_colors_delay = 10;
	}
}

inline void update_enemies() {
	GameObject* obj = OBJPOOL_loop_init(&enemy_pool);
	while (obj) {
		obj->update(obj);
		GameObject* obj_to_release = NULL;

		if (GAMEOBJECT_check_collision(&player, obj)) {
			PLAYER_on_hit(1);
			//obj->on_hit(obj, 1);
			obj_to_release = obj;
		}
		
		obj = OBJPOOL_loop_next(&enemy_pool);
		if (obj_to_release) {
			OBJPOOL_release(&enemy_pool, obj_to_release);
		}
	}
}

static inline void game_update() {
    update_input();
    PLAYER_update();
    update_enemies();
    //BACKGROUND_update();

    #if MAP_SOLUTION == MAP_BY_COMPACT_MAP
    LEVEL_update_camera(&player);
    #endif
}

// ===================================================================
// MENU E GAME STATES (seu código existente)
// ===================================================================

u8 menu_option = 0; // 0 = Play, 1 = Controles, 2 = Credits, 3 = Exit

void draw_menu() {
    VDP_drawImage(BG_B, &img_mainmenu, 0, 0);
    VDP_clearTextArea(0, 0, 40, 28);
    VDP_drawText(menu_option == 0 ? "> Play"      : "  Play",      15, 10);
    VDP_drawText(menu_option == 1 ? "> Controles" : "  Controles", 15, 12);
    VDP_drawText(menu_option == 2 ? "> Credits"   : "  Credits",   15, 14);
    VDP_drawText(menu_option == 3 ? "> Exit"      : "  Exit",      15, 16);
}

void update_menu() {
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_DOWN) {
        if (menu_option < 3) menu_option++;
        draw_menu();
        waitMs(150);
    }
    if (value & BUTTON_UP) {
        if (menu_option > 0) menu_option--;
        draw_menu();
        waitMs(150);
    }
    if (value & BUTTON_A || value & BUTTON_START) {
        if (menu_option == 0) gameState = GAME_STATE_PLAY;
        else if (menu_option == 1) gameState = GAME_STATE_CONTROLS;
        else if (menu_option == 2) gameState = GAME_STATE_CREDITS;
        else if (menu_option == 3) gameState = GAME_STATE_EXIT;
    }
}

int main(bool resetType) {
    // Soft reset doesn't clear RAM. Can lead to bugs.
    if (!resetType) {
        SYS_hardReset();
    }
    SYS_showFrameLoad(true);

    SYS_doVBlankProcess();

    kprintf("Free RAM after Game Init: %d", MEM_getFree());
    draw_menu(); // Mostra o menu ao iniciar

    bool game_started = false; // flag para inicializar o jogo só uma vez

    while (1) {
        switch (gameState) {
            case GAME_STATE_MENU:
                game_started = false; // Garante que o jogo será inicializado se voltar ao PLAY
                update_menu();
                break;
            case GAME_STATE_PLAY:
                if (!game_started) {
                    game_init(); // Inicializa todos os recursos da fase
                    SYS_doVBlankProcess(); // Processa as atualizações na VDP
                    kprintf("Free RAM after Game Init: %d", MEM_getFree());
                    game_started = true;
                }
                game_update();
                break;
            case GAME_STATE_CONTROLS:
                VDP_clearTextArea(0, 0, 40, 28);
                VDP_drawText("Controles:", 15, 8);
                VDP_drawText("Setas: mover", 12, 11);
                VDP_drawText("A: pular/selecionar", 12, 13);
                VDP_drawText("Start: menu", 12, 15);
                VDP_drawText("Pressione A para voltar", 8, 20);
                if (JOY_readJoypad(JOY_1) & BUTTON_A) {
                    waitMs(150); // Adicionado pequeno delay para evitar dupla detecção
                    gameState = GAME_STATE_MENU;
                    draw_menu();
                }
                break;
            case GAME_STATE_CREDITS:
                VDP_clearTextArea(0, 0, 40, 28);
                VDP_drawText("Feito por Felipe Gomes", 12, 12);
                VDP_drawText("Pressione A para voltar", 8, 20);
                if (JOY_readJoypad(JOY_1) & BUTTON_A) {
                    waitMs(150); // Adicionado pequeno delay
                    gameState = GAME_STATE_MENU;
                    draw_menu();
                }
                break;
            case GAME_STATE_EXIT:
                SYS_hardReset();
                break;

            case GAME_STATE_LEVEL_CLEAR:
                SPR_reset(); // Limpa todos os sprites existentes na tela
                SYS_doVBlankProcess(); // Garante que as mudanças de VRAM sejam aplicadas
                VDP_drawImage(BG_B, &img_level_clear, 0, 0); // Carrega nova imagem de fundo
                VDP_clearTextArea(0, 0, 40, 28);
                VDP_drawText("Fase Completa!", 14, 20);

                VDP_drawText("A: Jogar novamente", 10, 22);
                VDP_drawText("B: Proxima Fase", 10, 23);
                VDP_drawText("Start: Menu", 14, 25);

                u16 value = JOY_readJoypad(JOY_1);

                if (value & BUTTON_A) {
                    waitMs(150);
                    // Não é necessário SPR_reset() novamente, game_init() chama SPR_init()
                    game_init(); // Reinicia a fase atual
                    SYS_doVBlankProcess();
                    kprintf("Free RAM after Game Init: %d", MEM_getFree());
                    gameState = GAME_STATE_PLAY;
                    game_started = true;
                }
                else if (value & BUTTON_B) {
                    waitMs(150);
                    level++;
                    if (level > 3) level = 1; // volta para o primeiro se passar do último
                    kprintf("Indo para o level: %d", level);
                    // Não é necessário SPR_reset() novamente, game_init() chama SPR_init()
                    game_init();
                    SYS_doVBlankProcess();
                    kprintf("Free RAM after Game Init: %d", MEM_getFree());
                    gameState = GAME_STATE_PLAY;
                    game_started = true;
                }
                else if (value & BUTTON_START) {
                    waitMs(150);
                    level = 1; // Reseta o level ao voltar para o menu
                    gameState = GAME_STATE_MENU;
                    draw_menu();
                }
                break;

            case GAME_STATE_RETRY:
                    SPR_reset(); // Limpa todos os sprites existentes
                    SYS_doVBlankProcess(); // Garante que as mudanças de VRAM sejam aplicadas
                    game_started = false; // Garante que game_init será chamado
                    VDP_drawImage(BG_B, &img_retry, 0, 0); // Carrega nova imagem de fundo
                    VDP_clearTextArea(0, 0, 40, 28);
                    VDP_drawText("Voce morreu!", 15, 20);            
                    VDP_drawText("Pressione A para tentar de novo", 8, 22);
                    VDP_drawText("Pressione START para ir ao menu", 4, 24); // Alterado de ENTER para START

                    if (JOY_readJoypad(JOY_1) & BUTTON_A) {
                        waitMs(150);
                        // Não é necessário SPR_reset() novamente, game_init() chama SPR_init()
                        game_init();
                        SYS_doVBlankProcess();
                        kprintf("Free RAM after Game Init: %d", MEM_getFree());
                        game_started = true;
                        gameState = GAME_STATE_PLAY;
                    } else if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                        waitMs(150);
                        level = 1; // Reseta o level ao voltar para o menu
                        gameState = GAME_STATE_MENU;
                        draw_menu();
                    }
                    break;
        }
        SPR_update(); // Atualiza os dados de sprite no final do frame
        SYS_doVBlankProcess(); // Espera pelo VBlank e processa as atualizações da VDP
    }

    return 0;
}