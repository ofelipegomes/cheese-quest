#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <genesis.h>
#include "gameobject.h"
#include "globals.h"
#include "resources.h"
#include "utils.h"

#define COLLISION_LEFT   0b0001
#define COLLISION_RIGHT  0b0010
#define COLLISION_HORIZ  0b0011

#define COLLISION_TOP    0b0100
#define COLLISION_BOTTOM 0b1000
#define COLLISION_VERT   0b1100

extern Map* map;
extern u8 collision_map[SCREEN_METATILES_W + OFFSCREEN_TILES*2][SCREEN_METATILES_H + OFFSCREEN_TILES*2]; // screen collision map
extern u16 tilemap_buff[SCREEN_TILES_W * SCREEN_TILES_H];

extern u8 collision_result;
extern char text[5];

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 LEVEL_init(u16 ind, u8 level);

void LEVEL_generate_screen_collision_map(u8 empty, u8 first_wall, u8 last_wall);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

inline u8 LEVEL_collision_result() {
	return collision_result;
}

inline u8 LEVEL_wallXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES] == 1;
}

inline u8 LEVEL_tileXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES];
}

inline u8 LEVEL_tileIDX16(s16 metatile_x, s16 metatile_y) {
	return collision_map[metatile_x + OFFSCREEN_TILES][metatile_y + OFFSCREEN_TILES];
}

inline u16 LEVEL_mapbuffIDX8(s16 tile_x, s16 tile_y) {
	return (tilemap_buff[tile_y * SCREEN_TILES_W + tile_x]  - map->baseTile) & TILE_INDEX_MASK;
 }

inline void LEVEL_set_mapbuffIDX8(s16 tile_x, s16 tile_y, u16 value) {
	tilemap_buff[tile_y * SCREEN_TILES_W + tile_x] = value + map->baseTile;
}

/**
 * Define valor para tile no mapa de colisão.
 * @param x Posição X em pixels.
 * @param y Posição Y em pixels.
 */
static inline void LEVEL_set_tileXY(s16 x, s16 y, u8 value) {
	collision_map[x/METATILE_W + OFFSCREEN_TILES][y/METATILE_W + OFFSCREEN_TILES] = value;
}

/**
 * Define valor para tile no mapa de colisão.
 * @param x Posição X em metatiles 16x16.
 * @param y Posição Y em metatiles 16x16.
 */
static inline void LEVEL_set_tileIDX16(s16 x, s16 y, u8 value) {
	collision_map[x + OFFSCREEN_TILES][y + OFFSCREEN_TILES] = value;
}

u8 LEVEL_check_wall(GameObject* obj);
void LEVEL_move_and_slide(GameObject* obj);

void LEVEL_remove_tileXY(s16 x, s16 y, u8 new_tile);
u8 LEVEL_update_camera(GameObject* obj);
void LEVEL_check_map_boundaries(GameObject* obj);

u8 LEVEL_current_room();


u8 LEVEL_current_room();
u16 LEVEL_get_screen_x();
u16 LEVEL_get_screen_y();


////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_print_tilemap_buff();

void LEVEL_draw_collision_map();
void LEVEL_draw_tile_map();

// DEBUG: change for the map you want to draw
inline void LEVEL_draw_map() {
	LEVEL_draw_collision_map();
	// LEVEL_draw_tile_map();
}

#endif // _LEVEL_H_