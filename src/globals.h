#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <genesis.h>
#include <sprite_eng.h>

// #define DEBUG

#define MODE_SHOOTER    0
#define MODE_PLATFORMER 1
#define GAME_MODE MODE_EXPLORATION_SHOOTER

// GENERAL SCREEN CONFIG ////////////////////////////////

// maximum number of screens (rooms) in a map
#define NUMBER_OF_ROOMS 9
#define NUMBER_OF_ROOM_ROWS 3

// GENERAL SCREEN CONFIG ////////////////////////////////

#define HUD_TILES 1

#define SCREEN_W 320
#define SCREEN_H 224

#define SCREEN_W_F16 FIX16(320)
#define SCREEN_H_F16 FIX16(224)

#define SCREEN_TILES_W SCREEN_W/8
#define SCREEN_TILES_H SCREEN_H/8

// Map is made of 16x16 metatiles
#define METATILE_W 16
#define SCREEN_METATILES_W SCREEN_W/METATILE_W
#define SCREEN_METATILES_H SCREEN_H/METATILE_W

// GENERAL MAP CONFIG ///////////////////////////////////

#define MAP_METATILES_W 60
#define MAP_METATILES_H 42

#define MAP_W MAP_METATILES_W * METATILE_W
#define MAP_H MAP_METATILES_H * METATILE_W

// GENERAL VDP CONFIG ///////////////////////////////////

#define PAL_PLAYER 		PAL0
#define PAL_ENEMY 		PAL1
#define PAL_MAP 		PAL2
#define PAL_BACKGROUND 	PAL3

#define BG_BACKGROUND BG_B
#define BG_MAP        BG_A

#define NUMBER_OF_JOYPADS 2

// GENERAL PLAYER CONFIG ////////////////////////////////

#define PLAYER_SPEED   FIX16(2)
#define PLAYER_SPEED45 FIX16(0.707 * 2)

#define PLAYER_MAX_HEALTH 10

#endif