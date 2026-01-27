#ifndef GAME_MANAGER_H_
#define GAME_MANAGER_H_
#include <ncurses.h>
#include <stdbool.h>
#include "enums.h"
#include "types.h"

void draw(world_t *world, player_t *player);
bool manage_input(char c, world_t *world, player_t *player, menu_manager_t *menu_manager);
bool sdl_manage_input(SDL_Event event, world_t *world, player_t *player, menu_t *menus);

void display_world_message(world_t *world, const char *str);

void display_combat_message(world_t *world, const char *str);

bool is_opaque(room_t *room, int x, int y);

void mark_has_light(room_t *room, int x, int y);

void cast_light_check(world_t *world, player_t *player, int x0, int y0, float angle);

void calculate_light(world_t *world, player_t *player);

int pick_next_actor(world_t *world, player_t *player);

void turn_order_enter_new_room(world_t *world, player_t *player);

void generate_turn_order_display(world_t *world, player_t *player);

void draw_main_menu(WINDOW *main_menu, menu_manager_t *menu_manager);

void generate_load_menu_list(load_menu_t *load_menu);

void draw_load_menu(const load_menu_t *load_menu);

void display_and_manage_save_menu(WINDOW *win, char *buf, int max_len, world_t *world, player_t *player, menu_manager_t *menu_manager);

void display_and_manage_class_menu(WINDOW *win, world_t *world, player_t *player, menu_manager_t *menu_manager);

void display_spell_equip_menu(player_t *player, popup_menu_t menu);
void display_death_menu(player_t *player, popup_menu_t menu);

void manage_load_menu_input(char c, load_menu_t *load_menu, world_t *world, player_t *player, menu_manager_t *menu_manager);

void manage_menu_input(char c, menu_manager_t *menu_manager, world_t *world, player_t *player);

direction_t direction_from_key(int key);

void end_game(world_t *world, player_t *player);

void return_to_main_menu(world_t *world, player_t *player);

void setup_game(world_t *world, player_t *player);

void shutdown(world_t *world, player_t *player);

#endif
