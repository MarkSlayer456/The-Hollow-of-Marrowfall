#include "player.h"
#include "game_manager.h"
#include "map_manager.h"
#include "functions.h"
#include "enemy.h"
#include "hud.h"
#include "items/items.h"
#include "math.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include "types.h"
#include "entrances.h"
#include "pot.h"
#include "buff.h"
#include "menu.h"


extern char walk_chars[WALK_CHAR_LENGTH];

class_type_map_t class_type_map[] = {
	{NULL_CLASS_NAME, CLASS_NONE},
	{SWORDSMAN_CLASS_NAME, SWORDSMAN},
	{BARBARIAN_CLASS_NAME, BARBARIAN},
	{SPEARMAN_CLASS_NAME, SPEARMAN},
	{ARCHER_CLASS_NAME, ARCHER},
	{WIZARD_CLASS_NAME, WIZARD},
	{MERCHANT_CLASS_NAME, MERCHANT},
	{ROGUE_CLASS_NAME, ROGUE},
	{MONK_CLASS_NAME, MONK},
	{PALADIN_CLASS_NAME, PALADIN},
	{BRAWLER_CLASS_NAME, BRAWLER},
	{SAMURAI_CLASS_NAME, SAMURAI},
	{VOID_EMBRACE_CLASS_NAME, VOID_EMBRACE},
	{VOID_KNIGHT_CLASS_NAME, VOID_KNIGHT},
	{VOID_ASSASSIN_CLASS_NAME, VOID_ASSASSIN}
};

const int class_type_map_len = sizeof(class_type_map) / sizeof(class_type_map[0]);

class_type_t class_get_type(const char *name) {
	for(int i = 0; i < class_type_map_len; i++) {
		if(strcasecmp(name, class_type_map[i].name) == 0) {
			return class_type_map[i].value;
		}
	}
	return CLASS_NONE; // or an INVALID_TRAIT enum
}

const char *class_get_name(class_type_t name) {
	for(int i = 0; i < class_type_map_len; i++) {
		if(name == class_type_map[i].value) {
			return class_type_map[i].name;
		}
	}
	return "";
}

void load_class_data(class_data_t *class_data) {
	FILE *fp = fopen("./data/classes.csv", "r");
	if(!fp) {
		perror("File open failed");
		return;
	}
	
	char line[2048];
	
	if(fgets(line, sizeof(line), fp) == NULL) {
		fprintf(stderr, "File is empty\n");
		fclose(fp);
		return;
	}
	
	int row = 0;
	while(fgets(line, sizeof(line), fp)) {
		line[strcspn(line, "\n")] = '\0';
		int col = 0;
		char *token = strtok(line, ",");
		while(token) {
			switch(col) {
				case 0:
					class_data[row].type = class_get_type(token);
					break;
				case 1:
					class_data[row].base_strength = atoi(token);
					break;
				case 2:
					class_data[row].base_dexterity = atoi(token);
					break;
				case 3:
					class_data[row].base_intelligence = atoi(token);
					break;
				case 4:
					class_data[row].base_constitution = atoi(token);
					break;
				case 5:
					class_data[row].base_speed = atoi(token);
					break;
				case 6:
					class_data[row].growth_strength = atof(token);
					break;
				case 7:
					class_data[row].growth_dexterity = atof(token);
					break;
				case 8:
					class_data[row].growth_intelligence = atof(token);
					break;
				case 9:
					class_data[row].growth_constitution = atof(token);
					break;
				case 10:
					class_data[row].growth_speed = atof(token);
					break;
			}
			token = strtok(NULL, ",");
			col++;
		}
		DEBUG_LOG("Loaded Class Data: %d, %d, %d, %d, %d, %d, %f, %f, %f, %f, %f", class_data[row].type, 
			class_data[row].base_strength, class_data[row].base_dexterity, class_data[row].base_intelligence,
			class_data[row].base_constitution, class_data[row].base_speed, class_data[row].growth_strength, 
			class_data[row].growth_dexterity, class_data[row].growth_intelligence,
			class_data[row].growth_constitution, class_data[row].growth_speed);
		col = 0;
		row++;
	}
	fclose(fp);
}

char player_get_current_pos(player_t *player, world_t *world)
{
	return world->room[player->global_x][player->global_y]->tiles[player->y][player->x]->floor;
}

