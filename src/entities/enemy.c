#include <genesis.h>
#include "enemy.h"
#include "engine/level.h"

static u16** ball_indexes;

////////////////////////////////////////////////////////////////////////////
// PRIVATE PROTOTYPES

static void frame_changed(Sprite* sprite);

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 ENEMY_load_tiles(u16 ind) {
    u16 num_tiles;
    ball_indexes = SPR_loadAllFrames(&spr_ball, ind, &num_tiles);
    
    return num_tiles;
}

void ENEMY_init(GameObject* const obj, const MapObject* const mapobj, u16 ind) {
    // convert map coord to screen coord (and fix 1 tile offset y from Tiled)
    f32 x = F32_toInt(mapobj->x) % SCREEN_W;
    f32 y = F32_toInt(mapobj->y) % SCREEN_H - 16;

    GAMEOBJECT_init(obj, &spr_ball, x, y, -4, -4, PAL_ENEMY, ind);
    obj->speed_x = F16_mul(  cosFix16(mapobj->direction * 128), mapobj->speed );
    obj->speed_y = F16_mul( -sinFix16(mapobj->direction * 128), mapobj->speed );

    // check enemy type and define behavior
    switch (mapobj->type) {
        case ENEMY_BOUNCER:
            obj->update = ENEMY_bouncer_update;
            // Define animação inicial baseada na direção inicial
            if (obj->speed_x >= 0) {
                SPR_setAnim(obj->sprite, 1);    // direita
            } else {
                SPR_setAnim(obj->sprite, 0);    // esquerda
            }
            break;
    
        case ENEMY_WARPER:
            obj->update = ENEMY_warper_update;
            SPR_setAnim(obj->sprite, 1);
            break;
            
        default:
            obj->update = ENEMY_bouncer_update;
            SPR_setAnim(obj->sprite, 0);
            kprintf("ERROR: MAPOBJECTS - unknow enemy type %d. Default to ENEMY_BOUNCER.", mapobj->type);
        }
    obj->on_hit = ENEMY_on_hit;

    SPR_setAutoTileUpload(obj->sprite, FALSE);
    SPR_setFrameChangeCallback(obj->sprite, &frame_changed);
    
    // SPR_setAnimationLoop(obj->sprite, FALSE);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void ENEMY_bouncer_update(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;

    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);

    // Verifica o que tem à frente do gato antes de se mover
    u16 tile_ahead;
    if (obj->speed_x > 0) {				// moving right
        SPR_setAnim(obj->sprite, 1);    // animação para direita
        
        // Verifica tile à frente (direita)
        tile_ahead = LEVEL_tileXY(obj->box.right + 8, obj->box.top + obj->h/2);
        
        // Para se encontrar queijo, spike, ou buraco do rato
        if (tile_ahead == IDX_CHEESE || tile_ahead == IDX_SPIKE || tile_ahead == IDX_TOCA_DO_RATO) {
            obj->speed_x = -obj->speed_x;  // inverte direção
            SPR_setAnim(obj->sprite, 0);   // muda para esquerda
        }
        // Para se bater na parede
        else if (LEVEL_wallXY(obj->box.right, obj->box.top + obj->h/2)) {
            obj->speed_x = -obj->speed_x;
            SPR_setAnim(obj->sprite, 0);
        }
    } 
    else if (obj->speed_x < 0) {		// moving left
        SPR_setAnim(obj->sprite, 0);    // animação para esquerda
        
        // Verifica tile à frente (esquerda)
        tile_ahead = LEVEL_tileXY(obj->box.left - 8, obj->box.top + obj->h/2);
        
        // Para se encontrar queijo, spike, ou buraco do rato
        if (tile_ahead == IDX_CHEESE || tile_ahead == IDX_SPIKE || tile_ahead == IDX_TOCA_DO_RATO) {
            obj->speed_x = -obj->speed_x;  // inverte direção
            SPR_setAnim(obj->sprite, 1);   // muda para direita
        }
        // Para se bater na parede
        else if (LEVEL_wallXY(obj->box.left, obj->box.top + obj->h/2)) {
            obj->speed_x = -obj->speed_x;
            SPR_setAnim(obj->sprite, 1);
        }
    }
        
    if (obj->speed_y < 0) {             // moving up
        if (LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.top)) {
            obj->speed_y = -obj->speed_y;
        }
    }
    else if (obj->speed_y > 0) {        // moving down
        if (LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.bottom)) {
            obj->speed_y = -obj->speed_y;
        }
    }

    GAMEOBJECT_bounce_off_screen(obj);
    GAMEOBJECT_set_hwsprite_position(obj);
}

void ENEMY_warper_update(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;
    
    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);
    GAMEOBJECT_wrap_screen(obj);
    // GAMEOBJECT_bounce_off_screen(obj);
    GAMEOBJECT_set_hwsprite_position(obj);

    SPR_setVisibility(obj->sprite, !SPR_getVisibility(obj->sprite));
}

void ENEMY_on_hit(GameObject* obj, u8 amount) {
    KLog("Enemy hit!");
}

// Much worst performer...
// void ENEMY_update2(GameObject* obj) {
//     obj->next_x = obj->x + obj->speed_x;
//     obj->next_y = obj->y + obj->speed_y;

//     LEVEL_move_and_slide(obj);

//     obj->x = obj->next_x;
//     obj->y = obj->next_y;

//     if (LEVEL_collision_result() & COLLISION_BOTTOM || LEVEL_collision_result() & COLLISION_TOP) {
//         obj->speed_y = -obj->speed_y;
//     }
//     if (LEVEL_collision_result() & COLLISION_LEFT || LEVEL_collision_result() & COLLISION_RIGHT) {
//         obj->speed_x = -obj->speed_x;
//     }

//     GAMEOBJECT_set_hwsprite_position(obj);
//     GAMEOBJECT_bounce_off_screen(obj);
// }

////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

static void frame_changed(Sprite* sprite) {
    // get VRAM tile index for this animation of this sprite
    u16 tileIndex = ball_indexes[sprite->animInd][sprite->frameInd];
    
    // manually set tile index for the current frame (preloaded in VRAM)
    SPR_setVRAMTileIndex(sprite, tileIndex);
}