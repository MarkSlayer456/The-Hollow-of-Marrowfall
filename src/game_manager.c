#include "game_manager.h"
#include "hud.h"
#include "player.h"
#include "items/items.h"
#include "map_manager.h"
#include "enemy.h"
#include <math.h>
#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "types.h"
#include "lantern.h"
#include "save.h"
#include "player.h"
#include "functions.h"
#include "menu.h"

//wmove(win, x, y);
//waddch(win, char);
extern WINDOW *hud, *action_bar, *inventory_hud, *inventory_desc_hud;
extern char walk_chars[WALK_CHAR_LENGTH];

const int MENU_CLASS_LIST[] = {
	SWORDSMAN,
	BARBARIAN,
	SPEARMAN,
	ARCHER,
	WIZARD,
	MERCHANT,
	ROGUE,
	MONK,
	PALADIN,
	BRAWLER,
	SAMURAI,
	VOID_EMBRACE,
	VOID_KNIGHT,
	VOID_ASSASSIN
};

const int MENU_CLASS_LIST_SIZE = sizeof(MENU_CLASS_LIST) / sizeof(MENU_CLASS_LIST[0]);

// void draw(world_t *world, player_t *player) {
// 	werase(world->win);
// 	werase(hud);
// 	werase(action_bar);
// 	werase(inventory_hud);
// 	werase(inventory_desc_hud);
// 	// draw stuff...
// 	if(player->state == PLAYER_STATE_EQUIPPING_SPELL) {
// 		return;
// 	}
// 	if(player->state == PLAYER_STATE_DEAD) {
// 		display_death_menu(player, player->death_menu);
// 		return;
// 	}
// 	if(player->state == PLAYER_STATE_INVENTORY || player->state == PLAYER_STATE_LOOTING) {
// 		display_inventory_hud(world, player);
// 		display_inventory_desc_hud(world, player);
// 		wnoutrefresh(inventory_hud);
// 		wnoutrefresh(inventory_desc_hud);
// 	} else {
// 		// TODO add just a hud_update() function so you don't have to call a bunch of different functions
// 		room_t *room = world->room[player->global_x][player->global_y];
// 		hud_update_player_health(player, world->buffs, world->buff_count);
// 		hud_update_nearby_enemies(world, player);
// 		// hud_update_menus(player, room);
// 		hud_update_messages(world, player);
// 		for(int i = 0; i < ROOM_HEIGHT; i++) {
// 			for(int j = 0; j < ROOM_WIDTH; j++) {
// 				if(room->tiles[i][j]->has_light == false) {
// 					continue;
// 				}
// 				int enemyIsThere = 0;
// 				int playerIsThere = 0;
// 				wmove(world->win, i, j);
// 				for(int u = 0; u < room->current_enemy_count; u++) {
// 					if(room->enemies[u] == NULL) continue;
// 					if(room->enemies[u]->x == j && room->enemies[u]->y == i) {
// 						waddch(world->win, room->enemies[u]->symbol);
// 						enemyIsThere = 1;
// 						break;
// 					}
// 				}
// 				for(int u = 0; u < room->current_pot_count; u++) {
// 					if(room->pots[u].broken) continue;
// 					if(room->pots[u].x == j && room->pots[u].y == i) {
// 						waddch(world->win, POT);
// 						break;
// 					}
// 				}
//
// 				if(player->x == j && player->y == i) {
// 					waddch(world->win, PLAYER);
// 					playerIsThere = 1;
// 				}
//
// 				for(int k = 0; k < MAX_ITEMS_PER_TILE; k++) {
// 					if(room->tiles[i][j]->items[k] != NULL && room->tiles[i][j]->items[k]->stack > 0) {
// 						waddch(world->win, ITEM_SYMBOL);
// 						enemyIsThere = 1; //TODO
// 						break;
// 					}
// 				}
//
// 				if(!playerIsThere && !enemyIsThere) {
// 					waddch(world->win, room->tiles[i][j]->floor);
// 				}
// 			}
// 		}
// 		wnoutrefresh(inventory_hud); // this has to be first
// 		wnoutrefresh(inventory_desc_hud); // this has to be first
// 		wnoutrefresh(world->win);
// 		wnoutrefresh(hud);
// 		wnoutrefresh(action_bar);
// 	}
// 	doupdate();
// }