void player_move_dir(player_t *player, world_t *world, direction_t dir) {
	if(dir == LEFT && player_can_move_dir(player, world, LEFT)) player->x--;
	if(dir == RIGHT && player_can_move_dir(player, world, RIGHT)) player->x++;
	if(dir == DOWN && player_can_move_dir(player, world, DOWN)) player->y++;
	if(dir == UP && player_can_move_dir(player, world, UP)) player->y--;
	
	if(player_get_current_pos(player, world) == DOOR) {
		if(dir == LEFT) player->global_x--;
		if(dir == RIGHT) player->global_x++;
		if(dir == DOWN) player->global_y++;
		if(dir == UP) player->global_y--;
		if(player->global_y < 0) {
			player->y += 1;
			display_world_message(world, DOOR_BLOCKED_MESSAGE);
			return;
		}
		if(player->global_x < 0) {
			player->x += 1;
			display_world_message(world, DOOR_BLOCKED_MESSAGE);
			return;
		}
		
		if(!world->room[player->global_x][player->global_y]->is_created) {
			player_enter_new_room(player, world);
		}
		
		turn_order_enter_new_room(world, player);
		if(dir == LEFT) player->x = ROOM_WIDTH-2;
		if(dir == RIGHT) player->x = 1;
		if(dir == DOWN) player->y = 1;
		if(dir == UP) player->y = ROOM_HEIGHT-2;
	}
}

void player_enter_new_room(player_t *player, world_t *world) {
	float chance = (float)rand() / (float)RAND_MAX;
	if(chance <= ENTRANCE_MESSAGE_CHANCE) {
		int message = rand() % cave_entrance_messages_count;
		display_combat_message(world, cave_entrance_messages[message]);
	}
	world->room[player->global_x][player->global_y] = generate_room(&world->seed, player->global_x, player->global_y, world->enemy_data, world->item_data, world);
}

char player_check_dir(player_t *player, world_t *world, direction_t dir) {
	int x = player->x;
	int y = player->y;
	if(dir == LEFT) x--;
	if(dir == RIGHT) x++;
	if(dir == DOWN) y++;
	if(dir == UP) y--;
	
	room_t *room = world->room[player->global_x][player->global_y];
	char tmp = check_tile(room, player, y, x);
	return tmp;
}

int player_can_move_dir(player_t *player, world_t *world, direction_t dir) {
	char a = player_check_dir(player, world, dir);
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i]) { // the space is open
			return 1;
		}
	}
	return 0;
}

void player_wait(player_t *player, world_t *world)
{
	display_world_message(world, "You stand still!");
}

void player_damage(player_t *player, world_t *world, int attack) {
	player->health -= attack;
	if(player->health <= 0) {
		player->state = PLAYER_STATE_DEAD;

	}
}

void player_damage_ignore_armor(player_t *player, world_t *world, int attack) {
	player->health -= attack;
	if(player->health <= 0) {
		player->state = PLAYER_STATE_DEAD;
	}
}

void player_increase_health(player_t *player, int amount) {
	if(player->health + amount > player->max_health) {
		player->health = player->max_health;
	} else {
		player->health += amount;
	}
}

void player_increase_mana(player_t *player, int amount) {
	if(player->mana + amount > player->max_mana) {
		player->mana = player->max_mana;
	} else {
		player->mana += amount;
	}
}


enemy_t *player_get_dir_enemy(player_t *player, world_t *world, direction_t dir, uint8_t range) {
	int x = player->x;
	int y = player->y;
	for(int i = 0; i < range; i++) {
		if(dir == LEFT) x--;
		if(dir == RIGHT) x++;
		if(dir == DOWN) y++;
		if(dir == UP) y--;
		if(x < 0 || y < 0) return NULL;
		room_t *room = world->room[player->global_x][player->global_y];

		bool found = false;
		char test = room->tiles[y][x]->floor;
		for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
			if(test == walk_chars[i]) { // the space is open
				found = true;
				break;
			}
		}
		if(!found) return NULL;

		if(room->tiles[y][x]->floor )
		for(int i = 0; i < room->current_enemy_count; i++) {
			if(room->enemies[i] == NULL) continue;
			if(x == room->enemies[i]->x && y == room->enemies[i]->y) {
				return room->enemies[i];
			}
		}
	}
    return NULL;
}

pot_t *player_get_dir_pot(player_t *player, world_t *world, direction_t dir, uint8_t range) {
	int x = player->x;
	int y = player->y;
	for(int i = 0; i < range; i++) {
		if(dir == LEFT) x--;
		if(dir == RIGHT) x++;
		if(dir == DOWN) y++;
		if(dir == UP) y--;
		if(x < 0 || y < 0) return NULL;
		room_t *room = world->room[player->global_x][player->global_y];

		bool found = false;
		char test = room->tiles[y][x]->floor;
		for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
			if(test == walk_chars[i]) { // the space is open
				found = true;
				break;
			}
		}
		if(!found) return NULL;

		if(room->tiles[y][x]->floor )
			for(int i = 0; i < room->current_pot_count; i++) {
				if(room->pots[i].broken) continue;
				if(x == room->pots[i].x && y == room->pots[i].y) {
					return &room->pots[i];
				}
			}
	}
	return NULL;
}

