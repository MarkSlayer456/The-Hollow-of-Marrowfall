#include "save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "map_manager.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

const char *get_save_path() {
	//TODO save file loaction nedds changed
	#ifdef _WIN32
	static char path[512];
	snprintf(path, sizeof(path), "%s\\CopyKnights\\save.bin", getenv("APPDATA"));
	return path;
	#else
	static char path[512];
	snprintf(path, sizeof(path), "%s/.CopyKnights/", getenv("HOME"));
	return path;
	#endif
}

void ensure_save_folder() {
	#ifdef _WIN32
	char folder[512];
	strcpy(folder, get_save_path());
	// Remove the filename
	char *last_slash = strrchr(folder, '\\');
	if (last_slash) *last_slash = '\0';
	CreateDirectory(folder, NULL);
	#else
	char folder[512];
	strcpy(folder, get_save_path());
	// Remove the filename
	char *last_slash = strrchr(folder, '/');
	if (last_slash) *last_slash = '\0';
	mkdir(folder, 0755);
	#endif
}

//TODO add a data const, so this can be tracked later
int fwrite_checked(void *prt, size_t size, int count, FILE *file) {
	if(fwrite(prt, size, count, file) != count) {
		DEBUG_LOG("%s", "writing failed");
		return -1;
	}
	return 0;
}

//TODO add a data const, so this can be tracked later
int fread_checked(void *prt, size_t size, int count, FILE *file) {
	if(fread(prt, size, count, file) != count) {
		DEBUG_LOG("%s", "reading failed");
		return -1;
	}
	return 0;
}

void save_game(world_t *world, player_t *player, char *name) {
	char path[512];
	snprintf(path, sizeof(path), "%s%s", get_save_path(), name);
	FILE *file = fopen(path, "wb");
	if(!file) {
		DEBUG_LOG("saving error: failed reading file: %s", path);
	}
	save_world(world, file);
	save_player(player, file);
	fclose(file);
	DEBUG_LOG("game saved: %s", path);
}

void load_game(world_t *world, player_t *player, char *name) {
	char path[512];
	snprintf(path, sizeof(path), "%s%s", get_save_path(), name);
	FILE *file = fopen(path, "rb");
	if(!file) {
		DEBUG_LOG("loading error: failed reading file: %s", path);
	}
	load_world(world, player, file);
	load_player(player, file, world->item_data);
	fclose(file);
	DEBUG_LOG("game loaded: %s", path);
}

