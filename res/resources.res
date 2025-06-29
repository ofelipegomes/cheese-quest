SPRITE  spr_rato            "sprites/rato16-sheet_animation.png"      2 2 FAST 4 
SPRITE  spr_ball            "sprites/ball.png"      2 2 FAST 3
SPRITE  spr_cat            "sprites/gato16_animation.png"      2 2 FAST 4
IMAGE   img_hud           "hud/hud.png" BEST
IMAGE   img_background    "backgrounds/bg_clouds.png" BEST
IMAGE   img_mainmenu      "backgrounds/mainmenu.png" BEST
IMAGE   img_level_clear   "backgrounds/level_clear.png" BEST
IMAGE   img_retry         "backgrounds/retry.png" BEST

TILESET level1_tiles      "levels/level1/level_tiles.png" BEST ALL
PALETTE level1_pal        "levels/level1/level.pal"
MAP     level1_map        "levels/level1/level1_map.tmx" map_layer BEST 0

OBJECTS level1_objects    "levels/level1/level1_map.tmx" objects_layer "room:u16;x:f32;y:f32;direction:u16;speed:f16;enemy_type:u8" "sortby:room"