double get_weapon_stat_scaling_factor(player_t *player, stats_t stat, double required_stat) {
	double scaling_factor = 0.0f;
	switch(stat) {
		case STRENGTH:
			scaling_factor = player->strength / required_stat;
			break;
		case DEXTERITY:
			scaling_factor = player->dexterity / required_stat;
			break;
		case INTELLIGENCE:
			scaling_factor = player->intelligence / required_stat;
			break;
		case CONSTITUTION:
			scaling_factor = player->constitution / required_stat;
			break;
		case SPEED:
			scaling_factor = player->speed / required_stat;
			break;
		case NULL_STAT:
			break;
	}
	return scaling_factor;
}

/**
 * attacks in a given direction doesn't matter if there is an enemy there or not
 */
void player_attack(player_t *player, world_t *world, direction_t dir) {
	player_exit_attack_state(player, world);

	equipment_t equip = player->equipment;

	pot_t *pot = player_get_dir_pot(player, world, dir, 1);
	if(pot) {
		pot_break(world, get_current_room(world, player), pot);
		return;
	}
	if(equip.attack_weapon < 0) {
		int unarmed_damage = 1;
		enemy_t *enemy = player_get_dir_enemy(player, world, dir, 1);
		if(!enemy) return;
		int xp = (enemy->health + enemy->strength) * 5; // TODO this needs changed
		char message[MAX_MESSAGE_LENGTH_WITHOUT_PREFIX];
		snprintf(message, MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, "You attacked for %d", unarmed_damage);
		display_combat_message(world, message);
		if(enemy_damage(enemy, world, unarmed_damage, 0)) {
			player_add_xp(player, xp, world->class_data);
		}
		return;
	}



	if(player->inventory[equip.attack_weapon].value_type == VALUE_TYPE_SPELL) {
		spell_stats_t *spell = &player->inventory[equip.attack_weapon].stat_type.spell;
		if(player->mana < spell->mana_cost) {
			display_combat_message(world, NOT_ENOUGH_MANA_MESSAGE);
			return;
		}
		player->mana -= spell->mana_cost;
		enemy_t *enemy = player_get_dir_enemy(player, world, dir, spell->range);
		if(!enemy) return;

		double required_stat = spell->min_damage + spell->max_damage;
		double scaling_factor = get_weapon_stat_scaling_factor(player, INTELLIGENCE, required_stat);
		double stat_weight = scaling_factor * get_percent_from_grade(spell->stat_grade);
		double rand_weight = (((double)rand() / RAND_MAX) * (1-(get_percent_from_grade(spell->stat_grade))));

		int raw_damage = ceil(spell->max_damage * (stat_weight + rand_weight));
		raw_damage = MAX(spell->min_damage, raw_damage);
		int xp = (enemy->health + enemy->strength) * 5; // TODO this needs changed
		int damage = raw_damage * (DEFENSE_SCALING_CONSTANT)/(DEFENSE_SCALING_CONSTANT+enemy->defense);
		damage = MAX(1, damage);
		char message[MAX_MESSAGE_LENGTH_WITHOUT_PREFIX];
		snprintf(message, MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, "You attacked for %d", damage);
		display_combat_message(world, message);
		if(enemy_damage(enemy, world, damage, 0)) {
			player_add_xp(player, xp, world->class_data);
		} else { // if enemy is not dead
			float random = (float) rand() / (float) RAND_MAX;
			if(spell->status_chance >= random) {
				int raw_status_damage = ceil(spell->buff_max_damage * (stat_weight + rand_weight));
				raw_status_damage = MAX(spell->min_damage, raw_status_damage);
				int status_damage = raw_status_damage * (DEFENSE_SCALING_CONSTANT)/(DEFENSE_SCALING_CONSTANT+enemy->defense);
				status_damage = MAX(1, status_damage);
				buff_t buff = buff_create();
				buff.target_type_id = TARGET_ENEMY;
				buff.target.enemy = enemy;
				buff.damage = status_damage;
				buff.turns_left = spell->buff_duration;

				buff_set_type(&buff, spell->buff_type);
				buff_add_to_list(buff, world->buffs, &world->buff_count, &world->buff_size);
			}
		}


		return;
	}

	if(player->inventory[equip.attack_weapon].value_type == VALUE_TYPE_WEAPON) {
		if(equip.main_hand < 0) return;
		item_t main_hand = player->inventory[equip.main_hand];
		enemy_t *enemy = player_get_dir_enemy(player, world, dir, main_hand.stat_type.weapon.range);
		if(!enemy) return;
		int xp = (enemy->health + enemy->strength) * 5; // TODO this needs changed

		int raw_damage = 0;
		weapon_stats_t *weapon = &main_hand.stat_type.weapon;
		double required_stat = weapon->min_attack + weapon->max_attack;

		double scaling_factor_stat1 = get_weapon_stat_scaling_factor(player, weapon->scaling_stat1, required_stat);
		if(scaling_factor_stat1 > 1) scaling_factor_stat1 = 1;

		if(weapon->scaling_stat2 != NULL_STAT) {
			double scaling_factor_stat2 = get_weapon_stat_scaling_factor(player, weapon->scaling_stat2, required_stat);
			if(scaling_factor_stat2 > 1) scaling_factor_stat2 = 1;
			double stat1_weight = scaling_factor_stat1 * get_percent_from_grade(weapon->stat1_grade);
			double stat2_weight = scaling_factor_stat2 * get_percent_from_grade(weapon->stat2_grade);
			double rand_weight = (((double)rand() / RAND_MAX) * (1-(get_percent_from_grade(weapon->stat1_grade)+get_percent_from_grade(weapon->stat2_grade))));
			raw_damage = ceil(weapon->max_attack * (stat1_weight + stat2_weight + rand_weight));
		} else {
			double stat_weight = scaling_factor_stat1 * get_percent_from_grade(weapon->stat1_grade);
			double rand_weight = (((double)rand() / RAND_MAX) * (1-get_percent_from_grade(weapon->stat1_grade)));
			raw_damage = ceil(weapon->max_attack * (stat_weight + rand_weight));
		}

		raw_damage = MAX(weapon->min_attack, raw_damage);
		if(raw_damage > weapon->max_attack) raw_damage = weapon->max_attack;
		DEBUG_LOG("raw damage: %d", raw_damage);
		if(player_did_crit(player_get_total_crit_chance(weapon))) {
			raw_damage *= 2;
			display_combat_message(world, "You land a crit");
		}
		int damage = raw_damage * (DEFENSE_SCALING_CONSTANT)/(DEFENSE_SCALING_CONSTANT+enemy->defense);
		damage = MAX(1, damage);
		DEBUG_LOG("actual damage: %d", damage);
		char message[MAX_MESSAGE_LENGTH_WITHOUT_PREFIX];
		snprintf(message, MAX_MESSAGE_LENGTH_WITHOUT_PREFIX, "You attacked for %d", damage);
		display_combat_message(world, message);
		if(enemy_damage(enemy, world, damage, 0)) {
			player_add_xp(player, xp, world->class_data);
		}
	}
}

