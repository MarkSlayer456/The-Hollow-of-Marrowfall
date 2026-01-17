#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "renderer.h"
#include "game_constants.h"
#include "types.h"

sprite_t sprites[SPRITE_COUNT] = {
	[SPRITE_BLANK] = {.dst = {0}, .src = { .x = 0, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_EXCLAMATION] = { .dst = {0}, .src = { .x = 7, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_PLAYER] = { .dst = {0}, .src = { .x = 35, .y = 45, .w = 7, .h = 9 }},
	[SPRITE_DOT] = { .dst = {0}, .src = { .x = 98, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_POT] = { .dst = {0}, .src = { .x = 42, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_MUD] = { .dst = {0}, .src = { .x = 28, .y = 45, .w = 7, .h = 9 }},
	[SPRITE_HOLE] = { .dst = {0}, .src = { .x = 112, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_STALAGMITE] = { .dst = {0}, .src = { .x = 56, .y = 27, .w = 7, .h = 9 }},
	[SPRITE_TWISTED_ROOT] = { .dst = {0}, .src = { .x = 14, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_WALL] = { .dst = {0}, .src = { .x = 21, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_DOOR] = { .dst = {0}, .src = { .x = 105, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_FLOOR_ITEM] = { .dst = {0}, .src = { .x = 70, .y = 0, .w = 7, .h = 9 }},
	[SPRITE_BAT] = { .dst = {0}, .src = { .x = 84, .y = 27, .w = 7, .h = 9 }},
	[SPRITE_RAT] = { .dst = {0}, .src = { .x = 70, .y = 36, .w = 7, .h = 9 }},
	[SPRITE_SKELETON] = { .dst = {0}, .src = { .x = 105, .y = 18, .w = 7, .h = 9 }},
	[SPRITE_SLIME] = { .dst = {0}, .src = { .x = 77, .y = 36, .w = 7, .h = 9 }},
	[SPRITE_MUD_CRAWLER] = { .dst = {0}, .src = { .x = 91, .y = 27, .w = 7, .h = 9 }},
	[SPRITE_BOG_LURKER] = { .dst = {0}, .src = { .x = 112, .y = 18, .w = 7, .h = 9 }},
	[SPRITE_MOSS_BEAST] = { .dst = {0}, .src = { .x = 63, .y = 27, .w = 7, .h = 9 }},
	[SPRITE_DRAGON] = { .dst = {0}, .src = { .x = 0, .y = 18, .w = 7, .h = 9 }},
	[SPRITE_BABY_DRAGON] = { .dst = {0}, .src = { .x = 84, .y = 27, .w = 7, .h = 9 }},
	[SPRITE_GOBLIN] = { .dst = {0}, .src = { .x = 21, .y = 18, .w = 7, .h = 9 }},
	[SPRITE_GHOST] = { .dst = {0}, .src = { .x = 126, .y = 27, .w = 7, .h = 9 }},
	[SPRITE_LOOT_GOBLIN] = { .dst = {0}, .src = { .x = 84, .y = 36, .w = 7, .h = 9 }},
	[SPRITE_JESTER] = { .dst = {0}, .src = { .x = 42, .y = 18, .w = 7, .h = 9 }},
	[SPRITE_VOIDLING] = { .dst = {0}, .src = { .x = 98, .y = 36, .w = 7, .h = 9 }},
	[SPRITE_MARROW_CRAWLER] = { .dst = {0}, .src = { .x = 119, .y = 9, .w = 7, .h = 9 }},
	[SPRITE_VOID_MAW] = { .dst = {0}, .src = { .x = 0, .y = 36, .w = 7, .h = 9 }},
	[SPRITE_MARROW_LEECH] = { .dst = {0}, .src = { .x = 56, .y = 18, .w = 7, .h = 9 }},
};

const int sdl_sprite_map_len = sizeof(sprites) / sizeof(sprites[0]);

SDL_Context SDL_setup() {
	SDL_Context ctx = {0};
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		DEBUG_LOG("SDL_Init Error: %s", SDL_GetError());
		return ctx;
	}
	if(TTF_Init() == -1) {
		DEBUG_LOG("TTF_Init Error: %s", TTF_GetError());
		SDL_Quit();
		return ctx;
	}
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		DEBUG_LOG("IMG_Init Error: %s", IMG_GetError());
		return ctx;
	}

	ctx.window = SDL_CreateWindow("The Hollow of Marrowfall", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(ctx.window  == NULL) {
		DEBUG_LOG("SDL_CreateWindow Error: %s\n", SDL_GetError());
		return ctx;
	}

	ctx.renderer = SDL_CreateRenderer(ctx.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ctx.renderer == NULL) {
		DEBUG_LOG("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(ctx.window);
		TTF_Quit();
		SDL_Quit();
		return ctx;
	}
	SDL_RenderSetLogicalSize(ctx.renderer , WINDOW_WIDTH, WINDOW_HEIGHT);

	return ctx;
}

void load_textures(SDL_Context *ctx) {
	SDL_Surface *surface = IMG_Load("./data/ascii.png");
	if(!surface) DEBUG_LOG("IMG_Load Error: %s", IMG_GetError());
	ctx->texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
	if(!ctx->texture) DEBUG_LOG("SDL_Texture Error: %s", SDL_GetError());
	SDL_FreeSurface(surface);
}

enum sprite_enum get_sprite_from_char(char c) {
	switch(c) {
		case WALL:
			return SPRITE_WALL;
		case POT:
			return SPRITE_POT;
		case MUD:
			return SPRITE_MUD;
		case TWISTED_ROOT:
			return SPRITE_TWISTED_ROOT;
		case STALAGMITE:
			return SPRITE_STALAGMITE;
		case HOLE:
			return SPRITE_HOLE;
		case ITEM_SYMBOL:
			return SPRITE_FLOOR_ITEM;
		case DOOR:
			return SPRITE_DOOR;
		case EMPTY:
			return SPRITE_DOT;
		case 'b':
			return SPRITE_BAT;
		case 'r':
			return SPRITE_RAT;
		case 'S':
			return SPRITE_SKELETON;
		case 's':
			return SPRITE_SLIME;
		case 'c':
			return SPRITE_MUD_CRAWLER;
		case 'M':
			return SPRITE_MOSS_BEAST;
		case 'B':
			return SPRITE_BOG_LURKER;
		case 'D':
			return SPRITE_DRAGON;
		case 'd':
			return SPRITE_BABY_DRAGON;
		case 'g':
			return SPRITE_GOBLIN;
		case 'G':
			return SPRITE_GHOST;
		case 't':
			return SPRITE_LOOT_GOBLIN;
		case 'J':
			return SPRITE_JESTER;
		case 'v':
			return SPRITE_VOIDLING;
		case 'C':
			return SPRITE_MARROW_CRAWLER;
		case 'V':
			return SPRITE_VOID_MAW;
		case 'L':
			return SPRITE_MARROW_LEECH;
	}
	return SPRITE_BLANK;
}

void render_game(SDL_Context *ctx, world_t *world, player_t *player) {
	room_t *room = world->room[player->global_x][player->global_y];
	for(int i = 0; i < ROOM_HEIGHT; i++) {
		for(int j = 0; j < ROOM_WIDTH; j++) {
			if(room->tiles[i][j]->has_light == false) {
				continue;
			}
			int enemyIsThere = 0;
			int playerIsThere = 0;
			int potIsThere = 0;
			for(int u = 0; u < room->current_enemy_count; u++) {
				if(room->enemies[u] == NULL) continue;
				if(room->enemies[u]->x == j && room->enemies[u]->y == i) {
					enum sprite_enum sprite = get_sprite_from_char(room->enemies[u]->symbol);
					sprites[sprite].dst.x = j*22;
					sprites[sprite].dst.y = i*28;
					sprites[sprite].dst.w = 21;
					sprites[sprite].dst.h = 27;
					SDL_RenderCopy(ctx->renderer, ctx->texture, &sprites[sprite].src, &sprites[sprite].dst);
					enemyIsThere = 1;
					break;
				}
			}
			for(int u = 0; u < room->current_pot_count; u++) {
				if(room->pots[u].broken) continue;
				if(room->pots[u].x == j && room->pots[u].y == i) {
					potIsThere = 1;
					sprites[SPRITE_POT].dst.x = j*22;
					sprites[SPRITE_POT].dst.y = i*28;
					sprites[SPRITE_POT].dst.w = 21;
					sprites[SPRITE_POT].dst.h = 27;
					SDL_RenderCopy(ctx->renderer, ctx->texture, &sprites[SPRITE_POT].src, &sprites[SPRITE_POT].dst);
					break;
				}
			}

			if(player->x == j && player->y == i) {
				playerIsThere = 1;
			}

			for(int k = 0; k < MAX_ITEMS_PER_TILE; k++) {
				if(room->tiles[i][j]->items[k] != NULL && room->tiles[i][j]->items[k]->stack > 0) {
					sprites[SPRITE_FLOOR_ITEM].dst.x = j*22;
					sprites[SPRITE_FLOOR_ITEM].dst.y = i*28;
					sprites[SPRITE_FLOOR_ITEM].dst.w = 21;
					sprites[SPRITE_FLOOR_ITEM].dst.h = 27;
					SDL_RenderCopy(ctx->renderer, ctx->texture, &sprites[SPRITE_FLOOR_ITEM].src, &sprites[SPRITE_FLOOR_ITEM].dst);
					enemyIsThere = 1;
					break;
				}
			}

			if(!playerIsThere && !enemyIsThere && !potIsThere) {
				enum sprite_enum sprite = get_sprite_from_char(room->tiles[i][j]->floor);
				sprites[sprite].dst.x = j*22;
				sprites[sprite].dst.y = i*28;
				sprites[sprite].dst.w = 21;
				sprites[sprite].dst.h = 27;
				SDL_RenderCopy(ctx->renderer, ctx->texture, &sprites[sprite].src, &sprites[sprite].dst);
			}
		}
	}
	sprites[SPRITE_PLAYER].dst.x = player->x*22;
	sprites[SPRITE_PLAYER].dst.y = player->y*28;
	sprites[SPRITE_PLAYER].dst.w = 21;
	sprites[SPRITE_PLAYER].dst.h = 27;
	SDL_RenderCopy(ctx->renderer, ctx->texture, &sprites[SPRITE_PLAYER].src, &sprites[SPRITE_PLAYER].dst);
}

