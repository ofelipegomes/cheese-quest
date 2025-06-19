#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"
#include "engine/gameobject.h"
#include "engine/utils.h"
#include "engine/level.h"
#include "engine/background.h"
#include "engine/gameobject.h"
#include "player.h"

typedef enum { GAME_STATE_MENU, GAME_STATE_PLAY, GAME_STATE_CONTROLS, GAME_STATE_CREDITS, GAME_STATE_EXIT, GAME_STATE_LEVEL_CLEAR, GAME_STATE_RETRY } GameState;
GameState gameState = GAME_STATE_MENU;

extern bool pegou_queijo;


u16 ind = TILE_USER_INDEX;

u8 bg_colors_delay = 5;
const u16 const bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};




void game_init() {
    pegou_queijo = false;
    VDP_setScreenWidth320();
    SPR_init();

    ind += BACKGROUND_init(ind);
    ind += LEVEL_init(ind);

    PLAYER_init(ind);

    //LEVEL_draw_collision_map(); 
}

static inline void game_update() {
	update_input();
    PLAYER_update();
	//BACKGROUND_update();

	#if MAP_SOLUTION == MAP_BY_COMPACT_MAP
	LEVEL_update_camera(&player);
	#endif

	
  
}

u8 menu_option = 0; // 0 = Play, 1 = Controles, 2 = Credits, 3 = Exit

void draw_menu() {
    VDP_clearTextArea(0, 0, 40, 28);
    VDP_drawText("CHEESE QUEST", 12, 6);
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
		//waitMs(50);
    }
}

int main(bool resetType) {
    // Soft reset doesn't clear RAM. Can lead to bugs.
    if (!resetType) {
        SYS_hardReset();
    }
    SYS_showFrameLoad(true);
    ///game_init();

    SYS_doVBlankProcess();

    kprintf("Free RAM after Game Init: %d", MEM_getFree());
    draw_menu(); // Mostra o menu ao iniciar

    bool game_started = false; // flag para inicializar o jogo só uma vez

while (1) {
    switch (gameState) {
        case GAME_STATE_MENU:
            game_started = false;
            update_menu();
            break;
        case GAME_STATE_PLAY:
            if (!game_started) {
                game_init();
                SYS_doVBlankProcess();
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
                gameState = GAME_STATE_MENU;
                draw_menu();
            }
            break;
        case GAME_STATE_CREDITS:
            VDP_clearTextArea(0, 0, 40, 28);
            VDP_drawText("Feito por Felipe Gomes", 12, 12);
            if (JOY_readJoypad(JOY_1) & BUTTON_A) {
                gameState = GAME_STATE_MENU;
                draw_menu();
            }
            break;
        case GAME_STATE_EXIT:
            SYS_hardReset();
            break;
        case GAME_STATE_LEVEL_CLEAR:
            SPR_reset();

            VDP_clearTextArea(0, 0, 40, 28);

            VDP_drawText("Fase Completa!", 14, 12);
            VDP_drawText("Pressione A para proxima fase", 8, 16);
            VDP_drawText("Start para voltar ao menu", 10, 18);

            if (JOY_readJoypad(JOY_1) & BUTTON_A) {
                waitMs(150);
                game_init();
                SYS_doVBlankProcess();
                kprintf("Free RAM after Game Init: %d", MEM_getFree());
                game_started = true;
                gameState = GAME_STATE_PLAY;
                } else if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                    waitMs(150);
                    gameState = GAME_STATE_MENU;
                    draw_menu();
                }
            break;

        case GAME_STATE_RETRY:
                SPR_reset(); 
                SYS_doVBlankProcess();
                game_started = false;
                VDP_clearTextArea(0, 0, 40, 28);
                VDP_drawText("Voce morreu!", 15, 10);            
                VDP_drawText("Pressione A para tentar de novo", 8, 11);
                VDP_drawText("Pressione ENTER para ir ao menu", 4, 12);

                if (JOY_readJoypad(JOY_1) & BUTTON_A) {
                    waitMs(150);
                    game_init();
                    SYS_doVBlankProcess();
                    kprintf("Free RAM after Game Init: %d", MEM_getFree());
                    game_started = true;
                    gameState = GAME_STATE_PLAY;
                } else if (JOY_readJoypad(JOY_1) & BUTTON_START) {
                    waitMs(150);
                    gameState = GAME_STATE_MENU;
                    draw_menu();
                }
                break;
        

    }
    SPR_update();
    SYS_doVBlankProcess();
}

    return 0;
}