void player_cycle_attack_weapon(player_t *player) {
	if(player->equipment.attack_weapon == -1) {
		player->equipment.attack_weapon = player->equipment.main_hand;
		if(player->equipment.attack_weapon != -1) {
			return;
		}
	}
	if(player->equipment.main_hand == player->equipment.attack_weapon) {
		player->equipment.attack_weapon = player->equipment.spell1;
		if(player->equipment.attack_weapon != -1) {
			return;
		}
	}
	if(player->equipment.spell1 == player->equipment.attack_weapon) {
		player->equipment.attack_weapon = player->equipment.spell2;
		if(player->equipment.attack_weapon != -1) {
			return;
		}
	}
	if(player->equipment.spell2 == player->equipment.attack_weapon) {
		player->equipment.attack_weapon = player->equipment.spell3;
		if(player->equipment.attack_weapon != -1) {
			return;
		}
	}
	if(player->equipment.spell3 == player->equipment.attack_weapon) {
		player->equipment.attack_weapon = -1;
		if(player->equipment.attack_weapon != -1) {
			return;
		}
	}
}

bool player_did_crit(double total_crit_chance) {
	double roll = (double)rand() / RAND_MAX;
	return roll < total_crit_chance;
}

double player_get_total_crit_chance(weapon_stats_t *weapon) {
	// TODO doesn't account for 2 weaopns
	return BASE_CRIT_CHANCE + weapon->crit_chance;
}

int xp_to_level_up(int level) {
	return 50 * level + (level * level * 10);
}