void save_player(player_t *player, FILE *file) {
	DEBUG_LOG("%s", "saving player");
	if(!player) {
		DEBUG_LOG("%s", "player null");
	}
	fwrite_checked(&player->level, sizeof(int), 1, file);
	fwrite_checked(&player->xp, sizeof(int), 1, file);
	fwrite_checked(&player->health, sizeof(int), 1, file);
	fwrite_checked(&player->max_health, sizeof(int), 1, file);
	fwrite_checked(&player->strength, sizeof(float), 1, file);
	fwrite_checked(&player->dexterity, sizeof(float), 1, file);
	fwrite_checked(&player->intelligence, sizeof(float), 1, file);
	fwrite_checked(&player->constitution, sizeof(float), 1, file);
	fwrite_checked(&player->speed, sizeof(float), 1, file);
	fwrite_checked(&player->x, sizeof(int), 1, file);
	fwrite_checked(&player->y, sizeof(int), 1, file);
	fwrite_checked(&player->global_x, sizeof(int), 1, file);
	fwrite_checked(&player->global_y, sizeof(int), 1, file);
	
	fwrite_checked(&player->equipment.spell1, sizeof(int), 1, file);
	fwrite_checked(&player->equipment.spell2, sizeof(int), 1, file);
	fwrite_checked(&player->equipment.spell3, sizeof(int), 1, file);

	fwrite_checked(&player->inventory_count, sizeof(int), 1, file);
	for(int i = 0; i < player->inventory_count; i++) {
		if(player->inventory[i].stack == 0) continue;
		fwrite_checked(&player->inventory[i].id, sizeof(item_ids_t), 1, file);
		fwrite_checked(&player->inventory[i].stack, sizeof(int), 1, file);
		if(player->inventory[i].value_type == VALUE_TYPE_WEAPON) {
			fwrite_checked(&player->inventory[i].stat_type.weapon.equipped, sizeof(bool), 1, file);
		} else if(player->inventory[i].value_type == VALUE_TYPE_ARMOR) {
			fwrite_checked(&player->inventory[i].stat_type.armor.equipped, sizeof(bool), 1, file);
		}
	}
	fwrite_checked(&player->nearby_loot_count, sizeof(int), 1, file);
	for(int i = 0; i < player->nearby_loot_count; i++) {
		fwrite_checked(&player->nearby_loot[i]->id, sizeof(item_ids_t), 1, file);
		fwrite_checked(&player->nearby_loot[i]->stack, sizeof(int), 1, file);
	}
	
	fwrite_checked(&player->mana, sizeof(int), 1, file);
	fwrite_checked(&player->max_mana, sizeof(int), 1, file);
	fwrite_checked(&player->action_points, sizeof(int), 1, file);
	fwrite_checked(&player->player_class, sizeof(class_type_t), 1, file);
	fwrite_checked(&player->lantern, sizeof(lantern_t), 1, file);
	
	fwrite_checked(&player->equipment.attack_weapon, sizeof(item_ids_t), 1, file);
	fwrite_checked(&player->equipment.armor_id, sizeof(item_ids_t), 1, file);
	fwrite_checked(&player->equipment.main_hand_id, sizeof(item_ids_t), 1, file);
	fwrite_checked(&player->equipment.off_hand_id, sizeof(item_ids_t), 1, file);
	fwrite_checked(&player->equipment.spell1_id, sizeof(item_ids_t), 1, file);
	fwrite_checked(&player->equipment.spell2_id, sizeof(item_ids_t), 1, file);
	fwrite_checked(&player->equipment.spell3_id, sizeof(item_ids_t), 1, file);
	fwrite_checked(&player->equipment.main_hand_two_handed, sizeof(bool), 1, file);
	
	fwrite_checked(&player->oil, sizeof(int), 1, file);
	fwrite_checked(&player->state, sizeof(player_state_t), 1, file);
	fwrite_checked(&player->inventory_manager, sizeof(inventory_manager_t), 1, file);
	
}

void save_world(world_t *world, FILE *file) {
	for(int x = 0; x < WORLD_WIDTH; x++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			if(!world->room[x][y]->is_created) {
				int invalid = -1;
				fwrite_checked(&invalid, sizeof(int), 1, file);
				fwrite_checked(&invalid, sizeof(int), 1, file);
				continue;
			}
			fwrite_checked(&x, sizeof(int), 1, file);
			fwrite_checked(&y, sizeof(int), 1, file);
			save_room(world->room[x][y], file);
		}
	}
	fwrite_checked(&world->seed, sizeof(int), 1, file);
	
	fwrite_checked(&world->messages_size, sizeof(int), 1, file);
	fwrite_checked(&world->max_message_storage, sizeof(int), 1, file);
	
	for(int i = 0; i < world->messages_size; i++) {
		int len = strlen(world->messages[i]) + 1;
		fwrite_checked(&len, sizeof(int), 1, file);
		fwrite_checked(world->messages[i], sizeof(char), len, file);
	}
	
	fwrite_checked(&world->turn_order_size, sizeof(int), 1, file);
	fwrite_checked(world->turn_order, sizeof(int), world->turn_order_size, file);
	fwrite_checked(&world->room_template_count, sizeof(int), 1, file);
	fwrite_checked(&world->room_templates, sizeof(room_template_t), world->room_template_count, file);
	fwrite_checked(&world->is_player_turn, sizeof(bool), 1, file);
	fwrite_checked(&world->buff_size, sizeof(uint8_t), 1, file);
	fwrite_checked(&world->buff_count, sizeof(uint8_t), 1, file);
	for(int i = 0; i < world->buff_count; i++) {
		save_buff(&world->buffs[i], world, file);
	}
}

