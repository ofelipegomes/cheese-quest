#include "gameobject.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////
// INIT

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, s8 w_offset, s8 h_offset, u8 pal, u16 ind) {
	obj->active = TRUE;
	obj->x = FIX16(x);
	obj->y = FIX16(y);
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->speed_x = 0;
	obj->speed_y = 0;
	obj->anim = 0;
	obj->speed = 0;
	PAL_setPalette(pal, sprite->palette->data, DMA);
	
	obj->sprite = SPR_addSprite(sprite, x, y, TILE_ATTR_FULL(pal, FALSE, FALSE, 0, ind));

	obj->w = obj->sprite->definition->w + w_offset;
	obj->h = obj->sprite->definition->h + h_offset;
	obj->w_offset = w_offset/2; // half offset for each side
	obj->h_offset = h_offset/2;
	
	return obj->sprite->definition->maxNumTile;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

u8 GAMEOBJECT_check_collision(GameObject* obj1, GameObject* obj2) {
	GAMEOBJECT_update_boundbox(obj1->x, obj1->y, obj1);
	GAMEOBJECT_update_boundbox(obj2->x, obj2->y, obj2);

	return !((obj1->box.left > obj2->box.right) ||
	         (obj2->box.left > obj1->box.right) ||
		     (obj1->box.top > obj2->box.bottom) ||
		     (obj2->box.top > obj1->box.bottom));
}

/**
 * Updates GameObject's bound box (integer values) from positions and size (fix16).
 */
void GAMEOBJECT_update_boundbox(f16 x, f16 y, GameObject* obj) {
	obj->box.left  = F16_toInt(x);
	obj->box.top   = F16_toInt(y);
	obj->box.right = F16_toInt(x) + obj->w;// - 1;
	obj->box.bottom= F16_toInt(y) + obj->h;// - 1;
}

/**
 *	Prevents object from going outside screen by repositing it at screen bounds.
*/
void GAMEOBJECT_clamp_screen(GameObject* obj) {
	obj->x = clamp(obj->x, 0, FIX16(SCREEN_W - obj->w));
	obj->y = clamp(obj->y, 0, FIX16(SCREEN_H - obj->h));
}

/**
 * Wraps object around screen bounds.
 */
void GAMEOBJECT_wrap_screen(GameObject* obj) {
	if (obj->box.left < -obj->w/2) {
		obj->x += SCREEN_W_F16;
	} else
	if (obj->box.left > SCREEN_W - obj->w/2) {
		obj->x -= SCREEN_W_F16;
	}

	if (obj->box.top < -obj->h/2) {
		obj->y += SCREEN_H_F16;
	} else
	if (obj->box.top > SCREEN_H - obj->h/2) {
		obj->y -= SCREEN_H_F16;
	}
}

/**
 * Prevents object from going outside screen by reversing the speed sign (x or y)at screen bounds.
 * OBS: The GameObject's Box must be updated before calling this function.
 */
void GAMEOBJECT_bounce_off_screen(GameObject* obj) {
	// bounce off screen bounds
	if (obj->box.left < 0 || obj->box.right > SCREEN_W) {
		obj->speed_x = -obj->speed_x;
	}	

	if (obj->box.top < 0 || obj->box.bottom > SCREEN_H) {
		obj->speed_y = -obj->speed_y;
	}	
}
