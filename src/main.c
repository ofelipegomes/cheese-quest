#include <genesis.h>
#include <sprite_eng.h>

#include "globals.h"
#include "resources.h"
#include "gameobject.h"
#include "player.h"
#include "background.h"
#include "level.h"
#include "utils.h"

u16 ind = TILE_USER_INDEX;

u8 bg_colors_delay = 5;
const u16 const bg_color_glow[] = {0x0, 0x222, 0x444, 0x666, 0x888};


void game_init() {
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

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}
	SYS_showFrameLoad(true);
	game_init();

	SYS_doVBlankProcess();
	
	kprintf("Free RAM after Game Init: %d", MEM_getFree());

	while (true) {
		game_update();

		SPR_update();
		SYS_doVBlankProcess();
	}

	return 0;
}