void player_check_level_up(player_t *player, const class_data_t *class_data) {
	int level = player->level;
	int level_xp = xp_to_level_up(level);
	if(player->xp >= level_xp) {
		player->xp -= xp_to_level_up(level);
		player->level++;
		for(int i = 0; i < MAX_CLASSES; i++) {
			if(class_data[i].type == player->player_class) {
				float growth_strength = class_data[i].growth_strength;
				float growth_dexterity = class_data[i].growth_dexterity;
				float growth_intelligence = class_data[i].growth_intelligence;
				float growth_constitution = class_data[i].growth_constitution;
				float growth_speed = class_data[i].growth_speed;
				player->strength += ((float)(growth_strength));
				player->dexterity += ((float)(growth_dexterity));
				player->intelligence += ((float)(growth_intelligence));
				player->constitution += ((float)(growth_constitution));
				player->speed += ((float)(growth_speed));
				int old_max_health = player->max_health;
				player->max_health = player_get_base_constitution(player, class_data) * 10;
				int gained_health = player->max_health - old_max_health;
				player_increase_health(player, gained_health);

				int old_max_mana = player->max_mana;
				player->max_mana = player_get_base_intelligence(player, class_data) * 10;
				int gained_mana = player->max_mana -old_max_mana;
				player_increase_mana(player, gained_mana);
				break;
			}
		}
	}
}

void player_enter_attack_state(player_t *player, world_t *world) {
	display_combat_message(world, ATTACK_DIRECTION_MESSAGE);
	player->state = PLAYER_STATE_ATTACKING;
}

void player_exit_attack_state(player_t *player, world_t *world) {
	player->state = PLAYER_STATE_MOVING;
}

void player_add_xp(player_t *player, int amount, const class_data_t *class_data) {
	player->xp += amount;
	player_check_level_up(player, class_data);
}

void player_cycle_inv_selector_up(player_t *player)
{
	if(player->inventory_manager.inv_selector > 0) {
		if(player->inventory_manager.inv_selector - player->inventory_manager.inv_offset == 0) {
			player->inventory_manager.inv_offset--;
		}
		player->inventory_manager.inv_selector--;
	}
}

void player_cycle_inv_selector_down(player_t *player)
{
	int visible_item_count = INVENTORY_HEIGHT-2;
	if(player->inventory[player->inventory_manager.inv_selector+1].id != BLANK) {
		if(player->inventory_manager.inv_selector - player->inventory_manager.inv_offset >= visible_item_count-1) {
			player->inventory_manager.inv_offset++;
		}
		player->inventory_manager.inv_selector++;
	}
}

void player_cycle_loot_selector_up(player_t *player) {
	if(player->inventory_manager.loot_selector > 0) {
		if(player->inventory_manager.loot_selector - player->inventory_manager.loot_offset == 0) {
			player->inventory_manager.loot_offset++;
		}
		player->inventory_manager.loot_selector--;
	}
}

void player_cycle_loot_selector_down(player_t *player) {
	int visible_item_count = INVENTORY_HEIGHT-2;
	if(player->nearby_loot[player->inventory_manager.loot_selector+1]->id != BLANK) {
		if(player->inventory_manager.loot_selector - player->inventory_manager.loot_offset >= visible_item_count-1) {
			player->inventory_manager.loot_offset++;
		}
		player->inventory_manager.loot_selector++;
	}
}

void player_open_loot(player_t *player, menu_t *menus) {
	if(player->nearby_loot_count == 0) return;
	// player->inventory_manager.loot_selector = 0;
	menus[LOOT_MENU].selected = 0;
	menus[LOOT_MENU].offset = 0;
	menus[LOOT_MENU].needs_redraw = true;
	menus[INVENTORY_MENU].needs_redraw = true;
	player->state = PLAYER_STATE_LOOTING;
}

void player_close_loot(player_t *player) {
	player->state  = PLAYER_STATE_MOVING;
}


void player_open_inventory(player_t *player, menu_t *menus) {
	menus[LOOT_MENU].needs_redraw = true;
	menus[INVENTORY_MENU].needs_redraw = true;
	player->state  = PLAYER_STATE_INVENTORY;

}

void player_close_inventory(player_t *player) {
	player->state  = PLAYER_STATE_MOVING;
}

void player_cycle_popup_menu_cursor_up(player_t *player, popup_menu_t *popup_menu) {
	if(popup_menu->cursor_pos == 0) {
		popup_menu->cursor_pos = popup_menu->max_cursor_pos;
	}
	popup_menu->cursor_pos--;
}