void save_room(room_t *room, FILE *file) {
	fwrite_checked(room->room_file_name, sizeof(char), ROOM_FILE_NAME_MAX_SIZE, file);
	for(int x = 0; x < ROOM_WIDTH; x++) {
		for(int y = 0; y < ROOM_HEIGHT; y++) {
			// fwrite_checked(&room->tiles[y][x]->floor, sizeof(char), 1, file);
			fwrite_checked(&room->tiles[y][x]->item_count, sizeof(int8_t), 1, file);
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				fwrite_checked(&room->tiles[y][x]->items[i]->id, sizeof(item_ids_t), 1, file);
				fwrite_checked(&room->tiles[y][x]->items[i]->stack, sizeof(int), 1, file);
			}
		}
	}
	fwrite_checked(&room->current_pot_count, sizeof(int8_t), 1, file);
	for(int i = 0; i < room->current_pot_count; i++) {
		fwrite_checked(&room->pots[i].x, sizeof(int8_t), 1, file);
		fwrite_checked(&room->pots[i].y, sizeof(int8_t), 1, file);
		fwrite_checked(&room->pots[i].broken, sizeof(bool), 1, file);
	}
	fwrite_checked(&room->deleted_trap_count, sizeof(uint8_t), 1, file);
	for(int i = 0; i < room->deleted_trap_count; i++) {
		fwrite_checked(&room->deleted_trap_x[i], sizeof(uint8_t), 1, file);
		fwrite_checked(&room->deleted_trap_y[i], sizeof(uint8_t), 1, file);
	}
	fwrite_checked(&room->current_enemy_count, sizeof(int8_t), 1, file);
	for (int i = 0; i < room->current_enemy_count; i++) {
		fwrite_checked(room->enemies[i], sizeof(enemy_t), 1, file);
	}
	fwrite_checked(&room->is_created, sizeof(bool), 1, file);
	fwrite_checked(&room->global_time, sizeof(int), 1, file);
	fwrite_checked(&room->biome, sizeof(biome_t), 1, file);
	fwrite_checked(&room->is_main_path, sizeof(bool), 1, file);
	fwrite_checked(&room->door_mask, sizeof(uint8_t), 1, file);
}

void save_buff(buff_t *buff, world_t *world, FILE *file) {
	fwrite_checked(&buff->turns_left, sizeof(int), 1, file);
	fwrite_checked(&buff->name, sizeof(char), BUFF_NAME_MAX_LEN, file);
	fwrite_checked(&buff->type, sizeof(enum buff_type), 1, file);
	fwrite_checked(&buff->flat_strength, sizeof(float), 1, file);
	fwrite_checked(&buff->flat_dexterity, sizeof(float), 1, file);
	fwrite_checked(&buff->flat_intelligence, sizeof(float), 1, file);
	fwrite_checked(&buff->flat_constitution, sizeof(float), 1, file);
	fwrite_checked(&buff->flat_speed, sizeof(float), 1, file);
	fwrite_checked(&buff->percent_strength, sizeof(float), 1, file);
	fwrite_checked(&buff->percent_dexterity, sizeof(float), 1, file);
	fwrite_checked(&buff->percent_intelligence, sizeof(float), 1, file);
	fwrite_checked(&buff->percent_constitution, sizeof(float), 1, file);
	fwrite_checked(&buff->percent_speed, sizeof(float), 1, file);
	fwrite_checked(&buff->applied, sizeof(bool), 1, file);
	fwrite_checked(&buff->target_type_id, sizeof(enum target_type), 1, file);
	if(buff->target_type_id == TARGET_ENEMY) {
		fwrite_checked(&buff->target.enemy->global_x, sizeof(int), 1, file);
		fwrite_checked(&buff->target.enemy->global_y, sizeof(int), 1, file);
		fwrite_checked(&buff->target.enemy->x, sizeof(int), 1, file);
		fwrite_checked(&buff->target.enemy->y, sizeof(int), 1, file);
	}
}

