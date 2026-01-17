#ifndef MENU_H_
#define MENU_H_
#include "enums.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

void main_menu_init(menu_t *menu, SDL_Context *ctx);

void menu_handle_input(menu_t menu[MENU_COUNT], enum menu menu_id,  SDL_Event event);

void menu_update_cursor_pos(const menu_t *menu);

void menu_render(const menu_t *menu, SDL_Renderer *renderer);

#endif