void player_cycle_popup_menu_cursor_down(player_t *player, popup_menu_t *popup_menu) {
	popup_menu->cursor_pos++;
	if(popup_menu->cursor_pos >= popup_menu->max_cursor_pos) {
		popup_menu->cursor_pos = 0;
	}
}

int player_inv_contains(player_t *player, item_t item) {
	for(int i = 0; i < player->inventory_count; i++) {
		if(player->inventory[i].id == item.id) {
			return i;
		}
	}
	return -1;
}

bool player_add_to_inv(player_t *player, item_t item) {
	if(item.id == OIL) {
		player->oil += item.stack;
		return true;
	}
	int index = player_inv_contains(player, item);
	if(index != -1) {
		player->inventory[index].stack += item.stack;
		item_mark_as_changed(&player->inventory[index]);
		return true;
	}
	if(player->inventory_count < INV_SIZE) {
		player->inventory[player->inventory_count++] = item;
		item_mark_as_changed(&player->inventory[player->inventory_count-1]);
		return true;
	}
	return false;
}

void player_drop_item(player_t *player, world_t *world, menu_t *menu) {
	int item_index = menu->selected+menu->offset;
	item_t item = player->inventory[item_index];
	drop_item(world->room[player->global_x][player->global_y]->tiles[player->y][player->x], world->item_data, item.id, item.stack);
	memset(player->inventory+item_index, 0, sizeof(item_t)); //TODO this is not safe
	player_organize_inv(player, menu, item_index);
	player_get_nearby_loot(world->room[player->global_x][player->global_y], player);
}

void player_take_loot_item(room_t *room, player_t *player, menu_t *menus) {
	int item_index = menus[LOOT_MENU].selected+menus[LOOT_MENU].offset;
	item_t *selected_item = player->nearby_loot[item_index];
	if(!selected_item) {
		menus[LOOT_MENU].selected = 0;
		menus[LOOT_MENU].offset = 0;
	}
	player_add_to_inv(player, *selected_item);
	int start_y = player->y - 1;
	int start_x = player->x - 1;
	int end_y = player->y + 1;
	int end_x = player->x + 1;
	
	if(start_y < 0) start_y = 0;
	if(start_x < 0) start_x = 0;
	if(end_y >= ROOM_HEIGHT) end_y = ROOM_HEIGHT - 1;
	if(end_x >= ROOM_WIDTH) end_x = ROOM_WIDTH - 1;
	
	bool found = false;
	for(int y = start_y; y <= end_y; y++) {
		for(int x = start_x; x <= end_x; x++) {
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				item_t *item = room->tiles[y][x]->items[i];
				if(item == selected_item) {
					remove_item_from_tile(room->tiles[y][x], item);
					if(item_index == player->nearby_loot_count-1) {
						menu_cursor_up(&menus[LOOT_MENU]);
					}
					break;
				}
			}
			if(found) break;
		}
		if(found) break;
	}
	// need to repopulate the array
	player_get_nearby_loot(room, player);
	menus[LOOT_MENU].needs_redraw = true;
	menus[INVENTORY_MENU].needs_redraw = true;
	if(player->nearby_loot_count == 0) player_open_inventory(player, menus);
}

void player_get_nearby_loot(room_t *room, player_t *player) {
	player_clear_nearby_loot(player);
	int start_y = player->y - 1;
	int start_x = player->x - 1;
	int end_y = player->y + 1;
	int end_x = player->x + 1;
	
	if(start_y < 0) start_y = 0;
	if(start_x < 0) start_x = 0;
	if(end_y >= ROOM_HEIGHT) end_y = ROOM_HEIGHT - 1;
	if(end_x >= ROOM_WIDTH) end_x = ROOM_WIDTH - 1;
	
	for(int y = start_y; y <= end_y; y++) {
		for(int x = start_x; x <= end_x; x++) {
			for(int i = 0; i < room->tiles[y][x]->item_count; i++) {
				item_t *item = room->tiles[y][x]->items[i];
				if(item == NULL || item->stack == 0) continue;
				player->nearby_loot[player->nearby_loot_count++] = item;
			}
		}
	}
}

void player_clear_nearby_loot(player_t *player) {
	for(int i = 0; i < player->nearby_loot_count; i++) {
		player->nearby_loot[i] = NULL;
	}
	player->nearby_loot_count = 0;
}