bool sdl_manage_input(SDL_Event event, world_t *world, player_t *player, menu_t *menus) {
	room_t *room = world->room[player->global_x][player->global_y];
	bool end_turn = true;
	switch(event.type) {
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym) {
				case SDLK_w:
					switch(player->state) {
						case PLAYER_STATE_MOVING:
							player_move_dir(player, world, UP);
							break;
						case PLAYER_STATE_ATTACKING:
							player_attack(player, world, UP);
							break;
						case PLAYER_STATE_MAIN_MENU:
							menu_cursor_up(&menus[MAIN_MENU]);
							break;
						case PLAYER_STATE_CLASS_MENU:
							menu_cursor_up(&menus[CLASS_MENU]);
							break;
						case PLAYER_STATE_LOAD_MENU:
							menu_cursor_up(&menus[LOAD_MENU]);
							break;
						case PLAYER_STATE_INVENTORY:
							menu_cursor_up(&menus[INVENTORY_MENU]);
							menus[INVENTORY_MENU].needs_redraw = true;
							end_turn = false;
							break;
						case PLAYER_STATE_LOOTING:
							menu_cursor_up(&menus[LOOT_MENU]);
							menus[LOOT_MENU].needs_redraw = true;
							end_turn = false;
							break;
						case PLAYER_STATE_MENU:
							break;
						case PLAYER_STATE_EQUIPPING_SPELL:
							// player_cycle_popup_menu_cursor_up(player, &player->spell_equip_menu);
							menu_cursor_up(&menus[SPELL_MENU]);
							break;
						default:
							break;
					}
					break;
				case SDLK_s:
					switch(player->state) {
						case PLAYER_STATE_MOVING:
							player_move_dir(player, world, DOWN);
							break;
						case PLAYER_STATE_ATTACKING:
							player_attack(player, world, DOWN);
							break;
						case PLAYER_STATE_MAIN_MENU:
							menu_cursor_down(&menus[MAIN_MENU]);
							break;
						case PLAYER_STATE_CLASS_MENU:
							menu_cursor_down(&menus[CLASS_MENU]);
							break;
						case PLAYER_STATE_LOAD_MENU:
							menu_cursor_down(&menus[LOAD_MENU]);
							break;
						case PLAYER_STATE_INVENTORY:
							menu_cursor_down(&menus[INVENTORY_MENU]);
							menus[INVENTORY_MENU].needs_redraw = true;
							end_turn = false;
							break;
						case PLAYER_STATE_LOOTING:
							menu_cursor_down(&menus[LOOT_MENU]);
							menus[LOOT_MENU].needs_redraw = true;
							end_turn = false;
							break;
						case PLAYER_STATE_MENU:
							break;
						case PLAYER_STATE_EQUIPPING_SPELL:
							menu_cursor_down(&menus[SPELL_MENU]);
							// player_cycle_popup_menu_cursor_down(player, &player->spell_equip_menu);
							break;
						default:
							break;
					}
					break;
					case SDLK_a:
						switch(player->state) {
							case PLAYER_STATE_MOVING:
								player_move_dir(player, world, LEFT);
								break;
							case PLAYER_STATE_ATTACKING:
								player_attack(player, world, LEFT);
								break;
							case PLAYER_STATE_VIEWING:
								break;
							case PLAYER_STATE_INVENTORY:
								break;
							case PLAYER_STATE_LOOTING:
								player_open_inventory(player, menus);
								end_turn = false;
								break;
							case PLAYER_STATE_MENU:
								break;
							default:
								break;
						}
						break;
					case SDLK_d:
						switch(player->state) {
							case PLAYER_STATE_MOVING:
								player_move_dir(player, world, RIGHT);
								break;
							case PLAYER_STATE_ATTACKING:
								player_attack(player, world, RIGHT);
								break;
							case PLAYER_STATE_INVENTORY:
								player_open_loot(player, menus);
								end_turn = false;
								break;
							case PLAYER_STATE_MENU:
								break;
							default:
								break;
						}
						break;
					case SDLK_e:
						switch(player->state) {
							case PLAYER_STATE_MOVING:
								player_enter_attack_state(player, world);
								end_turn = false;
								break;
							case PLAYER_STATE_ATTACKING:
								break;
							default:
								break;
						}
						break;
					case SDLK_i:
						switch(player->state) {
							case PLAYER_STATE_MOVING:
								player_open_inventory(player, menus);
								end_turn = false;
								break;
							case PLAYER_STATE_INVENTORY:
								player_close_inventory(player);
								break;
							case PLAYER_STATE_LOOTING:
								player_close_inventory(player);
								break;
							case PLAYER_STATE_EQUIPPING_SPELL:
								player_open_inventory(player, menus);
								break;
							default:
								break;
						}
						break;
					case SDLK_q:
						switch(player->state) {
							case PLAYER_STATE_MOVING:
								player_cycle_attack_weapon(player);
								end_turn = false;
								break;
							case PLAYER_STATE_VIEWING:
								break;
							default:
								break;
						}
						break;
					case SDLK_RETURN:
						switch(player->state) {
							case PLAYER_STATE_INVENTORY:
								use_item(&menus[INVENTORY_MENU], player);
								menus[INVENTORY_MENU].needs_redraw = true;
								break;
							case PLAYER_STATE_LOOTING:
								player_take_loot_item(room, player, menus);
								menus[LOOT_MENU].needs_redraw = true;
								menus[INVENTORY_MENU].needs_redraw = true;
								break;
							case PLAYER_STATE_EQUIPPING_SPELL:
								//TODO
								// equip_spell(player, player->spell_equip_menu.cursor_pos+1);
								// player->state = PLAYER_STATE_INVENTORY;
								equip_spell(player, menus[SPELL_MENU].selected+1, menus[INVENTORY_MENU].selected);
								player->state = PLAYER_STATE_INVENTORY; //TODO this shouldn't be here
								menus[SPELL_MENU].needs_redraw = true;
								break;
							case PLAYER_STATE_DEAD:
								end_game(world, player);
								break;
							case PLAYER_STATE_MAIN_MENU:
								menus[MAIN_MENU].operation(menus[MAIN_MENU].operation_ctx1, menus[MAIN_MENU].operation_ctx2, menus[MAIN_MENU].operation_ctx3);
								break;
							case PLAYER_STATE_LOAD_MENU:
								menus[LOAD_MENU].operation(menus[LOAD_MENU].operation_ctx1, menus[LOAD_MENU].operation_ctx2, menus[LOAD_MENU].operation_ctx3);
								break;
							case PLAYER_STATE_CLASS_MENU: {
								menu_t class_menu = menus[CLASS_MENU];
								int data_index = class_menu.selected;
								player_change_class(
									player,
									world,
									class_get_type(class_menu.data[data_index].data_type.const_data.str));
								player->state = PLAYER_STATE_MOVING;
								break;
							}
							default:
								break;
						}
						break;
					case SDLK_b:
						switch(player->state) {
							case PLAYER_STATE_ATTACKING:
								break;
							case PLAYER_STATE_INVENTORY:
								player_drop_item(player, world, &menus[INVENTORY_MENU]);
								menus[LOOT_MENU].needs_redraw = true;
								break;
							case PLAYER_STATE_LOOTING:
								player_close_inventory(player);
								break;
							case PLAYER_STATE_MENU:
								break;
							case PLAYER_STATE_EQUIPPING_SPELL:
								player_open_inventory(player, menus);
								break;
							default:
								break;
						}
						break;
					case SDLK_SPACE:
						switch(player->state) {
							case PLAYER_STATE_MOVING:
								player_wait(player, world);
								break;
							case PLAYER_STATE_INVENTORY:
								break;
							case PLAYER_STATE_LOOTING:
								player_close_inventory(player);
								break;
							default:
								break;
						}
						break;
					case SDLK_f:
						switch(player->state) {
							case PLAYER_STATE_MOVING:
								if(lantern_increase_power(&player->lantern, &player->oil) == false) {
									display_world_message(world, LANTERN_CAN_AFFORD_REFUEL);
								}
								break;
							case PLAYER_STATE_LOOTING:
								break;
							default:
								break;
						}
						break;
				}
			return end_turn;
		case SDL_QUIT:
			// end_game(world, player);
			// running = 0;
			break;
		default:
			end_turn = false;
			break;
		}
	return end_turn;
}

