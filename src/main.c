/**
 * WINDOWS
 * ==================================================================
 * Para compilar & rodar (com extensão Genesis Code):
 * ----------------------------------------------------
 *  OBS: instalar extensao Genesis Code e configurar "Gens Path"
 *
 *  Executar: $ cmd									<< pelo terminal do VSCode (extensão não funciona em PowerShell)
 *  F1 -> Genesis Code Compile Project				<< compilar
 *  F1 -> Genesis Code Compiler & Run Project		<< compilar & executar
 * -----------------------------------------------------
 * 
 * LINUX
 * ==================================================================
 * Considerando que o caminho seja ~/sgdk200, para fazer build:
 * 
 * $ make GDK=~/sgdk200 -f ~/sgdk200/makefile_wine.gen
 */

#include <genesis.h>
#include <sprite_eng.h>

#include "resources.h"
#include "gameobject.h"
#include "player.h"

u16 spr_ind = 1;

void game_init() {
	VDP_setScreenWidth320();
	SPR_init();

	VDP_drawImageEx(BG_A, &img_bg, TILE_ATTR_FULL(PAL0, 0, 0, 0, 1), 0, 0, true, DMA);
	spr_ind += PLAYER_init();

	// inits dos inimigos, obstáculos, eventos, etc..
}

void game_update() {
	PLAYER_update();
	
	// updates dos inimigos, obstáculos, eventos, etc..
}

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}
	game_init();
	
	SYS_doVBlankProcess();
	
	while (true) {
		game_update();
		
		// update hardware sprites table
		SPR_update();	
		
		// wait for VBLANK
		SYS_doVBlankProcess();
	}

	return 0;
}
