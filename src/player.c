#include <genesis.h>
#include <maths.h>
#include "gamestate.h"
#include "player.h"
#include "engine/level.h"
#include "engine/utils.h"
#include "engine/gameobject.h"
#include "globals.h"

#define PLAYER_SPEED FIX16(2)

GameObject player;
extern GameState gameState;

static inline void PLAYER_get_input_platformer(void);
static inline bool on_ground();
bool pegou_queijo = false;
////////////////////////////////////////////////////////////////////////////
// INIT

u16 PLAYER_init(u16 ind) {
    ind += GAMEOBJECT_init(&player, &spr_rato, 16, SCREEN_H - player.h - 16, 0, 0, PAL_PLAYER, ind);
    player.x = FIX16(16);
    player.y = FIX16(SCREEN_H - player.h - 16);
    player.next_x = player.x;
    player.next_y = player.y;
    return ind;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void PLAYER_update() {
	// input
	// PLAYER_get_input_dir4();
	//PLAYER_get_input_dir8();
	 PLAYER_get_input_platformer();
	
	// project next position
	player.next_x = player.x + player.speed_x;
	player.next_y = player.y + player.speed_y;

	// check and resolve walls
	LEVEL_move_and_slide(&player);
	
	// update current position
	player.x = player.next_x;
	player.y = player.next_y;
	
	// limit do map boundaries
	//LEVEL_check_map_boundaries(&player);
	
	// item check
GAMEOBJECT_update_boundbox(player.x, player.y, &player);
u16 tile = LEVEL_tileXY(player.box.left + player.w/2, player.box.top + player.h/2);

if (tile == IDX_CHEESE) {
    //HUD_gem_collected(1);
    LEVEL_remove_tileXY(player.box.left + player.w/2, player.box.top + player.h/2, IDX_EMPTY);
    pegou_queijo = true;
    LEVEL_generate_screen_collision_map(IDX_EMPTY, IDX_WALL_FIRST, IDX_WALL_LAST); // <-- Adicione esta linha
}

if (tile == IDX_SPIKE) {
    gameState = GAME_STATE_LEVEL_CLEAR;
    return;
}

if (tile == IDX_TOCA_DO_RATO && pegou_queijo) {
    gameState = GAME_STATE_RETRY;
    return;
}
	

	// GAMEOBJECT_wrap_screen(&player);
	// GAMEOBJECT_clamp_screen(&player);
	
	// update VDP/SGDK
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	SPR_setPosition(player.sprite, fix16ToInt(player.x), fix16ToInt(player.y));
	SPR_setAnim(player.sprite, player.anim);
}

////////////////////////////////////////////////////////////////////////////
// PLATFORMER INPUT

static inline bool on_ground() {
    return LEVEL_collision_result() & COLLISION_BOTTOM;
}
static inline void PLAYER_get_input_platformer() {
    if (key_down(JOY_1, BUTTON_RIGHT)) {
        player.speed_x = PLAYER_SPEED;
        player.anim = 1; // Andando para a direita
    }
    else if (key_down(JOY_1, BUTTON_LEFT)) {
        player.speed_x = -PLAYER_SPEED;
        player.anim = 2; // Andando para a esquerda
    } else {
        player.speed_x = 0;
        player.anim = 0; // Idle
    }

	// if is on ground
	if (on_ground()) {
		player.speed_y = FIX16(1);
	} 


	
	// falls when jump button is released
	if (key_released(JOY_1, BUTTON_A)) {
		// if isn't on ground and is going up
		if (!on_ground()) {
			if (player.speed_y < 0 && player.speed_y < FIX16(-2.4)) {
				player.speed_y = FIX16(-2.4);
			}
		}
	}

	// jump
	if (key_pressed(JOY_1, BUTTON_A)) {
		if (on_ground()) {
			player.speed_y = FIX16(-4);
		}
	}

	// limit gravity down speed
	player.speed_y += FIX16(0.15);
	if (player.speed_y > FIX16(4)) {
		player.speed_y = FIX16(4);
	}

// 	if (player.speed_y < 0) 
// 		player.anim = 2;
// 	else if (player.speed_y > 0) 
// 		player.anim = 6;
}
