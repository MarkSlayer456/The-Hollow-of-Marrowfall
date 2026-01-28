#ifndef TYPES_H_

#define TYPES_H_
#include <stdbool.h>
#include "game_constants.h"
#include "items/item_types.h"
#include "character_types.h"
#include "map_types.h"
#include <SDL2/SDL.h>

#define DEBUG_LOG(fmt, ...) \
do { \
	FILE *f = fopen("debug.log", "a"); \
	if (f) { \
		fprintf(f, fmt "\n", ##__VA_ARGS__); \
		fclose(f); \
	} \
} while (0)

enum sprite_enum {
	SPRITE_BLANK,
	SPRITE_EXCLAMATION,
	SPRITE_PLAYER,
	SPRITE_DOT,
	SPRITE_POT,
	SPRITE_WALL,
	SPRITE_DOOR,
	SPRITE_FLOOR_ITEM,
	SPRITE_MUD,
	SPRITE_HOLE,
	SPRITE_STALAGMITE,
	SPRITE_TWISTED_ROOT,
	SPRITE_BAT,
	SPRITE_RAT,
	SPRITE_SKELETON,
	SPRITE_SLIME,
	SPRITE_MUD_CRAWLER,
	SPRITE_BOG_LURKER,
	SPRITE_MOSS_BEAST,
	SPRITE_DRAGON,
	SPRITE_BABY_DRAGON,
	SPRITE_GOBLIN,
	SPRITE_GHOST,
	SPRITE_LOOT_GOBLIN,
	SPRITE_JESTER,
	SPRITE_VOIDLING,
	SPRITE_MARROW_CRAWLER,
	SPRITE_VOID_MAW,
	SPRITE_MARROW_LEECH,

	// this is the last item at all times do not append after it
	SPRITE_COUNT
};

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
} SDL_Context;

/*
 * holds all world information, aside from the player
 */
typedef struct world {
	SDL_Context ctx;
	room_t *room[WORLD_HEIGHT][WORLD_WIDTH];
	unsigned int seed;
	enemy_data_t *enemy_data;
	class_data_t *class_data;
	item_data_t *item_data;
	int item_data_count;
	int trap_data_count;
	trap_data_t *trap_data;
	int messages_size; // TODO rename
	char **messages;
	int max_message_storage; // TODO rename
	int turn_order_size;
	int *turn_order; // -1 for player, index for enemies
	int room_template_count;
	room_template_t room_templates[128];
	bool is_player_turn; // this is used to save the player's turn, mostly for the save funcationality to work
	int action_points;
	bool turn_has_passed;
	uint8_t buff_size;
	uint8_t buff_count;
	buff_t *buffs;
	drop_table_t pot_drop_table;
} world_t;

typedef struct {
	SDL_Texture *texture;
	SDL_Rect rect;
	SDL_Texture *texture2;
	SDL_Rect rect2;
} SDL_Data;

typedef struct {
	SDL_Rect dst;
	SDL_Rect src;
} sprite_t;

typedef struct {
	const char *str;
	int font_size;
} const_data_menu_item_t;

typedef struct {
	char str[256];
	int font_size;
} dynamic_data_menu_item_t;

typedef struct {
	SDL_Data sdl_data;
	union {
		const_data_menu_item_t const_data;
		dynamic_data_menu_item_t dynamic_data;
	} data_type;
} menu_data_item_t;

typedef struct menu menu_t;

struct menu {
	menu_data_item_t *data;
	player_state_t *dests;
	void (*operation)(void *ctx1, void *ctx2, void *ctx3);
	void *operation_ctx1;
	void *operation_ctx2;
	void *operation_ctx3;
	int16_t menu_item_size;
	int selected;
	int selectable_size; // the number of items that are selectable
	int select_start_offset; // the offset in the array where selection starts, this avoids titles and such
	int display_max; // max number of items displayed before scrolling
	int offset;
	int offset_max;
	bool needs_redraw;
};

#endif