void load_buff(buff_t *buff, world_t *world, player_t *player, FILE *file) {
	fread_checked(&buff->turns_left, sizeof(int), 1, file);
	fread_checked(&buff->name, sizeof(char), BUFF_NAME_MAX_LEN, file);
	fread_checked(&buff->type, sizeof(enum buff_type), 1, file);
	fread_checked(&buff->flat_strength, sizeof(float), 1, file);
	fread_checked(&buff->flat_dexterity, sizeof(float), 1, file);
	fread_checked(&buff->flat_intelligence, sizeof(float), 1, file);
	fread_checked(&buff->flat_constitution, sizeof(float), 1, file);
	fread_checked(&buff->flat_speed, sizeof(float), 1, file);
	fread_checked(&buff->percent_strength, sizeof(float), 1, file);
	fread_checked(&buff->percent_dexterity, sizeof(float), 1, file);
	fread_checked(&buff->percent_intelligence, sizeof(float), 1, file);
	fread_checked(&buff->percent_constitution, sizeof(float), 1, file);
	fread_checked(&buff->percent_speed, sizeof(float), 1, file);
	fread_checked(&buff->applied, sizeof(bool), 1, file);
	fread_checked(&buff->target_type_id, sizeof(enum target_type), 1, file);
	if(buff->target_type_id == TARGET_ENEMY) {
		int target_gx, target_gy, target_x, target_y;
		fread_checked(&target_gx, sizeof(int), 1, file);
		fread_checked(&target_gy, sizeof(int), 1, file);
		fread_checked(&target_x, sizeof(int), 1, file);
		fread_checked(&target_y, sizeof(int), 1, file);
		//TODO this is insane and probably won't even work... this is like 5 or 6 nested loops
		for(int gx = 0; gx < WORLD_WIDTH; gx++) {
			for(int gy = 0; gy < WORLD_HEIGHT; gy++) {
				room_t *room = world->room[gx][gy];
				if(!room->is_created) continue;
				for(int x = 0; x < ROOM_WIDTH; x++) {
					for(int y = 0; y < ROOM_HEIGHT; y++) {
						for(int i = 0; i < room->current_enemy_count; i++) {
							enemy_t *enemy = room->enemies[i];
							if(enemy->x == target_x && enemy->y == target_y
							&& enemy->global_x == target_gx && enemy->global_y == target_gy) {
								buff->target.enemy = enemy;
							}
						}
					}
				}
			}
		}
	} else {
		buff->target.player = player;
	}
}