// removes an item from the inventory list and reorganizes, not used to decrease item count
void player_organize_inv(player_t *player, menu_t *menu, int loc)
{
	int item_index = menu->selected+menu->offset;
	player_decrement_equipment_indexes(player, loc);

	for(int i = loc; i < INV_SIZE-1; i++) {
		player->inventory[i] = player->inventory[i + 1];
	}
	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0, false};
	player->inventory[INV_SIZE-1] = blank;
	player->inventory_count--;
	while(player->inventory[item_index].id == BLANK && item_index > 0) {
		menu_cursor_up(menu);
		item_index = menu->selected+menu->offset;
	}
}

void player_decrement_equipment_indexes(player_t *player, int loc) {
	equipment_t *equip = &player->equipment;

	if(loc == equip->attack_weapon) equip->attack_weapon = -1;
	if(loc == equip->armor) equip->armor = -1;
	if(loc == equip->main_hand) equip->main_hand = -1;
	if(loc == equip->off_hand) equip->off_hand = -1;
	if(loc == equip->spell1) equip->spell1 = -1;
	if(loc == equip->spell2) equip->spell2 = -1;
	if(loc == equip->spell3) equip->spell3 = -1;

	if(loc < equip->attack_weapon) equip->attack_weapon--;
	if(loc < equip->armor) equip->armor--;
	if(loc < equip->main_hand) equip->main_hand--;
	if(loc < equip->off_hand) equip->off_hand--;
	if(loc < equip->spell1) equip->spell1--;
	if(loc < equip->spell2) equip->spell2--;
	if(loc < equip->spell3) equip->spell3--;
}

void player_change_class(player_t *player, world_t *world, enum class_type player_class) {
	player->player_class = player_class;
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(world->class_data[i].type == player->player_class) {
			int base_strength = world->class_data[i].base_strength;
			int base_dexterity = world->class_data[i].base_dexterity;
			int base_intelligence = world->class_data[i].base_intelligence;
			int base_constitution = world->class_data[i].base_constitution;
			int base_speed = world->class_data[i].base_speed;

			player->strength = base_strength;
			player->dexterity = base_dexterity;
			player->intelligence = base_intelligence;
			player->constitution = base_constitution;
			player->speed = base_speed;
			break;
		}
	}
	player->health = player->constitution * 10;
	player->max_health = player->constitution * 10;
	player->mana = player->intelligence * 10;
	player->max_mana = player->intelligence * 10;
}

float player_get_base_strength(player_t *player, const class_data_t *class_data) {
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(class_data[i].type == player->player_class) {
			return class_data[i].base_strength + (class_data[i].growth_strength * (player->level-1));
		}
	}
	return 0;
}

float player_get_base_dexterity(player_t *player, const class_data_t *class_data) {
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(class_data[i].type == player->player_class) {
			return class_data[i].base_dexterity + (class_data[i].growth_dexterity * (player->level-1));
		}
	}
	return 0;
}
float player_get_base_intelligence(player_t *player, const class_data_t *class_data) {
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(class_data[i].type == player->player_class) {
			return class_data[i].base_intelligence + (class_data[i].growth_intelligence * (player->level-1));
		}
	}
	return 0;
}
float player_get_base_constitution(player_t *player, const class_data_t *class_data) {
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(class_data[i].type == player->player_class) {
			return class_data[i].base_constitution + (class_data[i].growth_constitution * (player->level-1));
		}
	}
	return 0;
}
float player_get_base_speed(player_t *player, const class_data_t *class_data) {
	for(int i = 0; i < MAX_CLASSES; i++) {
		if(class_data[i].type == player->player_class) {
			return class_data[i].base_speed + (class_data[i].growth_speed * (player->level-1));
		}
	}
	return 0;
}