void display_combat_message(world_t *world, const char *str) {
	if(MAX_MESSAGE_LENGTH_WITHOUT_PREFIX < strlen(str)) return;
	int current_size = world->messages_size;
	if(current_size >= world->max_message_storage) {
		world->max_message_storage*=2;
		char **tmp = realloc(world->messages, world->max_message_storage * sizeof(char *));
		if(tmp) {
			for(int i = world->max_message_storage/2; i < world->max_message_storage; i++) {
					tmp[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
			}
			world->messages = tmp;
		} else {
			world->max_message_storage/=2;
		}
	}
	// strncpy(world->messages[current_size], str, MAX_MESSAGE_LENGTH-1);
	snprintf(world->messages[current_size], MAX_MESSAGE_LENGTH, "[Combat] %s", str);
	world->messages_size++;
	//TODO max_message_storage
}

void display_world_message(world_t *world, const char *str) {
	if(MAX_MESSAGE_LENGTH_WITHOUT_PREFIX < strlen(str)) return;
	int current_size = world->messages_size;
	if(current_size >= world->max_message_storage) {
		world->max_message_storage*=2;
		char **tmp = realloc(world->messages, world->max_message_storage * sizeof(char *));
		if(tmp) {
			for(int i = world->max_message_storage/2; i < world->max_message_storage; i++) {
				tmp[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
			}
			world->messages = tmp;
		} else {
			world->max_message_storage/=2;
		}
	}
	// strncpy(world->messages[current_size], str, MAX_MESSAGE_LENGTH-1);
	snprintf(world->messages[current_size], MAX_MESSAGE_LENGTH, "[World] %s", str);
	world->messages_size++;
	//TODO max_message_storage
}

bool is_opaque(room_t *room, int x, int y) {
	char a = room->tiles[y][x]->floor;
	for(int i = 0; i < WALK_CHAR_LENGTH; i++) {
		if(a == walk_chars[i] || a == PLAYER) { // the space is open
			return false;
		}
	}
	return true;
}

void mark_has_light(room_t *room, int x, int y) {
	room->tiles[y][x]->has_light = true;
}

void cast_light_check(world_t *world, player_t *player, int x0, int y0, float angle) {
	float dx = cos(angle);
	float dy = sin(angle);
	float x = x0 + 0.3f;
	float y = y0 + 0.3f;
	int max_distance = player->lantern.power > PLAYER_MIN_VISION_RANGE ? player->lantern.power : PLAYER_MIN_VISION_RANGE;
	
	for(int i = 0; i < max_distance; i++) {
		int tile_x = (int)x;
		int tile_y = (int)y;
		
		if(tile_x > ROOM_WIDTH-1 || tile_y > ROOM_HEIGHT-1 || tile_y < 0 || tile_x < 0) {
			break;
		}
		room_t *room = world->room[player->global_x][player->global_y];
		
		mark_has_light(room, tile_x, tile_y);
		
		if(is_opaque(room, tile_x, tile_y)) {
			break;
		}
		
		x += dx;
		y += dy;
	}
}

void calculate_light(world_t *world, player_t *player) {
	for(int i = 0; i < ROOM_HEIGHT; i++) {
		for(int j = 0; j < ROOM_WIDTH; j++) {
			world->room[player->global_x][player->global_y]->tiles[i][j]->has_light = false;
		}
	}
	for(float a = 0; a < 2 * M_PI; a += 0.03f) {
		cast_light_check(world, player, player->x, player->y, a);
	}
}

void remove_points_from_actor() {	
	
}

int pick_next_actor(world_t *world, player_t *player) {
	int idx = INVALID_ACTOR_INDEX;
	int fastest = 0;
	while(true) {
		world->action_points += WORLD_SPEED;
		if(world->action_points >= TIME_TO_ACT) {
			if(fastest < world->action_points) {
				fastest = world->action_points;
				idx = WORLD_TURN_ORDER_INDEX;
			}
		}

		player->action_points += MAX(player->speed, 1);
		if(player->action_points >= TIME_TO_ACT) {
			if(fastest < player->action_points) {
				fastest = player->action_points;
				idx = PLAYER_TURN_ORDER_INDEX;
			}
		}
		
		for(int i = 0; i < world->room[player->global_x][player->global_y]->current_enemy_count; i++) {
			enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[i];
			if(enemy == NULL) continue;
			enemy->action_points += MAX(enemy->speed, 1);
			if(enemy->action_points >= TIME_TO_ACT) {
				if(fastest < enemy->action_points) {
					fastest = enemy->action_points;
					idx = i;
				}
			}
		}
		
		if(idx == INVALID_ACTOR_INDEX) continue;
		
		if(idx == PLAYER_TURN_ORDER_INDEX) {
			player->action_points -= TIME_TO_ACT;
		} else if(idx == WORLD_TURN_ORDER_INDEX) {
			world->action_points -= TIME_TO_ACT;
		} else {
			enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[idx];
			enemy->action_points -= TIME_TO_ACT;
		}
		break;
	}
	return idx;
}

void generate_turn_order_display(world_t *world, player_t *player) {
	int projected_enemy_ap[MAX_ENEMIES_PER_LEVEL];
	int player_projected_ap = player->action_points;
	int world_projected_ap = world->action_points;
	world->turn_order_size = 0;
	room_t *room = world->room[player->global_x][player->global_y];
	for(int i = 0; i < room->current_enemy_count; i++) {
		enemy_t *enemy = room->enemies[i];
		if(enemy == NULL) continue;
		projected_enemy_ap[i] = enemy->action_points;
	}
	while(world->turn_order_size < MAX_ENEMIES_PER_LEVEL) {
		int largest = 0;
		int idx = INVALID_ACTOR_INDEX;
		world_projected_ap += WORLD_SPEED;
		if(world_projected_ap >= TIME_TO_ACT && world_projected_ap > largest) {
			if(largest < world_projected_ap) {
				largest = world_projected_ap;
				idx = WORLD_TURN_ORDER_INDEX;
			}
		}

		player_projected_ap += MAX(player->speed, 1);
		if(player_projected_ap >= TIME_TO_ACT && player_projected_ap > largest) {
			if(largest < player_projected_ap) {
				largest = player_projected_ap;
				idx = PLAYER_TURN_ORDER_INDEX;
			}
		}

		room_t *room = world->room[player->global_x][player->global_y];
		for(int i = 0; i < room->current_enemy_count; i++) {
			enemy_t *enemy = room->enemies[i];
			if(enemy == NULL) continue;
			projected_enemy_ap[i] += MAX(enemy->speed, 1);
			if(projected_enemy_ap[i] >= TIME_TO_ACT && projected_enemy_ap[i] > largest) {
				if(largest < projected_enemy_ap[i]) {
					largest = projected_enemy_ap[i];
					idx = i;
				}
			}
		}
		
		if(idx == INVALID_ACTOR_INDEX) {
			continue;
		} else if(idx == PLAYER_TURN_ORDER_INDEX) {
			player_projected_ap -= TIME_TO_ACT;
		} else if(idx == WORLD_TURN_ORDER_INDEX) {
			world_projected_ap -= TIME_TO_ACT;
		} else {
			projected_enemy_ap[idx] -= TIME_TO_ACT;
		}
		world->turn_order[world->turn_order_size++] = idx;
	}
}

void turn_order_enter_new_room(world_t *world, player_t *player) {
	player->action_points = 0;
	world->action_points = 0;
	assert(world->room[player->global_x][player->global_y]->is_created);
	for(int i = 0; i < world->room[player->global_x][player->global_y]->current_enemy_count; i++) {
		if(world->room[player->global_x][player->global_y]->enemies[i] == NULL) continue;
		world->room[player->global_x][player->global_y]->enemies[i]->action_points = 0;
	}
}

/* TODO keeping this for reference when creating sdl version
void display_and_manage_save_menu(WINDOW *win, char *buf, int max_len, world_t *world, player_t *player, menu_manager_t *menu_manager) {
	werase(win);
	memset(buf, 0, max_len);
	int ch;
	int len = 0;
	int y = 1;
	int x = 0;
	waddstr(win, SAVE_MENU_PROMPT);
	wmove(win, y, x);
	touchwin(win);
	wrefresh(win);
	while((ch = wgetch(win)) != ESC_KEY) {
		if(isprint(ch) && len < max_len-1) {
			buf[len++] = ch;
			mvwaddch(win, y, x + len - 1, ch);
		} else if(ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
			len--;
			buf[len] = '\0';
			mvwaddch(win, y, x + len, ' ');
			wmove(win, y, x + len);
		} else if(ch == ENTER_KEY) {
			save_game(world, player, buf);
			menu_manager->current_menu = GAME;
			break;
		}
		wnoutrefresh(win);
		doupdate();
	}
}*/

// void display_death_menu(player_t *player, popup_menu_t menu) {
// 	WINDOW *win = menu.win;
// 	werase(win);
// 	box(win, 0, 0);
// 	touchwin(win);
// 	wrefresh(win);
//
// 	char str[64];
// 	int y = 1;
// 	char title[9] = "You Died";
// 	wmove(win, y++, (DEATH_MENU_WIDTH/2) - strlen(title)/2);
// 	waddstr(win, title);
//
// 	snprintf(str, sizeof(str), "Class: %s | Level: %d", class_get_name(player->player_class), player->level);
// 	wmove(win, y, DEATH_MENU_WIDTH/2 - strlen(str)/2);
// 	waddstr(win, str);
// 	y+=2;
// 	snprintf(str, sizeof(str), ">>Return to Main Menu");
// 	wmove(win, y, DEATH_MENU_WIDTH/2 - strlen(str)/2);
// 	waddstr(win, str);
//
// 	wnoutrefresh(win);
// 	doupdate();
// }


direction_t direction_from_key(int key) {
	switch(key) {
		case LEFT_ARROW:
			return LEFT;
		case RIGHT_ARROW:
			return RIGHT;
		case UP_ARROW:
			return UP;
		case DOWN_ARROW:
			return DOWN;
	}
	return -1;
}

void handle_world_turn(world_t *world, player_t *player) {

}

void end_game(world_t *world, player_t *player) {
    return_to_main_menu(world, player);
	// shutdown(world, player); //TODO this obvisouly isn't what should happpen
}

void return_to_main_menu(world_t *world, player_t *player) {
	for(int y = 0; y < WORLD_HEIGHT; y++) {
		for(int x = 0; x < WORLD_WIDTH; x++) {
			room_t *room = world->room[x][y];
			if(!room) continue;
			for(int ty = 0; ty < ROOM_HEIGHT; ty++) {
				for(int tx = 0; tx < ROOM_WIDTH; tx++) {
					tile_t *tile = room->tiles[ty][tx];
					if(!tile) continue;
					for(int i = 0; i < tile->item_count; i++) {
						if(tile->items[i]) {
							free(tile->items[i]);
						}
					}
					free(tile);
				}
			}
			for(int i = 0; i < MAX_ENEMIES_PER_LEVEL; i++) {
				enemy_t *enemy = room->enemies[i];
				if(enemy) {
					memset(enemy, 0, sizeof(enemy_t));
				}
			}
			room->current_enemy_count = 0;
			room->is_created = false;
			room->global_time = 0;
			room->biome = BIOME_NULL;
			room->is_main_path = false;
			room->door_mask = 0x0;
			if(!room) {
				DEBUG_LOG("%s", "you have failed");
			}
		}
	}

	for(int i = 0; i < world->max_message_storage; i++) {
		memset(world->messages[i], 0, MAX_MESSAGE_LENGTH*sizeof(char));
	}
	memset(world->turn_order, 0, sizeof(int)*world->turn_order_size);
	memset(world->buffs, 0, sizeof(int)*world->buff_size);
	world->room_template_count = 0;

	player_reset_values(player, world);
}

// void setup_game(world_t *world, player_t *player) {
//
// }

void shutdown(world_t *world, player_t *player) {
	for(int y = 0; y < WORLD_HEIGHT; y++) {
		for(int x = 0; x < WORLD_WIDTH; x++) {
			room_t *room = world->room[x][y];
			for(int ty = 0; ty < ROOM_HEIGHT; ty++) {
				for(int tx = 0; tx < ROOM_WIDTH; tx++) {
					tile_t *tile = room->tiles[ty][tx];
					if(tile) {
						for(int i = 0; i < MAX_ITEMS_PER_TILE; i++) {
							if(tile->items[i]) {
								free(tile->items[i]);
							}
						}
						free(tile);
					}
				}
			}
			for(int i = 0; i < MAX_ENEMIES_PER_LEVEL; i++) {
				enemy_t *enemy = room->enemies[i];
				free(enemy);
			}
			free(room);
		}
	}
	free(world->enemy_data);
	free(world->class_data);
	free(world->item_data);
	free(world->trap_data);
	for(int i = 0; i < world->messages_size; i++) {
		free(world->messages[i]);
	}
	free(world->turn_order);
	free(world->buffs);

	free(player->inventory);

	SDL_DestroyRenderer(world->ctx.renderer);
	SDL_DestroyWindow(world->ctx.window);
	SDL_DestroyTexture(world->ctx.texture);
	SDL_Quit();
	free(player);
	free(world);
	exit(0);
}

