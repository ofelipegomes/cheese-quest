#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <genesis.h>
#include <sprite_eng.h>

typedef struct {
    Sprite* sprite;
    s16 x;
    s16 y;
    f16 speed_x;
    f16 speed_y;
    u8 w;
    u8 h;
    u8 flip;     // bool
} GameObject;

u16 GAMEOBJECT_init(GameObject* const obj, 
    const SpriteDefinition* const sprite, s16 x, s16 y);

void GAMEOBJECT_clamp_screen(GameObject* obj);
void GAMEOBJECT_wrap_screen(GameObject* obj);

#endif