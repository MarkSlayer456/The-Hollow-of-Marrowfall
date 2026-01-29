#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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
#include "renderer.h"
#include "menu.h"

/*
	Top down dungeon crawler
	oil is the currency here
	your lamp is powered by oil
	oil is your friend without it you won't last long
*/

char walk_chars[WALK_CHAR_LENGTH] = {EMPTY, HOLE, TWISTED_ROOT, STALAGMITE, DOOR, MUD, CHEST}; // characters entites can walk on

int main(int argc, char *argv[]) {
	
	ensure_save_folder();
	
	srand(time(NULL));
	
	if(argc != 1) {
		fprintf(stderr, "No arguments required!\n");
	}

	world_t *world = malloc(sizeof(world_t));
	world->is_player_turn = true;
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

	world->action_points = 0;
	world->turn_has_passed = false;

	world->room[0][0]->door_mask = 0x6;
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, FIREBALL_SPELL_BOOK, 1);
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, POISON_SPELL_BOOK, 1);
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, HEALTH_POTION, 1);
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, APPLE, 1);
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, ROTTEN_APPLE, 1);
	drop_item(world->room[0][0]->tiles[10][1], world->item_data, CHICKEN_DINNER, 1);
	drop_item(world->room[0][0]->tiles[10][2], world->item_data, RAT_MEAT, 1);
	drop_item(world->room[0][0]->tiles[10][2], world->item_data, DRAGONFIRE_JERKY, 1);
	drop_item(world->room[0][0]->tiles[10][2], world->item_data, CELESTIAL_CORNBREAD, 1);
	drop_item(world->room[0][0]->tiles[10][2], world->item_data, NIGHTBLOOM_GRAPES, 1);
	drop_item(world->room[0][0]->tiles[10][2], world->item_data, PIZZA, 1);
	drop_item(world->room[0][0]->tiles[11][1], world->item_data, BEAR_CHILI, 1);
	drop_item(world->room[0][0]->tiles[11][1], world->item_data, BLACKSTONE_ARMOR, 1);
	drop_item(world->room[0][0]->tiles[11][1], world->item_data, BRONZE_ARMOR, 1);

	calculate_door_masks(world);
	calculate_main_path(&world->seed, world);
	
	world->ctx = SDL_setup();
	load_textures(&world->ctx);
    world->turn_order_size = 0;

	SDL_RenderClear(world->ctx.renderer);
	SDL_RenderPresent(world->ctx.renderer);
	menu_t menus[MENU_COUNT] = {
		[MAIN_MENU] = {0},
		[LOAD_MENU] = {0},
		[SAVE_MENU] = {0},
		[LOG_BOOK_MENU] = {0},
		[CLASS_MENU] = {0},
		[GAME] = {0},
		[INVENTORY_MENU] = {0},
		[LOOT_MENU] = {0},
		[SPELL_MENU] = {0},
	};
	hud_t game_hud = {0};
	// menu_t main_menu = {0};
	main_menu_init(player, &menus[MAIN_MENU], &world->ctx);
	class_menu_init(player, &menus[CLASS_MENU], &world->ctx);
	load_menu_init(player, &menus[LOAD_MENU], &world->ctx);
	inventory_menu_init(player, &menus[INVENTORY_MENU], &world->ctx, (SDL_Color){255, 255, 255, 255});
	loot_inventory_menu_init(world, player, &menus[LOOT_MENU], &world->ctx,(SDL_Color){255, 255, 255, 255});
	spell_menu_init(player, &menus[SPELL_MENU], &world->ctx);
	hud_init(player, &game_hud, &world->ctx);

	int running = 1;
	SDL_Event event;
	int actor = 0;

	const Uint32 FRAME_MS = 16;
	while(running) {
		SDL_SetRenderDrawColor(world->ctx.renderer, 0, 0, 0, 255); // black background
		SDL_RenderClear(world->ctx.renderer);
		Uint32 start = SDL_GetTicks();
		switch(player->state) {
			case PLAYER_STATE_MOVING:
			case PLAYER_STATE_ATTACKING:
			case PLAYER_STATE_VIEWING:
				if(world->room[0][0]->is_created == false) {
					//TODO reset world function
					world->room[0][0] = load_room(&world->seed, 0, 0, world->enemy_data, world->item_data, world);
					calculate_door_masks(world);
					calculate_main_path(&world->seed, world);
					world->turn_order_size = 0;
					drop_item(world->room[0][0]->tiles[10][1], world->item_data, FIREBALL_SPELL_BOOK, 1);
					world->turn_has_passed = false;
					world->action_points = 0;
				}
				if(world->turn_has_passed) {
					for(int i = 0; i < world->room[player->global_x][player->global_y]->current_enemy_count; i++) {
						enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[i];
						if(enemy != NULL) {
							enemy_handle_lighting_buff(enemy, world);
						}
					}
					player_get_nearby_loot(world->room[player->global_x][player->global_y], player);
					if(actor == INVALID_ACTOR_INDEX) {
						actor = pick_next_actor(world, player);
					}
					assert(actor != INVALID_ACTOR_INDEX);

					if(actor == PLAYER_TURN_ORDER_INDEX) {
						world->turn_has_passed = false;
						game_hud.needs_redraw = true;
						actor = INVALID_ACTOR_INDEX;
					} else if(actor >= 0) {
						enemy_t *enemy = world->room[player->global_x][player->global_y]->enemies[actor];
						if(enemy != NULL) {
							enemy_handle_lighting_buff(enemy, world);
							enemy_decide_move(enemy, world, player);
						}
						actor = INVALID_ACTOR_INDEX;
					} else if(actor == WORLD_TURN_ORDER_INDEX) {
						lantern_update_dimming(&player->lantern);
						traps_triggered_check_player(world, player);
						buff_apply(world->buffs, &world->buff_count, world);
						actor = INVALID_ACTOR_INDEX;
					}
					calculate_light(world, player);
					traps_triggered_check_enemies(world, world->room[player->global_x][player->global_y]);
				}
				generate_turn_order_display(world, player);
				hud_update_textures(world, player, &game_hud, &world->ctx);
				render_game(&world->ctx, world, player);
				// render_game_hud(&world->ctx, world, player, &actor);
				ui_render(game_hud.data, game_hud.data_size, world->ctx.renderer);
				break;
			case PLAYER_STATE_MAIN_MENU: { // this bracket must be here and it infuriates me
				world->turn_has_passed = false;
				menu_update_cursor_pos(&menus[MAIN_MENU]);
				menu_render(&menus[MAIN_MENU], world->ctx.renderer);
				// char c = getch();
				// manage_menu_input(c, &player->menu_manager, world, player);
				break;
			}
			case PLAYER_STATE_LOAD_MENU: {
				world->turn_has_passed = false;
				menu_update_cursor_pos(&menus[LOAD_MENU]);
				menu_render(&menus[LOAD_MENU], world->ctx.renderer);
				break;
			}
			case PLAYER_STATE_SAVE_MENU: {
				world->turn_has_passed = false;
				// char buf[SAVE_FILE_MAX_LEN];
				// display_and_manage_save_menu(main_menu_win, buf, SAVE_FILE_MAX_LEN, world, player, &player->menu_manager);
				break;
			}
			case PLAYER_STATE_LOG_BOOK:
				world->turn_has_passed = false;
				break;
			case PLAYER_STATE_CLASS_MENU:
				world->turn_has_passed = false;
				menu_update_cursor_pos(&menus[CLASS_MENU]);
				menu_render(&menus[CLASS_MENU], world->ctx.renderer);
				break;
			case PLAYER_STATE_INVENTORY:
			case PLAYER_STATE_LOOTING:
				world->turn_has_passed = false;
				SDL_RenderClear(world->ctx.renderer);
				if(player->state == PLAYER_STATE_INVENTORY) {
					inventory_update_textures(player, &menus[INVENTORY_MENU], &world->ctx, (SDL_Color){255, 255, 255, 255});
					loot_inventory_update_textures(player, &menus[LOOT_MENU], &world->ctx, (SDL_Color){128, 128, 128, 255});
				} else {
					inventory_update_textures(player, &menus[INVENTORY_MENU], &world->ctx, (SDL_Color){128, 128, 128, 255});
					loot_inventory_update_textures(player, &menus[LOOT_MENU], &world->ctx, (SDL_Color){255, 255, 255, 255});
				}

				menu_update_cursor_pos(&menus[INVENTORY_MENU]);
				menu_update_cursor_pos(&menus[LOOT_MENU]);

				menu_render(&menus[INVENTORY_MENU], world->ctx.renderer);
				menu_render(&menus[LOOT_MENU], world->ctx.renderer);
				break;
			case PLAYER_STATE_EQUIPPING_SPELL:
				world->turn_has_passed = false;
				SDL_RenderClear(world->ctx.renderer);
				spell_update_textures(player, &menus[SPELL_MENU], &world->ctx);

				menu_update_cursor_pos(&menus[SPELL_MENU]);

				menu_render(&menus[SPELL_MENU], world->ctx.renderer);
				break;
			default:
				world->turn_has_passed = false;
				break;
		}

		while(SDL_PollEvent(&event)) {
			if(!world->turn_has_passed) {
				world->turn_has_passed = sdl_manage_input(event, world, player, menus);
			}

			switch(event.type) {
				case SDL_QUIT:
					end_game(world, player);
					running = 0;
					break;
			}
		}

		SDL_RenderPresent(world->ctx.renderer);

		Uint32 elapsed = SDL_GetTicks() - start;
		if(elapsed < FRAME_MS) {
			SDL_Delay(FRAME_MS - elapsed);
		}

    }
    return 0;
}
