#ifndef CHARACTER_TYPES_H_

#define CHARACTER_TYPES_H_

#include <ncurses.h>
#include "items/item_types.h"

typedef struct {
	WINDOW *win;
	uint8_t cursor_pos;
	uint8_t cursor_offset;
	uint8_t max_cursor_pos;
	uint8_t data_count;
	uint16_t data_capacity;
	void *data;
} popup_menu_t;

typedef struct path_node {
	int16_t x, y;
	int16_t g;
	int16_t h;
	int16_t f;
	int16_t px, py;
	bool checked;
} path_node_t;

typedef struct location {
	int x;
	int y;
	int global_x;
	int global_y;
} location_t;

typedef struct {
	int spells_selector; // current spell you are selected on
	
	int inv_selector; // current inventory item you are selected on in a category
	int inv_offset;
	
	int loot_selector; // current loot item you are selected on
	int loot_offset;
	inv_cat_t cat; // current inventory category you are selected on TODO add categories
} inventory_manager_t;

typedef struct class_data {
	class_type_t type;
	int base_strength;
	int base_dexterity;
	int base_intelligence;
	int base_constitution;
	int base_speed;
	float growth_strength;
	float growth_dexterity;
	float growth_intelligence;
	float growth_constitution;
	float growth_speed;
} class_data_t;

typedef struct class_type_map {
	const char *name;
	class_type_t value;
} class_type_map_t;

typedef struct lantern {
	int power;
	bool is_on;
	int turns_since_last_dim;
} lantern_t;

typedef struct equipment {
	item_ids_t armor_id, main_hand_id, off_hand_id, spell1_id, spell2_id, spell3_id;
	int attack_weapon; // this is the weapon currently used by the player when pressing attack
	int armor;
	int main_hand;
	bool main_hand_two_handed; // is this weapon 2 handed
	int off_hand;
	int spell1;
	int spell2;
	int spell3;
} equipment_t;

typedef struct player {
	int level;
	int xp;
	int health;
	int max_health;
	float strength;
	float dexterity;
	float intelligence;
	float constitution;
	float speed;
	int x; // each knight has it's only pos
	int y; // each knight has it's only pos
	int global_x; // pos on the map
	int global_y; // pos on the map
	item_t *inventory; // list of items in players inventory
	int inventory_count; // amount of items the player has
	item_t *nearby_loot[MAX_ITEMS_NEARBY_PLAYER];
	int nearby_loot_count;
	int mana; // amount of player currently has
	int max_mana;
	int action_points;
	class_type_t player_class;
	lantern_t lantern;
	equipment_t equipment;
	int oil;
	player_state_t state;
	popup_menu_t spell_equip_menu;
	popup_menu_t death_menu;
	
	inventory_manager_t inventory_manager;
} player_t;

typedef struct enemy {
	int health;
	int level;
	int strength;
	int dexterity;
	int intelligence;
	int constitution;
	int speed;
	int defense;
	int world;
	int x; // room pos
	int y; // room pos
	int global_x; // map pos
	int global_y; // map pos
	uint8_t range;
	enemy_type_t type;
	char name[MAX_ENEMY_NAME_LENGTH]; // name will be the type of enemy
	int action_points;
	// and the first letter will be the letter to represent them
	char symbol;
	trait_t trait;

	uint8_t knockback;
	float knockback_chance;
	float dodge_chance;

	// path finding, not saved
	// this could probably be a struct itself
	bool clist[ROOM_HEIGHT][ROOM_WIDTH]; // closed list
	path_node_t all_nodes[ROOM_HEIGHT][ROOM_WIDTH];
	path_node_t *olist; // open list
	uint16_t olist_count;
	uint16_t olist_size;
	uint16_t start_x;
	uint16_t start_y;
	uint16_t end_x;
	uint16_t end_y;
} enemy_t;

typedef struct enemy_data {
	enemy_type_t type;
	int base_strength;
	int base_dexterity;
	int base_intelligence;
	int base_constitution;
	int base_speed;
	int base_defense;
	int level;
	char symbol;
	trait_t trait;
	uint8_t range;
	uint8_t knockback;
	float knockback_chance;
	float dodge_chance;
	bool can_spawn[NUMBER_OF_BIOMES];
	int highest_level[NUMBER_OF_BIOMES];
	int lowest_level[NUMBER_OF_BIOMES];
	drop_table_t drop_table;
} enemy_data_t;

typedef struct enemy_type_map {
	const char name[MAX_TYPE_MAP_NAME_LENGTH];
	enemy_type_t value;
} enemy_type_map_t;

// this will also be used to make debuffs with negative numbers
typedef struct {
	int turns_left; // how many turns until this buff wears off
	// if turns_left is -1 the effect will never end, could be useful for
	// effects that have to be cleared by magic or armor and weapon effects
	char name[BUFF_NAME_MAX_LEN];
	enum buff_type type;
	int8_t damage;
	float flat_strength;
	float flat_dexterity;
	float flat_intelligence;
	float flat_constitution;
	float flat_speed;
	float percent_strength;
	float percent_dexterity;
	float percent_intelligence;
	float percent_constitution;
	float percent_speed;
	bool applied;
	enum target_type target_type_id;
	union {
		player_t *player;
		enemy_t *enemy;
	} target;
} buff_t;

#endif
