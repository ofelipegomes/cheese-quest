#include "gameobject.h"
#include "globals.h"

u16 GAMEOBJECT_init(GameObject* const obj, 
        const SpriteDefinition* const sprite, s16 x, s16 y) {
    obj->x = x;
    obj->y = y;
    obj->speed_x = 0;
    obj->speed_y = 0;
    obj->flip = FALSE;
    PAL_setPalette(PAL1,  sprite->palette->data, DMA);
    obj->sprite = SPR_addSprite(sprite, x, y, 
        TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, 1));
    obj->w = obj->sprite->definition->w;
    obj->h = obj->sprite->definition->h;

    return obj->sprite->definition->maxNumTile;
}

void GAMEOBJECT_clamp_screen(GameObject *obj) {
    if (obj->x < 0) {
        obj->x = 0;
    } else 
    if (obj->x > (SCREEN_W - obj->w)) {
        obj->x = SCREEN_W - obj->w;
    }

    if (obj->y < 0) {
        obj->y = 0;
    } else 
    if (obj->y > (SCREEN_H - obj->h)) {
        obj->y = SCREEN_H - obj->h;
    }
}

void GAMEOBJECT_wrap_screen(GameObject *obj) {
}
