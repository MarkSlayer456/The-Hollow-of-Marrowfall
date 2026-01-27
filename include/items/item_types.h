#ifndef ITEM_TYPES_H_
#define ITEM_TYPES_H_

#include <stdint.h>
#include "item_enums.h"
#include "enums.h"
#include "game_constants.h"



typedef struct type_map {
	const char name[MAX_TYPE_MAP_NAME_LENGTH];
	int16_t value;
} type_map_t;

typedef struct stat_modifier {
	int16_t modifier;
	stats_t stat;
} stat_modifier_t;

typedef struct {
	int16_t min_damage;
	int16_t max_damage;
	int8_t mana_cost;
	int8_t range;
	double crit_chance;
	double status_chance;
	int16_t buff_duration;
	int16_t buff_min_damage;
	int16_t buff_max_damage;
	enum grade stat_grade;
	bool equipped;
	bool applies_buff;
	enum buff_type buff_type;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
} spell_stats_t;

typedef struct armor_stats {
	armor_type_t type;
	int16_t defense;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
	bool equipped;
} armor_stats_t;

typedef struct weapon_stats {
	int16_t max_attack;
	int16_t min_attack;
	stats_t scaling_stat1;
	stats_t scaling_stat2;
	enum grade stat1_grade;
	enum grade stat2_grade;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
	double crit_chance;
	bool equipped;
	bool two_handed;
	bool main_hand;
	uint8_t range;
} weapon_stats_t;

typedef struct food_stats {
	int16_t heal_amount;
	int16_t mana_heal_amount;
	stat_modifier_t modifier_stats[MAX_ARMOR_MODIFIERS];
	int8_t modifier_count;
	int16_t duration;
} food_stats_t;

typedef struct scroll_stats {
	int tmp; // this is to avoid errors when compiling get rid of
	// TODO 
} scroll_stats_t;

typedef struct item {
	char name[MAX_ITEM_NAME_LENGTH];
	char desc[MAX_ITEM_DESC_LEN];
	item_ids_t id;
	int stack;
	bool has_changed;
	value_type_t value_type;
	union {
		armor_stats_t armor;
		weapon_stats_t weapon;
		food_stats_t food;
		scroll_stats_t scroll;
		spell_stats_t spell;
	} stat_type;
} item_t;

typedef struct item_data {
	char name[MAX_ITEM_NAME_LENGTH];
	char desc[MAX_ITEM_DESC_LEN];
	item_ids_t id;
	value_type_t value_type;
	rarity_t rarity[NUMBER_OF_BIOMES];
	bool spawn_biomes[NUMBER_OF_BIOMES];
	union {
		armor_stats_t armor;
		weapon_stats_t weapon;
		food_stats_t food;
		scroll_stats_t scroll;
		spell_stats_t spell;
	} stat_type;
} item_data_t;

typedef struct {
	item_ids_t id;
	float drop_chance;
	int min_quantity;
	int max_quantity;
} item_drop_t;

typedef struct {
	item_drop_t *drops;
	int drop_count;
} drop_table_t;

#endif
