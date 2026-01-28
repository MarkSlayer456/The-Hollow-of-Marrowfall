#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "renderer.h"
#include "game_constants.h"
#include "types.h"
#include "player.h"

#define MAX(a, b) a > b ? a : b

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
	[SPRITE_MOSS_BEAST] = { .dst = {0}, .src = { .x = 63, .y = 18, .w = 7, .h = 9 }},
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

void render_game_hud(SDL_Context *ctx, world_t *world, player_t *player, int *actor) {
	room_t *room = world->room[player->global_x][player->global_y];
	int start_x = 22*20+1;
	int cur_y = 25;
	//TODO textures shouldn't be created every frame, probably want a hud object to store
	// textures, and only update the texture if a value changes

	TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", 16);
	if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
	SDL_Color white = {255, 255, 255, 255};

	char buf[256];
	snprintf(buf, sizeof(buf), "Class: %s", class_get_name(player->player_class));
	SDL_Rect class_rect = {.x = start_x, .y = cur_y};


	SDL_Surface *class_surface = TTF_RenderText_Solid(font, buf, white);
	if(!class_surface) DEBUG_LOG("%s", "surface error");
	SDL_Texture *class = SDL_CreateTextureFromSurface(ctx->renderer, class_surface);

	class_rect.w = class_surface->w;
	class_rect.h = class_surface->h;
	cur_y += class_rect.h+1;

	snprintf(buf, sizeof(buf), "Health %d / %d | Mana: %d / %d | Level: %d | XP: %d / %d", player->health, player->max_health, player->mana, player->max_mana, player->level, player->xp, xp_to_level_up(player->level));
	SDL_Rect level_rect = {.x = start_x, .y = cur_y};

	SDL_Surface *level_surface = TTF_RenderText_Solid(font, buf, white);
	if(!level_surface) DEBUG_LOG("%s", "surface error");
	SDL_Texture *level = SDL_CreateTextureFromSurface(ctx->renderer, level_surface);

	level_rect.w = level_surface->w;
	level_rect.h = level_surface->h;
	cur_y += level_rect.h+1;

	snprintf(buf, sizeof(buf), "Str: %d | Dex: %d | Int: %d | Const: %d | Spd: %d", (int)player->strength, (int)player->dexterity, (int)player->intelligence, (int)player->constitution, (int)player->speed);
	SDL_Rect stats_rect = {.x = start_x, .y = cur_y};

	SDL_Surface *stats_surface = TTF_RenderText_Solid(font, buf, white);
	if(!stats_surface) DEBUG_LOG("%s", "surface error");
	SDL_Texture *stats = SDL_CreateTextureFromSurface(ctx->renderer, stats_surface);

	stats_rect.w = stats_surface->w;
	stats_rect.h = stats_surface->h;
	cur_y += stats_rect.h+1;

	if(player->equipment.attack_weapon >= 0) {
		snprintf(buf, sizeof(buf), "Attack Weapon: %s | Oil: %d", player->inventory[player->equipment.attack_weapon].name, player->oil);
	} else {
		snprintf(buf, sizeof(buf), "Attack Weapon: Unarmed | Oil: %d", player->oil);
	}
	SDL_Rect weapon_rect = {.x = start_x, .y = cur_y};


	SDL_Surface *weapon_surface = TTF_RenderText_Solid(font, buf, white);
	if(!weapon_surface) DEBUG_LOG("%s", "surface error");
	SDL_Texture *weapon = SDL_CreateTextureFromSurface(ctx->renderer, weapon_surface);

	weapon_rect.w = weapon_surface->w;
	weapon_rect.h = weapon_surface->h;
	cur_y += weapon_rect.h+1;

	memset(buf, 0, sizeof(buf));

	int max_status_effects = 4;
	strcat(buf, "Status: ");
	int status_count = 0;
	for(int i = 0; i < world->buff_count; i++) {
		if(status_count == max_status_effects) break;
		if(world->buffs[i].target_type_id == TARGET_PLAYER) {
			if(status_count != 0) {
				strcat(buf, ", ");
			}
			strcat(buf, world->buffs[i].name);
			strcat(buf, ":");
			char turns_left[8];
			snprintf(turns_left, sizeof(turns_left), "%d", world->buffs[i].turns_left);
			strcat(buf, turns_left);
			status_count++;
		}
	}

	SDL_Rect status_rect = {.x = start_x, .y = cur_y};

	SDL_Surface *status_surface = TTF_RenderText_Solid(font, buf, white);
	if(!status_surface) DEBUG_LOG("%s", "surface error");
	SDL_Texture *status = SDL_CreateTextureFromSurface(ctx->renderer, status_surface);

	status_rect.w = status_surface->w;
	status_rect.h = status_surface->h;
	cur_y += status_rect.h+1;

	memset(buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf), "Room Location: (%d, %d)", player->global_x, player->global_y);

	SDL_Rect room_rect = {.x = start_x, .y = cur_y};

	SDL_Surface *room_surface = TTF_RenderText_Solid(font, buf, white);
	if(!room_surface) DEBUG_LOG("%s", "surface error");
	SDL_Texture *room_texture = SDL_CreateTextureFromSurface(ctx->renderer, room_surface);
	room_rect.w = room_surface->w;
	room_rect.h = room_surface->h;

	cur_y += room_rect.h+1;

	int pos = snprintf(buf, sizeof(buf), "Turn Order: ");
	if(*actor == PLAYER_TURN_ORDER_INDEX) {
		pos += snprintf(buf + pos, sizeof(buf)-pos, "[%c] ", 'P');
	}
	for(int i = 0; i < world->turn_order_size; i++) {
		if(world->turn_order[i] == PLAYER_TURN_ORDER_INDEX) {
			pos += snprintf(buf + pos, sizeof(buf)-pos, "[%c] ", 'P');
			continue;
		}
		if(world->turn_order[i] == WORLD_TURN_ORDER_INDEX) {
			pos += snprintf(buf + pos, sizeof(buf)-pos, "[%c] ", 'W');
			continue;
		}
		enemy_t *enemy = room->enemies[world->turn_order[i]];
		if(enemy != NULL && enemy->type != ENEMY_NONE) {
			if(room->tiles[enemy->y][enemy->x]->has_light) {
				pos += snprintf(buf + pos, sizeof(buf)-pos, "[%c] ", enemy->symbol);
			} else {
				pos += snprintf(buf + pos, sizeof(buf)-pos, "[?] ");
			}
		}
	}

	SDL_Rect turn_rect = {.x = start_x, .y = cur_y};

	SDL_Surface *turn_surface = TTF_RenderText_Solid(font, buf, white);
	if(!turn_surface) DEBUG_LOG("%s", "surface error");
	SDL_Texture *turn = SDL_CreateTextureFromSurface(ctx->renderer, turn_surface);

	turn_rect.w = turn_surface->w;
	turn_rect.h = turn_surface->h;

	cur_y += turn_rect.h+1;

	int detect_radius = MAX(2, player->lantern.power);

	for(int i = 0; i < room->current_enemy_count; i++) {
		if(!room->enemies[i]) continue;
		if(room->enemies[i]->x > player->x-detect_radius && room->enemies[i]->x < player->x+detect_radius &&
			room->enemies[i]->y > player->y-(detect_radius) && room->enemies[i]->y < player->y+(detect_radius)) {
			snprintf(buf, sizeof(buf), "%s : %d", room->enemies[i]->name, room->enemies[i]->health);

			SDL_Rect enemy_rect = {.x = start_x, .y = cur_y};


			SDL_Surface *enemy_surface = TTF_RenderText_Solid(font, buf, white);
			if(!enemy_surface) DEBUG_LOG("%s", "surface error");
			SDL_Texture *enemey_texture = SDL_CreateTextureFromSurface(ctx->renderer, enemy_surface);

			enemy_rect.w = enemy_surface->w;
			enemy_rect.h = enemy_surface->h;
			cur_y += enemy_rect.h+1;


			SDL_RenderCopy(ctx->renderer, enemey_texture, NULL, &enemy_rect);
			SDL_FreeSurface(enemy_surface);
		}
	}


	SDL_RenderCopy(ctx->renderer, class, NULL, &class_rect);
	SDL_RenderCopy(ctx->renderer, level, NULL, &level_rect);
	SDL_RenderCopy(ctx->renderer, stats, NULL, &stats_rect);
	SDL_RenderCopy(ctx->renderer, weapon, NULL, &weapon_rect);
	SDL_RenderCopy(ctx->renderer, status, NULL, &status_rect);
	SDL_RenderCopy(ctx->renderer, room_texture, NULL, &room_rect);
	SDL_RenderCopy(ctx->renderer, turn, NULL, &turn_rect);
	SDL_FreeSurface(class_surface);
	SDL_FreeSurface(level_surface);
	SDL_FreeSurface(stats_surface);
	SDL_FreeSurface(weapon_surface);
	SDL_FreeSurface(status_surface);
	SDL_FreeSurface(room_surface);
	SDL_FreeSurface(turn_surface);
	TTF_CloseFont(font);
}

