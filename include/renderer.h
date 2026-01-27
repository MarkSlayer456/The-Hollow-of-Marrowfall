#ifndef RENDERER_H_

#define RENDERER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "types.h"

SDL_Context SDL_setup();

void init_main_menu(SDL_Context *ctx, SDL_Data *text);

void render_main_menu(SDL_Context *ctx, SDL_Data *text);

void load_textures(SDL_Context *ctx);

enum sprite_enum get_sprite_from_char(char c);

void render_game(SDL_Context *ctx, world_t *world,  player_t *player);


void render_game_hud(SDL_Context *ctx, world_t *world, player_t *player);

#endif
