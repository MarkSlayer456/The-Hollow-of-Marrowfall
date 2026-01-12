#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "map_manager.h"
#include "game_manager.h"
#include "player.h"
#include "items/items.h"
#include "enemy.h"
#include "hud.h"
#include "types.h"
#include "lantern.h"
#include "items/armor.h"
#include "items/weapons.h"
#include "items/spells.h"
#include "items/foods.h"
#include "items/misc.h"
#include "save.h"
#include "traps.h"
#include "buff.h"
#include "pot.h"

/*
	Top down dungeon crawler
	oil is the currency here
	your lamp is powered by oil
	oil is your friend without it you won't last long
*/

WINDOW *main_menu;
WINDOW *hud; // gives player useful information
WINDOW *action_bar; // OLG player's inventory/spells menu, maybe a help menu in the future
WINDOW *error; // USED FOR ERROR CHECKING ONLY
WINDOW *inventory_hud; // player's inventory
WINDOW *inventory_desc_hud; // player's description in the inventory

char walk_chars[WALK_CHAR_LENGTH] = {EMPTY, HOLE, TWISTED_ROOT, STALAGMITE, DOOR, MUD, CHEST}; // characters entites can walk on

int main(int argc, char *argv[]) {
    WINDOW *win;
	
    initscr();
    curs_set(0);
	win = newwin(20, 20, 0, 0);
	box(win,0,0);
    hud = newwin(HUD_HEIGHT, HUD_WIDTH, 0, 21);
	action_bar = newwin(5, 21, 21, 0);
	inventory_hud = newwin(INVENTORY_HEIGHT, INVENTORY_WIDTH, 0, 0);
	inventory_desc_hud = newwin(INVENTORY_HEIGHT, INVENTORY_WIDTH, INVENTORY_HEIGHT, 0);
    error = newwin(25, 25, 51, 30);
	main_menu = newwin(SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0);
    
	load_menu_t load_menu;
	load_menu.win = newwin(SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0);
	load_menu.filename_count = 0;
	load_menu.filename_size = 16;
	load_menu.cursor_pos = 0;
	load_menu.cursor_offset = 0;
	load_menu.filename = calloc(load_menu.filename_size, sizeof(char[SAVE_FILE_MAX_LEN]));

	generate_load_menu_list(&load_menu);
	
    refresh();
    wrefresh(hud);
    wrefresh(error);
    
	noecho();
	nodelay(stdscr, FALSE);
	keypad(stdscr, FALSE);
	scrollok(win, FALSE);
	raw();
	
	ensure_save_folder();
	
	srand(time(NULL));
	
	if(argc != 1) {
		fprintf(stderr, "No arguments required!\n");
	}

	world_t *world = malloc(sizeof(world_t));
	world->is_player_turn = false;
	world->enemy_data = calloc(MAX_ENEMIES, sizeof(enemy_data_t));
	for(int row = 0; row < MAX_ENEMIES; row++) {
		for(int i = 0; i < NUMBER_OF_BIOMES; i++) {
			world->enemy_data[row].type = ENEMY_NONE;
			world->enemy_data[row].can_spawn[i] = false;
			world->enemy_data[row].lowest_level[i] = -1;
			world->enemy_data[row].highest_level[i]= -1;
		}
	}
	
	world->class_data = calloc(MAX_CLASSES, sizeof(class_data_t));
	world->item_data = calloc(MAX_ITEMS, sizeof(item_data_t));
	for(int i = 0; i < MAX_ITEMS; i++) {
		world->item_data[i] = (item_data_t){0};
	}
	
	world->trap_data = calloc(MAX_TRAPS, sizeof(trap_data_t));
	for(int i = 0; i < MAX_TRAPS; i++) {
		world->trap_data[i] = (trap_data_t){0};
	}
	
	world->max_message_storage = DEFAULT_MAX_MESSAGE_STORAGE;
	world->messages_size = 0;
	world->messages = calloc(world->max_message_storage, sizeof(char *));
	for(int i = 0; i < world->max_message_storage; i++) {
		world->messages[i] = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
	}
	
	world->item_data_count = 0;
	world->room_template_count = 0;
	world->trap_data_count = 0;

	world->buff_size = STARTING_BUFF_SIZE;
	world->buff_count = 0;
	world->buffs = calloc(world->buff_size, sizeof(buff_t));
	
	load_enemy_data(world->enemy_data);
	load_enemy_drop_data(world->enemy_data);
	load_class_data(world->class_data);
	load_armor_data(world);
	load_weapon_data(world);
	load_spell_data(world);
	load_foods_data(world);
	load_misc_data(world);
	load_trap_data(world);
	load_pot_data(world);
	
	player_t *player = calloc(1, sizeof(player_t));

	player_setup(player, world);

	world->turn_order = calloc(MAX_ENEMIES_PER_LEVEL+1, sizeof(int));
	world->turn_order_size = 0;
	for(int y = 0; y < WORLD_HEIGHT; y++) {
		for(int x = 0; x < WORLD_WIDTH; x++) {
			world->room[x][y] = calloc(1, sizeof(room_t));

			for(int i = 0; i < MAX_ENEMIES_PER_LEVEL; i++) {
				world->room[x][y]->enemies[i] = calloc(1, sizeof(enemy_t));
			}
		}
	}
	room_template_t empty = {0};
	world->seed = TEST_SEED;
	for(int i = 0; i < 128; i++) {
		world->room_templates[i] = empty;
	}
	
	room_t *first = setup_first_room(&world->seed, 0, 0, world->enemy_data, world->item_data, world);

	world->room[0][0] = first;

	world->room[0][0]->door_mask = 0x6;
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, FIREBALL_SPELL_BOOK, 1);
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, POISON_SPELL_BOOK, 1);
	calculate_door_masks(world);
	calculate_main_path(&world->seed, world);
	
	world->win = win;
    world->turn_order_size = 0;
	
	for(;;) {
		switch(player->menu_manager.current_menu) {
			case GAME:
				if(world->room[0][0]->is_created == false) {
					//TODO reset world function
					world->room[0][0] = load_room(&world->seed, 0, 0, world->enemy_data, world->item_data, world);
					enemy_spawn(world->room[0][0]->enemies[0], BAT, world->enemy_data, 1, 1, 0, 0, world->room[0][0]->biome);
					world->room[0][0]->current_enemy_count++;
					calculate_door_masks(world);
					calculate_main_path(&world->seed, world);
					world->turn_order_size = 0;
					drop_item(world->room[0][0]->tiles[10][1], world->item_data, FIREBALL_SPELL_BOOK, 1);
				}
				calculate_light(world, player);
				generate_turn_order_display(world, player);
				for(int i = 0; i < world->room[player->global_x][player->global_y]->current_enemy_count; i++) {
					enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[i];
					if(enemy != NULL) {
						enemy_handle_lighting_buff(enemy, world);
					}
				}
				draw(world, player);
				player_get_nearby_loot(world->room[player->global_x][player->global_y], player);
				int actor = INVALID_ACTOR_INDEX;

				if(!world->is_player_turn) {
					actor = pick_next_actor(world, player);
					assert(actor != INVALID_ACTOR_INDEX);
				}

				if(world->is_player_turn || actor == PLAYER_TURN_ORDER_INDEX) {
					lantern_update_dimming(&player->lantern);
					draw(world, player);
					bool run = false;
					char c;
					while(run == false) {
						c = getch();
						run = manage_input(c, world, player, &player->menu_manager);
						draw(world, player);
					}
					traps_triggered_check_player(world, player);
					buff_apply(world->buffs, &world->buff_count, world);
				} else if(actor >= 0) {
					enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[actor];
					if(enemy != NULL) {
						enemy_handle_lighting_buff(enemy, world);
						enemy_decide_move(enemy, world, player);
					}
				}
				traps_triggered_check_enemies(world, world->room[player->global_x][player->global_y]);
				break;
			case MAIN_MENU: { // this bracket must be here and it infuriates me
				draw_main_menu(main_menu, &player->menu_manager);
				char c = getch();
				manage_menu_input(c, &player->menu_manager, world, player);
				break;
			}
			case LOAD_MENU: {
				draw_load_menu(&load_menu);
				char c = getch();
				manage_load_menu_input(c, &load_menu, world, player, &player->menu_manager);
				break;
			}
			case SAVE_MENU: {
				char buf[SAVE_FILE_MAX_LEN];
				display_and_manage_save_menu(main_menu, buf, SAVE_FILE_MAX_LEN, world, player, &player->menu_manager);
				break;
			}
			case LOG_BOOK_MENU:
				break;
			case CLASS_MENU:
				display_and_manage_class_menu(main_menu, world, player, &player->menu_manager);
				break;
			case NULL_MENU:
				player->menu_manager.current_menu = MAIN_MENU;
				DEBUG_LOG("%s", "WARNING: NULL_MENU WAS SET, RESETTING TO MAIN_MENU");
				break;
		}
    }
    endwin();
    exit(0);
    return 0;
}
