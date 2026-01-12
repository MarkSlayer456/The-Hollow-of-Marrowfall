#ifndef MAP_TYPES_H_
#define MAP_TYPES_H_

#include "items/item_types.h"

#include <stdint.h>

typedef struct {
	const char name;
	float value;
} float_map_t;

typedef struct {
	int8_t x;
	int8_t y;
	bool broken;
} pot_t;

typedef struct {
	enum trap_id id;
	char name[32]; // need name for messages
	uint8_t effect_duration;
	int8_t damage; // this isn't unsigned because it could heal
	int8_t damage_each_turn; // same here
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
	char symbol;
	enum buff_type buff_type;
	bool break_on_trigger; // does this get removed after triggered
} trap_data_t;

typedef struct room_template {
	char filename[128];
	uint8_t mask;
	biome_t biome;
} room_template_t;

typedef struct tile {
	char floor; // what the floor tile is
	bool has_light; // does this tile have light
	int8_t item_count; // number of items on this tile
	item_t *items[MAX_ITEMS_PER_TILE]; // list of items on this tile
} tile_t;

typedef struct room {
	char room_file_name[ROOM_FILE_NAME_MAX_SIZE];
	tile_t *tiles[ROOM_WIDTH][ROOM_HEIGHT];
	int8_t current_enemy_count;
	enemy_t *enemies[MAX_ENEMIES_PER_LEVEL];
	int8_t current_pot_count;
	pot_t pots[MAX_POTS_PER_ROOM];
	uint8_t deleted_trap_count;
	uint8_t deleted_trap_x[MAX_TRAPS_PER_ROOM];
	uint8_t deleted_trap_y[MAX_TRAPS_PER_ROOM];
	bool is_created;
	int global_time;
	biome_t biome;
	bool is_main_path; // is main path
	uint8_t door_mask;
} room_t;

#endif