void load_player(player_t *player, FILE *file, item_data_t *item_data) {
	fread_checked(&player->level, sizeof(int), 1, file);
	fread_checked(&player->xp, sizeof(int), 1, file);
	fread_checked(&player->health, sizeof(int), 1, file);
	fread_checked(&player->max_health, sizeof(int), 1, file);
	fread_checked(&player->strength, sizeof(float), 1, file);
	fread_checked(&player->dexterity, sizeof(float), 1, file);
	fread_checked(&player->intelligence, sizeof(float), 1, file);
	fread_checked(&player->constitution, sizeof(float), 1, file);
	fread_checked(&player->speed, sizeof(float), 1, file);
	fread_checked(&player->x, sizeof(int), 1, file);
	fread_checked(&player->y, sizeof(int), 1, file);
	fread_checked(&player->global_x, sizeof(int), 1, file);
	fread_checked(&player->global_y, sizeof(int), 1, file);

	fread_checked(&player->equipment.spell1, sizeof(int), 1, file);
	fread_checked(&player->equipment.spell2, sizeof(int), 1, file);
	fread_checked(&player->equipment.spell3, sizeof(int), 1, file);

	fread_checked(&player->inventory_count, sizeof(int), 1, file);
	for(int i = 0; i < player->inventory_count; i++) {
		fread_checked(&player->inventory[i].id, sizeof(item_ids_t), 1, file); // TODO this causes save files to not work if you add any items at all, should probably store this as a string
		fread_checked(&player->inventory[i].stack, sizeof(int), 1, file);
		if(player->inventory[i].stack == 0) continue;
		load_item_from_data(&player->inventory[i], item_data);
		if(player->inventory[i].value_type == VALUE_TYPE_WEAPON) {
			fread_checked(&player->inventory[i].stat_type.weapon.equipped, sizeof(bool), 1, file);
			if(player->inventory[i].stat_type.weapon.equipped) {
				if(player->inventory[i].stat_type.weapon.main_hand) {
					player->equipment.main_hand = i;
				} else {
					player->equipment.off_hand = i;
				}
			}
			
		} else if(player->inventory[i].value_type == VALUE_TYPE_ARMOR) {
			fread_checked(&player->inventory[i].stat_type.armor.equipped, sizeof(bool), 1, file);
			if(player->inventory[i].stat_type.armor.equipped) {
				player->equipment.armor = i;
			}
		}
	}
	fread_checked(&player->nearby_loot_count, sizeof(int), 1, file);
	for(int i = 0; i < player->nearby_loot_count; i++) {
		fread_checked(&player->nearby_loot[i]->id, sizeof(item_ids_t), 1, file);
		fread_checked(&player->nearby_loot[i]->stack, sizeof(int), 1, file);
	}
	
	fread_checked(&player->mana, sizeof(int), 1, file);
	fread_checked(&player->max_mana, sizeof(int), 1, file);
	fread_checked(&player->action_points, sizeof(int), 1, file);
	fread_checked(&player->player_class, sizeof(class_type_t), 1, file);
	fread_checked(&player->lantern, sizeof(lantern_t), 1, file);
	
	fread_checked(&player->equipment.attack_weapon, sizeof(item_ids_t), 1, file);
	fread_checked(&player->equipment.armor_id, sizeof(item_ids_t), 1, file);
	fread_checked(&player->equipment.main_hand_id, sizeof(item_ids_t), 1, file);
	fread_checked(&player->equipment.off_hand_id, sizeof(item_ids_t), 1, file);
	fread_checked(&player->equipment.spell1_id, sizeof(item_ids_t), 1, file);
	fread_checked(&player->equipment.spell2_id, sizeof(item_ids_t), 1, file);
	fread_checked(&player->equipment.spell3_id, sizeof(item_ids_t), 1, file);

	fread_checked(&player->equipment.main_hand_two_handed, sizeof(bool), 1, file);
	
	fread_checked(&player->oil, sizeof(int), 1, file);
	fread_checked(&player->state, sizeof(player_state_t), 1, file);
	fread_checked(&player->inventory_manager, sizeof(inventory_manager_t), 1, file);
}

