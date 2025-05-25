#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include <sprite_eng.h>
#include "gameobject.h"

#define PLAYER_SPEED FIX16(2)

extern GameObject player;

u16 PLAYER_init();

void PLAYER_get_input();
void PLAYER_update();
void PLAYER_animate();

#endif
