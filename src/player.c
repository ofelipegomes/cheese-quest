#include "player.h"
#include "resources.h"
#include "globals.h"

#define GRAVITY      FIX16(0.5)
#define JUMP_SPEED   FIX16(-6)
#define PLAYER_HEIGHT 32


GameObject player;

u16 PLAYER_init() {
    return GAMEOBJECT_init(&player, &spr_rato, 
        SCREEN_W/2 - spr_rato.w/2, 
        SCREEN_H/2 - spr_rato.h/2);
}




bool player_on_ground = FALSE;

void PLAYER_get_input() {
    u16 joy = JOY_readJoypad(JOY_1);

    // Movimento lateral
    if (joy & BUTTON_LEFT) {
        player.speed_x = -PLAYER_SPEED;
    } else if (joy & BUTTON_RIGHT) {
        player.speed_x = PLAYER_SPEED;
    } else {
        player.speed_x = 0;
    }

    // Pulo
    if ((joy & BUTTON_UP) && player_on_ground) {
        player.speed_y = JUMP_SPEED;
        player_on_ground = FALSE;
    }
}

// Simples detecção de chão (ajuste conforme seu cenário)
void PLAYER_apply_gravity() {
    if (!player_on_ground) {
        player.speed_y += GRAVITY;
    }
    // Usa a constante PLAYER_HEIGHT para detectar o chão
    if (player.y >= (SCREEN_H - PLAYER_HEIGHT)) {
        player.y = SCREEN_H - PLAYER_HEIGHT;
        player.speed_y = 0;
        player_on_ground = TRUE;
    }
}

void PLAYER_animate() {
    if (player.speed_x > 0) {
        SPR_setAnim(player.sprite, 2);
    }
    else if (player.speed_x < 0) {
        SPR_setAnim(player.sprite, 3);
    }

    if (player.speed_y > 0) {
        SPR_setAnim(player.sprite, 1);
    }
    else if (player.speed_y < 0) {
        SPR_setAnim(player.sprite, 0);
    }
}

void PLAYER_update() {
    PLAYER_get_input();
    PLAYER_apply_gravity();
    PLAYER_animate();

    player.x += fix16ToInt(player.speed_x);
    player.y += fix16ToInt(player.speed_y);

    GAMEOBJECT_clamp_screen(&player);

    SPR_setPosition(player.sprite, player.x, player.y);
}