void load_world(world_t *world, player_t *player, FILE *file) {
	for(int x = 0; x < WORLD_WIDTH; x++) {
		for(int y = 0; y < WORLD_HEIGHT; y++) {
			int next_room_x = 0; 
			int next_room_y = 0;
			fread_checked(&next_room_x, sizeof(int), 1, file);
			fread_checked(&next_room_y, sizeof(int), 1, file);
			if(y == next_room_y && x == next_room_x) {
				load_room_save(world->room[x][y], file, world->item_data);
			}
		}
	}
	fread_checked(&world->seed, sizeof(int), 1, file);
	DEBUG_LOG("seed: %d", world->seed);
	
	fread_checked(&world->messages_size, sizeof(int), 1, file);
	fread_checked(&world->max_message_storage, sizeof(int), 1, file);
	
	world->messages = calloc(world->max_message_storage, sizeof(char *));
	for(int i = 0; i < world->max_message_storage; i++) {
		world->messages[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	}
	
	for(int i = 0; i < world->messages_size; i++) {
		int len;
		fread_checked(&len, sizeof(int), 1, file);
		fread_checked(world->messages[i], sizeof(char), len, file);
	}
	
	fread_checked(&world->turn_order_size, sizeof(int), 1, file);
	fread_checked(world->turn_order, sizeof(int), world->turn_order_size, file);
	fread_checked(&world->room_template_count, sizeof(int), 1, file);
	fread_checked(&world->room_templates, sizeof(room_template_t), world->room_template_count, file);
	fread_checked(&world->is_player_turn, sizeof(bool), 1, file);
	fread_checked(&world->buff_size, sizeof(uint8_t), 1, file);
	fread_checked(&world->buff_count, sizeof(uint8_t), 1, file);
	for(int i = 0; i < world->buff_count; i++) {
		load_buff(&world->buffs[i], world, player, file);
	}
}

void load_room_save(room_t *room, FILE *file, item_data_t *item_data) {
	fread_checked(room->room_file_name, sizeof(char), ROOM_FILE_NAME_MAX_SIZE, file);
	for(int tile_y = 0; tile_y < ROOM_HEIGHT; tile_y++) {
		for(int tile_x = 0; tile_x < ROOM_WIDTH; tile_x++) {
			room->tiles[tile_y][tile_x] = calloc(1, sizeof(tile_t));
			for(int i = 0; i < MAX_ITEMS_PER_TILE; i++) {
				room->tiles[tile_y][tile_x]->items[i] = calloc(1, sizeof(item_t));
			}
		}
	}
	for(int x = 0; x < ROOM_WIDTH; x++) {
		for(int y = 0; y < ROOM_HEIGHT; y++) {
			// fwrite_checked(&room->tiles[y][x]->floor, sizeof(char), 1, file);
			fread_checked(&room->tiles[y][x]->item_count, sizeof(int8_t), 1, file);
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				fread_checked(&room->tiles[y][x]->items[i]->id, sizeof(item_ids_t), 1, file);
				fread_checked(&room->tiles[y][x]->items[i]->stack, sizeof(int), 1, file);
				load_item_from_data(room->tiles[y][x]->items[i], item_data);
			}
		}
	}

	fread_checked(&room->current_pot_count, sizeof(int8_t), 1, file);
	for(int i = 0; i < room->current_pot_count; i++) {
		fread_checked(&room->pots[i].x, sizeof(int8_t), 1, file);
		fread_checked(&room->pots[i].y, sizeof(int8_t), 1, file);
		fread_checked(&room->pots[i].broken, sizeof(bool), 1, file);
	}

	fread_checked(&room->deleted_trap_count, sizeof(uint8_t), 1, file);
	for(int i = 0; i < room->deleted_trap_count; i++) {
		fread_checked(&room->deleted_trap_x[i], sizeof(uint8_t), 1, file);
		fread_checked(&room->deleted_trap_y[i], sizeof(uint8_t), 1, file);
	}

	fread_checked(&room->current_enemy_count, sizeof(int8_t), 1, file);
	for (int i = 0; i < room->current_enemy_count; i++) {
		fread_checked(room->enemies[i], sizeof(enemy_t), 1, file);
	}
	fread_checked(&room->is_created, sizeof(bool), 1, file);
	fread_checked(&room->global_time, sizeof(int), 1, file);
	fread_checked(&room->biome, sizeof(biome_t), 1, file);
	fread_checked(&room->is_main_path, sizeof(bool), 1, file);
	fread_checked(&room->door_mask, sizeof(uint8_t), 1, file);
	load_room_floor_tiles(room);
}
