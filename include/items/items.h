#ifndef ITEMS_H_
#define ITEMS_H_
#include "types.h"

item_ids_t item_get_id(const char *name);

stats_t get_stat(const char *name);

enum grade get_grade(const char *name);

rarity_t get_rarity(const char *name);

double get_percent_from_grade(enum grade g);

void item_mark_as_unchanged(item_t *item);

void item_mark_as_changed(item_t *item);

// returns 1 on success and 0 on fail
int use_item(menu_t *menu, player_t *player);

void equip_spell(player_t *player, int spell_slot, int inv_index);

int handle_armor_change(player_t *player, int new_armor);
int handle_weapon_change(player_t *player, int new_weapon);
int handle_spell_one_change(player_t *player, int new_spell1);
int handle_spell_two_change(player_t *player, int new_spell2);
int handle_spell_three_change(player_t *player, int new_spell3);

void add_player_equipment_stats(player_t *player, stats_t stat, int modifier);
void sub_player_equipment_stats(player_t *player, stats_t stat, int modifier);

int use_teleport_scroll(player_t *player);
int use_health_potion(player_t *player);
int use_apple(player_t *player);
int use_chicken_dinner(player_t *player);

void remove_item(menu_t *menu, player_t *player);

void item_spawn(item_ids_t id, room_t *room, tile_t *tile, item_data_t *item_data);
item_ids_t item_generate_type(unsigned int *seed, item_data_t *item_data, biome_t biome);

int tile_contains_item(tile_t *tile, item_ids_t item_id);

void drop_item(tile_t *tile, item_data_t *item_data, item_ids_t item_id, int quantity);

#endif