void player_setup(player_t *player, world_t *world) {
	inventory_manager_t inv_manager = {
		.spells_selector = 0,
		.inv_selector = 0,
		.inv_offset = 0,
		.loot_selector = 0,
		.loot_offset = 0,
		.cat = ITEM
	};
	player->inventory_manager = inv_manager;
	player->x = 1;
	player->y = 10;
	player_change_class(player, world, SAMURAI);

	player->spell_equip_menu.win = newwin(SPELL_EQUIP_MENU_HEIGHT, SPELL_EQUIP_MENU_WIDTH, SCREEN_HEIGHT/2-(SPELL_EQUIP_MENU_HEIGHT/2), SCREEN_WIDTH/2-(SPELL_EQUIP_MENU_WIDTH/2));
	player->spell_equip_menu.cursor_pos = 0;
	player->spell_equip_menu.cursor_offset = 0;
	player->spell_equip_menu.max_cursor_pos = MAX_SPELL_SLOTS;
	player->spell_equip_menu.data_capacity = 0;
	player->spell_equip_menu.data_count = 0;
	player->spell_equip_menu.data = NULL;

	player->death_menu.win = newwin(DEATH_MENU_HEIGHT, DEATH_MENU_WIDTH, SCREEN_HEIGHT/2-(DEATH_MENU_HEIGHT/2), SCREEN_WIDTH/2-(DEATH_MENU_WIDTH/2));
	player->death_menu.cursor_pos = 0;
	player->death_menu.cursor_offset = 0;
	player->death_menu.max_cursor_pos = 1;
	player->death_menu.data_capacity = 0;
	player->death_menu.data_count = 0;
	player->death_menu.data = NULL;

	player->health = player->constitution * 10;
	player->max_health = player->constitution * 10;
	player->mana = player->intelligence * 10;
	player->max_mana = player->intelligence * 10;
	player->global_x = 0;
	player->global_y = 0;
	player->action_points = 0;
	player->level = 1;
	player->xp = 0;
	player->oil = STARTING_OIL;
	player->equipment = (equipment_t) {0};
	player->equipment.attack_weapon = -1;
	player->equipment.main_hand = -1;
	player->equipment.off_hand = -1;
	player->equipment.armor = -1;
	player->equipment.spell1 = -1;
	player->equipment.spell2 = -1;
	player->equipment.spell3 = -1;

	player->state = PLAYER_STATE_MAIN_MENU;

	player->inventory = malloc(INV_SIZE * sizeof(item_t));
	player->inventory_count = 0;
	for(int i = 0; i < MAX_ITEMS_NEARBY_PLAYER; i++) {
		player->nearby_loot[i] = malloc(1 * sizeof(item_t));
	}
	player->nearby_loot_count = 0;

	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0, false};

	for(int i = 0; i < INV_SIZE; i++) {
		player->inventory[i] = blank;
	}

	player->lantern.power = 5;
	player->lantern.is_on = true;
	player->lantern.turns_since_last_dim = 0;
}

void player_reset_values(player_t *player, world_t *world) {
	inventory_manager_t inv_manager = {
		.spells_selector = 0,
		.inv_selector = 0,
		.inv_offset = 0,
		.loot_selector = 0,
		.loot_offset = 0,
		.cat = ITEM
	};

	player->inventory_manager = inv_manager;
	player->x = 1;
	player->y = 10;
	player_change_class(player, world, SAMURAI);

	player->spell_equip_menu.cursor_pos = 0;
	player->spell_equip_menu.cursor_offset = 0;
	player->spell_equip_menu.max_cursor_pos = MAX_SPELL_SLOTS;
	player->spell_equip_menu.data_capacity = 0;
	player->spell_equip_menu.data_count = 0;
	player->spell_equip_menu.data = NULL;

	player->death_menu.cursor_pos = 0;
	player->death_menu.cursor_offset = 0;
	player->death_menu.max_cursor_pos = 1;
	player->death_menu.data_capacity = 0;
	player->death_menu.data_count = 0;
	player->death_menu.data = NULL;

	player->health = player->constitution * 10;
	player->max_health = player->constitution * 10;
	player->mana = player->intelligence * 10;
	player->max_mana = player->intelligence * 10;
	player->global_x = 0;
	player->global_y = 0;
	player->action_points = 0;
	player->level = 1;
	player->xp = 0;
	player->oil = STARTING_OIL;
	player->equipment = (equipment_t) {0};
	player->equipment.attack_weapon = -1;
	player->equipment.main_hand = -1;
	player->equipment.off_hand = -1;
	player->equipment.armor = -1;
	player->equipment.spell1 = -1;
	player->equipment.spell2 = -1;
	player->equipment.spell3 = -1;
	player->state = PLAYER_STATE_MAIN_MENU;

	player->inventory_count = 0;
	player->nearby_loot_count = 0;

	item_t blank = {BLANK_NAME, "does nothing", BLANK, 0, false};

	for(int i = 0; i < INV_SIZE; i++) {
		player->inventory[i] = blank;
	}
	memset(player->inventory, 0, sizeof(item_t)*player->inventory_count);
	// for(int i = 0; i < MAX_ITEMS_NEARBY_PLAYER; i++) {
	// 	memset(player->nearby_loot[i], 0, sizeof(item_t));
	// }

	player->lantern.power = 5;
	player->lantern.is_on = true;
	player->lantern.turns_since_last_dim = 0;

	player->state = PLAYER_STATE_MAIN_MENU;
	// menu manager doesn't need to be cleared, it's always in use

}
