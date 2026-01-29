#ifndef MENU_H_
#define MENU_H_
#include "enums.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

void main_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx);

void class_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx);

void load_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx);

void inventory_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color);

void inventory_update_textures(player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color);

void loot_inventory_menu_init(world_t *world, player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color);

void loot_inventory_update_textures(player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color);

void spell_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx);

void spell_update_textures(player_t *player, menu_t *menu, SDL_Context *ctx);

void hud_init(player_t *player, hud_t *hud, SDL_Context *ctx);

void hud_update_textures(world_t *world, player_t *player, hud_t *hud, SDL_Context *ctx);

void menu_use_item(void *ctx1, void *ctx2, void *ctx3);

void menu_loot_item(void *ctx1, void *ctx2, void *ctx3);

void confirm_class(void *ctx1, void *ctx2, void *ctx3);

void go_to_dest(void *ctx1, void *ctx2, void *ctx3);

void menu_cursor_up(menu_t *menu);

void menu_cursor_down(menu_t *menu);

void menu_update_cursor_pos(menu_t *menu);

void menu_render(const menu_t *menu, SDL_Renderer *renderer);


void ui_render(ui_data_t *data, int16_t data_size, SDL_Renderer *renderer);

#endif
