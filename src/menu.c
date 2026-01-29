#include "renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "player.h"
#include "menu.h"
#include "save.h"
#include "functions.h"
#include "items/items.h"

void main_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx) {
	static ui_data_t main_menu_items[] = {
		{ .sdl_data = { NULL, { .x = 50, .y = 50, .w = 250, .h = 50} }, .data_type.const_data = { .str = GAME_TITLE, .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 100, .w = 250, .h = 50} }, .data_type.const_data = { .str = MENU_DIVIDE_LINE, .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 150, .w = 250, .h = 50} }, .data_type.const_data = { .str = "New Game", .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 200, .w = 250, .h = 50} }, .data_type.const_data = { .str = "Load Game", .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 250, .w = 250, .h = 50} }, .data_type.const_data = { .str = "Log Book", .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.const_data = { .str = "--> ", .font_size = 18}},
	};

	static player_state_t dests[] = {
		PLAYER_STATE_CLASS_MENU,
		PLAYER_STATE_LOAD_MENU,
		PLAYER_STATE_LOG_BOOK
	};
	menu->menu_item_size = 6;
	menu->selected = 0;
	menu->data = main_menu_items;
	menu->dests = dests;
	menu->select_start_offset = 2;
	menu->selectable_size = 3;
	menu->operation = go_to_dest;
	menu->operation_ctx1 = menu;
	menu->operation_ctx2 = player;
	menu->operation_ctx3 = NULL;
	menu->display_max = 6;
	menu->offset = 0;
	menu->offset_max = 0;

	for(int i = 0; i < menu->menu_item_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.const_data.font_size);
		if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
		SDL_Color white = {255, 255, 255, 255};

		SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.const_data.str, white);
		if(!surface) DEBUG_LOG("%s", "surface error");
		menu->data[i].sdl_data.rect.w = surface->w;
		menu->data[i].sdl_data.rect.h = surface->h;
		if(i+1 == menu->menu_item_size) {
			menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
			menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
		}
		menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
	}
}

void class_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx) {
	static ui_data_t class_menu_items[CLASS_COUNT+1];
	static player_state_t dests[CLASS_COUNT];
	for(int i = 0; i < CLASS_COUNT; i++) {
		class_menu_items[i] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 25*(i+1), .w = 250, .h = 50} }, .data_type.const_data = { .str = class_get_name(i), .font_size = 18}};
		dests[i] = PLAYER_STATE_MOVING;
	}
	class_menu_items[CLASS_COUNT] = (ui_data_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.const_data = { .str = "--> ", .font_size = 18}};


	menu->menu_item_size = CLASS_COUNT+1;
	menu->selected = 0;
	menu->data = class_menu_items;
	menu->dests = dests;
	menu->select_start_offset = 0;
	menu->selectable_size = CLASS_COUNT;
	menu->operation = confirm_class;
	menu->operation_ctx1 = menu;
	menu->operation_ctx2 = player;
	menu->operation_ctx3 = NULL;
	menu->display_max = CLASS_COUNT+1;
	menu->offset = 0;
	menu->offset_max = 0;

	for(int i = 0; i < menu->menu_item_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.const_data.font_size);
		if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
		SDL_Color white = {255, 255, 255, 255};

		SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.const_data.str, white);
		menu->data[i].sdl_data.rect.w = surface->w;
		menu->data[i].sdl_data.rect.h = surface->h;
		if(i+1 == menu->menu_item_size) {
			menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
			menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
		}
		if(!surface) DEBUG_LOG("%s", "surface error");
		menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
	}
}

void load_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx) {

	int filenames_capacity = 32;
	int filenames_count = 0;
	char **filenames = calloc(filenames_capacity, sizeof(char *));
	for(int i = 0; i < filenames_capacity; i++) {
		filenames[i] = calloc(SAVE_FILE_MAX_LEN, sizeof(char));
	}

	DIR *dir = opendir(get_save_path());

	if(!dir) {
		perror("opendir failed");
		return;
	}
	struct dirent *entry;
	while((entry = readdir(dir)) != NULL) {
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		if(filenames_count >= filenames_capacity) {
			filenames_capacity *= 2;
			filenames = realloc(filenames, filenames_capacity * sizeof(char *));
		}
		strcpy(filenames[filenames_count], entry->d_name);
		filenames_count++;
	}

	ui_data_t *load_menu_items = calloc(filenames_count+1, sizeof(ui_data_t));
	player_state_t *dests = calloc(filenames_count, sizeof(player_state_t));
	for(int i = 0; i < filenames_count; i++) {
		load_menu_items[i] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 25*(i+1), .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 18}};
		strcpy(load_menu_items[i].data_type.dynamic_data.str, filenames[i]);
		dests[i] = PLAYER_STATE_MOVING;
	}

	load_menu_items[filenames_count] = (ui_data_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};

	strcpy(load_menu_items[filenames_count].data_type.dynamic_data.str, "-->");
	menu->menu_item_size = filenames_count+1;
	menu->selected = 0;
	menu->data = load_menu_items;
	menu->dests = dests;
	menu->select_start_offset = 0;
	menu->selectable_size = filenames_count;
	menu->operation = go_to_dest;
	menu->operation_ctx1 = menu;
	menu->operation_ctx2 = NULL;
	menu->operation_ctx3 = NULL;
	menu->display_max = MAX_MENU_ITEMS_DISPLAYED;
	menu->offset = 0;
	menu->offset_max = 0;

	for(int i = 0; i < menu->menu_item_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.dynamic_data.font_size);
		if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
		SDL_Color white = {255, 255, 255, 255};

		SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.dynamic_data.str, white);
		menu->data[i].sdl_data.rect.w = surface->w;
		menu->data[i].sdl_data.rect.h = surface->h;
		if(i+1 == menu->menu_item_size) {
			menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
			menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
		}
		if(!surface) DEBUG_LOG("%s", "surface error");
		menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
	}

	for(int i = 0; i < filenames_count; i++) {
		if(filenames[i]) free(filenames[i]);
	}
	free(filenames);

}

void inventory_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color) {
	const int size = 10+13;
	const int lines_for_desc = 10;
	ui_data_t *inventory_menu_items = calloc(size, sizeof(ui_data_t));
	player_state_t *dests = calloc(size, sizeof(player_state_t));

	inventory_menu_items[0] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 25, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(inventory_menu_items[0].data_type.dynamic_data.str, "Inventory");
	inventory_menu_items[1] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 50, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(inventory_menu_items[1].data_type.dynamic_data.str, MENU_DIVIDE_LINE);

	for(int i = 2; i < lines_for_desc+2; i++) {
		inventory_menu_items[i] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 25*(i+12), .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
	}

	int inv_index = 0;
	for(int i = 12; i < size-1; i++) {
		inventory_menu_items[i] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 25*((i-10)+1), .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 14}};
		strcpy(inventory_menu_items[i].data_type.dynamic_data.str, player->inventory[inv_index].name);
		dests[i] = PLAYER_STATE_MOVING; //TODO.. this probably won't be used anyway
		inv_index++;
	}

	inventory_menu_items[size-1] = (ui_data_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
	strcpy(inventory_menu_items[size-1].data_type.dynamic_data.str, "-->");

	menu->menu_item_size = size;
	menu->selected = 0;
	menu->data = inventory_menu_items;
	menu->dests = dests;
	menu->select_start_offset = 12;
	menu->selectable_size = player->inventory_count > 10 ? 10 : player->inventory_count;
	menu->operation = menu_use_item;
	menu->operation_ctx1 = menu;
	menu->operation_ctx2 = player;
	menu->operation_ctx3 = NULL;
	menu->display_max = 8;
	menu->offset = 0;
	menu->offset_max = player->inventory_count-menu->selectable_size;
	menu->needs_redraw = true;

	for(int i = 0; i < menu->menu_item_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.dynamic_data.font_size);
		if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());

		if(strlen(menu->data[i].data_type.dynamic_data.str) == 0 ||
			strcmp(menu->data[i].data_type.dynamic_data.str, BLANK_NAME) == 0) continue;
		SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.dynamic_data.str, color);
		if(!surface) DEBUG_LOG("%s %s", "surface error: ", TTF_GetError());
		menu->data[i].sdl_data.rect.w = surface->w;
		menu->data[i].sdl_data.rect.h = surface->h;
		if(i+1 == menu->menu_item_size) {
			menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
			menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
		}
		menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
	}
}

void inventory_update_textures(player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color) {
	if(menu->needs_redraw) {
		menu->needs_redraw = false;
		const int lines_for_desc = 10;
		menu->selectable_size = player->inventory_count > 10 ? 10 : player->inventory_count;
		menu->offset_max = player->inventory_count-menu->selectable_size;
		if(menu->offset_max < menu->offset) {
			menu->offset = menu->offset_max;
		}
		int inv_index = menu->offset;
		for(int i = 12; i < menu->menu_item_size-1; i++) {
			TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.dynamic_data.font_size);
			if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());

			menu->data[i].sdl_data.rect.y = 50+(25*(inv_index+1)-25*menu->offset);

			strcpy(menu->data[i].data_type.dynamic_data.str, "");

			if(player->equipment.spell3 == inv_index || player->equipment.spell2 == inv_index || player->equipment.spell1 == inv_index || player->equipment.armor == inv_index || player->equipment.main_hand == inv_index || player->equipment.off_hand == inv_index) {
				if(player->equipment.spell1 == inv_index) {
					strcat(menu->data[i].data_type.dynamic_data.str, "(E) [SP 1] ");
				} else if(player->equipment.spell2 == inv_index) {
					strcat(menu->data[i].data_type.dynamic_data.str, "(E) [SP 2] ");
				} else if(player->equipment.spell3 == inv_index) {
					strcat(menu->data[i].data_type.dynamic_data.str, "(E) [SP 3] ");
				} else {
					strcat(menu->data[i].data_type.dynamic_data.str, "(E) ");
				}
			}

			if(player->inventory[inv_index].stack == 0) {
				strcpy(menu->data[i].data_type.dynamic_data.str, " ");
			} else {
				strcat(menu->data[i].data_type.dynamic_data.str, player->inventory[inv_index].name);
				strcat(menu->data[i].data_type.dynamic_data.str, " x");
				char buf[8];
				snprintf(buf, sizeof(buf), "%d",player->inventory[inv_index].stack);
				strcat(menu->data[i].data_type.dynamic_data.str, buf);
			}
			// if(player->inventory[inv_index].has_changed == true) {
			item_mark_as_unchanged(&player->inventory[inv_index]);
			//TODO delete texture
			SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.dynamic_data.str, color);
			if(!surface) DEBUG_LOG("%s %s", "surface error: ", TTF_GetError());
			menu->data[i].sdl_data.rect.w = surface->w;
			menu->data[i].sdl_data.rect.h = surface->h;
			menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
			SDL_FreeSurface(surface);
			// }
			TTF_CloseFont(font);

			inv_index++;

		}
		char *tmp = my_strdup(player->inventory[menu->selected+menu->offset].desc);
		char *desc = strtok(tmp, "\n");
		for(int j = 2; j < lines_for_desc+2; j++) {
			if(!desc) break;

			TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[j].data_type.dynamic_data.font_size);
			if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());

			strcpy(menu->data[j].data_type.dynamic_data.str, desc);
			//TODO delete texture
			SDL_Surface *desc_surface = TTF_RenderText_Solid(font, menu->data[j].data_type.dynamic_data.str, color);
			if(!desc_surface) DEBUG_LOG("%s %s", "surface error: ", SDL_GetError());
			menu->data[j].sdl_data.rect.w = desc_surface->w;
			menu->data[j].sdl_data.rect.h = desc_surface->h;
			menu->data[j].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, desc_surface);
			SDL_FreeSurface(desc_surface);

			desc = strtok(NULL, "\n");
			TTF_CloseFont(font);
		}
		free(tmp);
	}
}

void loot_inventory_menu_init(world_t *world, player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color) {
	const int size = 10+13;
	const int lines_for_desc = 10;
	ui_data_t *loot_inventory_menu_items = calloc(size, sizeof(ui_data_t));
	player_state_t *dests = calloc(size, sizeof(player_state_t));

	loot_inventory_menu_items[0] = (ui_data_t){ .sdl_data = { NULL, { .x = 462, .y = 25, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(loot_inventory_menu_items[0].data_type.dynamic_data.str, "Loot Menu");
	loot_inventory_menu_items[1] = (ui_data_t){ .sdl_data = { NULL, { .x = 462, .y = 50, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(loot_inventory_menu_items[1].data_type.dynamic_data.str, MENU_DIVIDE_LINE);

	for(int i = 2; i < lines_for_desc+2; i++) {
		loot_inventory_menu_items[i] = (ui_data_t){ .sdl_data = { NULL, { .x = 462, .y = 25*(i+12), .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
	}
	int loot_index = 0;
	for(int i = 12; i < size-1; i++) {
		loot_inventory_menu_items[i] = (ui_data_t){ .sdl_data = { NULL, { .x = 462, .y = 25*((i-10)+1), .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 14}};
		dests[i] = PLAYER_STATE_MOVING; //TODO.. this probably won't be used anyway
		loot_index++;
	}

	loot_inventory_menu_items[size-1] = (ui_data_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
	strcpy(loot_inventory_menu_items[size-1].data_type.dynamic_data.str, "-->");

	menu->menu_item_size = size;
	menu->selected = 0;
	menu->data = loot_inventory_menu_items;
	menu->dests = dests;
	menu->select_start_offset = 12;
	menu->selectable_size = player->nearby_loot_count > 10 ? 10 : player->nearby_loot_count;
	menu->operation = menu_loot_item;
	menu->operation_ctx1 = menu;
	menu->operation_ctx2 = world;
	menu->operation_ctx3 = player;
	menu->display_max = 8;
	menu->offset = 0;
	menu->offset_max = player->nearby_loot_count-menu->selectable_size;
	menu->needs_redraw = true;

	for(int i = 0; i < menu->menu_item_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.dynamic_data.font_size);
		if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());

		if(strlen(menu->data[i].data_type.dynamic_data.str) == 0 ||
			strcmp(menu->data[i].data_type.dynamic_data.str, BLANK_NAME) == 0) continue;
		SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.dynamic_data.str, color);
		if(!surface) DEBUG_LOG("%s %s", "surface error: ", TTF_GetError());
		menu->data[i].sdl_data.rect.w = surface->w;
		menu->data[i].sdl_data.rect.h = surface->h;
		if(i+1 == menu->menu_item_size) {
			menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
			menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
		}
		menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
	}
}

void loot_inventory_update_textures(player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color) {
	if(menu->needs_redraw) {
		menu->needs_redraw = false;
		const int lines_for_desc = 10;
		menu->selectable_size = player->nearby_loot_count > 10 ? 10 : player->nearby_loot_count;
		menu->offset_max = player->nearby_loot_count-menu->selectable_size;
		if(menu->offset_max < menu->offset) {
			menu->offset = menu->offset_max;
		}
		int inv_index = menu->offset;
		for(int i = 12; i < menu->menu_item_size-1; i++) {
			TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.dynamic_data.font_size);
			if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());

			menu->data[i].sdl_data.rect.y = 50+(25*(inv_index+1)-25*menu->offset);

			if(player->nearby_loot[inv_index] == NULL) {
				strcpy(menu->data[i].data_type.dynamic_data.str, " ");
			} else {
				strcpy(menu->data[i].data_type.dynamic_data.str, player->nearby_loot[inv_index]->name);
			}

			// if(player->nearby_loot[inv_index].has_changed == true) {
			// item_mark_as_unchanged(player->nearby_loot[inv_index]);
			//TODO delete texture
			SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.dynamic_data.str, color);
			if(!surface) DEBUG_LOG("%s %s", "surface error: ", TTF_GetError());
			menu->data[i].sdl_data.rect.w = surface->w;
			menu->data[i].sdl_data.rect.h = surface->h;
			menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
			SDL_FreeSurface(surface);
			// }
			TTF_CloseFont(font);

			inv_index++;
		}

		if(!player->nearby_loot[menu->selected+menu->offset]) {
			for(int j = 2; j < lines_for_desc+2; j++) {

				TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[j].data_type.dynamic_data.font_size);
				if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());

				strcpy(menu->data[j].data_type.dynamic_data.str, " ");
				//TODO delete texture
				SDL_Surface *desc_surface = TTF_RenderText_Solid(font, menu->data[j].data_type.dynamic_data.str, color);
				if(!desc_surface) DEBUG_LOG("%s %s", "surface error: ", SDL_GetError());
				menu->data[j].sdl_data.rect.w = desc_surface->w;
				menu->data[j].sdl_data.rect.h = desc_surface->h;
				menu->data[j].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, desc_surface);
				SDL_FreeSurface(desc_surface);

				TTF_CloseFont(font);
			}
		} else {
			char *tmp = my_strdup(player->nearby_loot[menu->selected+menu->offset]->desc);
			char *desc = strtok(tmp, "\n");
			for(int j = 2; j < lines_for_desc+2; j++) {
				if(!desc) break;

				TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[j].data_type.dynamic_data.font_size);
				if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
				SDL_Color white = {255, 255, 255, 255};

				strcpy(menu->data[j].data_type.dynamic_data.str, desc);
				//TODO delete texture
				SDL_Surface *desc_surface = TTF_RenderText_Solid(font, menu->data[j].data_type.dynamic_data.str, white);
				if(!desc_surface) DEBUG_LOG("%s %s", "surface error: ", SDL_GetError());
				menu->data[j].sdl_data.rect.w = desc_surface->w;
				menu->data[j].sdl_data.rect.h = desc_surface->h;
				menu->data[j].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, desc_surface);
				SDL_FreeSurface(desc_surface);

				desc = strtok(NULL, "\n");
				TTF_CloseFont(font);
			}
			free(tmp);
		}
	}
}

void spell_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx) {
	const int size = 6;
	ui_data_t *spell_menu_items = calloc(size, sizeof(ui_data_t));
	player_state_t *dests = calloc(size, sizeof(player_state_t));

	spell_menu_items[0] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 25, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(spell_menu_items[0].data_type.dynamic_data.str, "Which Spell Slot?");
	spell_menu_items[1] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 50, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(spell_menu_items[1].data_type.dynamic_data.str, MENU_DIVIDE_LINE);

	for(int i = 2; i < size-1; i++) {
		spell_menu_items[i] = (ui_data_t){ .sdl_data = { NULL, { .x = 50, .y = 25*i+1, .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 14}};
		strcpy(spell_menu_items[i].data_type.dynamic_data.str, " ");
		dests[i] = PLAYER_STATE_MOVING; //TODO.. this probably won't be used anyway
	}

	spell_menu_items[size-1] = (ui_data_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
	strcpy(spell_menu_items[size-1].data_type.dynamic_data.str, "-->");

	menu->menu_item_size = size;
	menu->selected = 0;
	menu->data = spell_menu_items;
	menu->dests = dests;
	menu->select_start_offset = 2;
	menu->selectable_size = 3;
	menu->operation = menu_use_item;
	menu->operation_ctx1 = menu;
	menu->operation_ctx2 = player;
	menu->operation_ctx3 = NULL;
	menu->display_max = 3;
	menu->offset = 0;
	menu->offset_max = 0;
	menu->needs_redraw = true;

	for(int i = 0; i < menu->menu_item_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.dynamic_data.font_size);
		if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
		SDL_Color white = {255, 255, 255, 255};

		SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.dynamic_data.str, white);
		if(!surface) DEBUG_LOG("%s %s", "surface error: ", TTF_GetError());
		menu->data[i].sdl_data.rect.w = surface->w;
		menu->data[i].sdl_data.rect.h = surface->h;
		if(i+1 == menu->menu_item_size) {
			menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
			menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
		}
		menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
	}
}

void spell_update_textures(player_t *player, menu_t *menu, SDL_Context *ctx) {
	if(menu->needs_redraw) {
		menu->needs_redraw = false;
		int cur_spell = menu->offset;
		for(int i = 2; i < menu->menu_item_size-1; i++) {
			TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", menu->data[i].data_type.dynamic_data.font_size);
			if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
			SDL_Color white = {255, 255, 255, 255};

			menu->data[i].sdl_data.rect.y = 50+(25*(cur_spell+1)-25*menu->offset);

			char buf[16];
			snprintf(buf, sizeof(buf), "%d. ", cur_spell+1);

			strcpy(menu->data[i].data_type.dynamic_data.str, buf);

			if(cur_spell == 0) {
				if(player->equipment.spell1 >= 0) {
					strcat(menu->data[i].data_type.dynamic_data.str, player->inventory[player->equipment.spell1].name);
				}
			} else if(cur_spell == 1) {
				if(player->equipment.spell2 >= 0) {
					strcat(menu->data[i].data_type.dynamic_data.str, player->inventory[player->equipment.spell2].name);
				}
			} else if(cur_spell == 2) {
				if(player->equipment.spell3 >= 0) {
					strcat(menu->data[i].data_type.dynamic_data.str, player->inventory[player->equipment.spell3].name);
				}
			}

			//TODO delete texture
			SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].data_type.dynamic_data.str, white);
			if(!surface) DEBUG_LOG("%s %s", "surface error: ", TTF_GetError());
			menu->data[i].sdl_data.rect.w = surface->w;
			menu->data[i].sdl_data.rect.h = surface->h;
			menu->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
			SDL_FreeSurface(surface);
			TTF_CloseFont(font);

			cur_spell++;
		}
	}
}

void hud_init(player_t *player, hud_t *hud, SDL_Context *ctx) {
	const int size = 7+MAX_ENEMIES_PER_LEVEL;
	int start_x = 22*20+1;
	ui_data_t *hud_data = calloc(size, sizeof(ui_data_t));

	for(int i = 0; i < size-1; i++) {
		hud_data[i] = (ui_data_t){
			.sdl_data = { NULL, { .x = start_x, .y = 0, .w = 0, .h = 0} },
			.data_type.dynamic_data = { .font_size = 18}};
	}

	hud->data = hud_data;
	hud->data_size = size;
	hud->needs_redraw = true;

	//TODO delete this
	// for(int i = 0; i < hud->menu_item_size; i++) {
	// 	TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", hud->data[i].data_type.dynamic_data.font_size);
	// 	if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
 //
	// 	if(strlen(hud->data[i].data_type.dynamic_data.str) == 0 ||
	// 		strcmp(hud->data[i].data_type.dynamic_data.str, BLANK_NAME) == 0) continue;
	// 	SDL_Surface *surface = TTF_RenderText_Solid(font, hud->data[i].data_type.dynamic_data.str, color);
	// 	if(!surface) DEBUG_LOG("%s %s", "surface error: ", TTF_GetError());
	// 	hud->data[i].sdl_data.rect.w = surface->w;
	// 	hud->data[i].sdl_data.rect.h = surface->h;
	// 	if(i+1 == hud->menu_item_size) {
	// 		hud->data[i].sdl_data.rect.x = hud->data[hud->selected+hud->select_start_offset].sdl_data.rect.x - hud->data[i].sdl_data.rect.w;
	// 		hud->data[i].sdl_data.rect.y = hud->data[hud->selected+hud->select_start_offset].sdl_data.rect.y;
	// 	}
	// 	hud->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
	// 	SDL_FreeSurface(surface);
	// 	TTF_CloseFont(font);
	// }
}

void hud_update_textures(world_t *world, player_t *player, hud_t *hud, SDL_Context *ctx) {
	if(!hud->needs_redraw) return;
	hud->needs_redraw = false;
	for(int i = 0; i < hud->data_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/Instruction.ttf", hud->data[i].data_type.dynamic_data.font_size);
		if(!font) DEBUG_LOG("open font error: %s", TTF_GetError());
		SDL_Color white = {255, 255, 255, 255};

		hud->data[i].sdl_data.rect.y = (25*i);
		size_t size = sizeof(hud->data[i].data_type.dynamic_data.str);
		char *str = hud->data[i].data_type.dynamic_data.str;
		char *cmp = my_strdup(hud->data[i].data_type.dynamic_data.str);
		room_t *room = world->room[player->global_x][player->global_y];
		bool changed = false;
		switch(i) {
			case 0: {
				const char *player_class = class_get_name(player->player_class);
				snprintf(str, size, "Class: %s", player_class);
				break;
			}
			case 1:
				snprintf(
					str, size,
					"Health %d / %d | Mana: %d / %d | Level: %d | XP: %d / %d",
					player->health, player->max_health,
					player->mana, player->max_mana,
					player->level, player->xp,
					xp_to_level_up(player->level));
				break;
			case 2:
				DEBUG_LOG("speed = %f", player->speed);
				snprintf(
					str, size,
					"Str: %d | Dex: %d | Int: %d | Const: %d | Spd: %d",
					(int)player->strength, (int)player->dexterity,
					(int)player->intelligence, (int)player->constitution,
					(int)player->speed);
				break;
			case 3:
				if(player->equipment.attack_weapon >= 0) {
					snprintf(
						str, size,
						"Attack Weapon: %s | Oil: %d",
						player->inventory[player->equipment.attack_weapon].name,
						player->oil);
				} else {
					snprintf(
						str, size,
						"Attack Weapon: Unarmed | Oil: %d",
						player->oil);
				}
				break;
			case 4:
				memset(str, 0, size);

				int max_status_effects = 4;
				strcat(str, "Status: ");
				int status_count = 0;
				for(int j = 0; j < world->buff_count; j++) {
					if(status_count == max_status_effects) break;
					if(world->buffs[j].target_type_id == TARGET_PLAYER) {
						if(status_count != 0) {
							strcat(str, ", ");
						}
						strcat(str, world->buffs[j].name);
						strcat(str, ":");
						char turns_left[8];
						snprintf(
							turns_left, sizeof(turns_left),
							"%d", world->buffs[j].turns_left);
						strcat(str, turns_left);
						status_count++;
					}
				}
				break;
			case 5:
				snprintf(
					str, size,
					"Room Location: (%d, %d)",
					player->global_x, player->global_y);
				break;
			case 6: {
				// if(*actor == PLAYER_TURN_ORDER_INDEX) {
				// 	pos += snprintf(buf + pos, sizeof(buf)-pos, "[%c] ", 'P');
				// }
				int pos = snprintf(str, size, "Turn Order: ");
				for(int j = 0; j < world->turn_order_size; j++) {
					if(world->turn_order[j] == PLAYER_TURN_ORDER_INDEX) {
						pos += snprintf(str + pos, size-pos, "[%c] ", 'P');
						continue;
					}
					if(world->turn_order[j] == WORLD_TURN_ORDER_INDEX) {
						pos += snprintf(str + pos, size-pos, "[%c] ", 'W');
						continue;
					}
					enemy_t *enemy = room->enemies[world->turn_order[j]];
					if(enemy != NULL && enemy->type != ENEMY_NONE) {
						if(room->tiles[enemy->y][enemy->x]->has_light) {
							pos += snprintf(
									str + pos, size-pos,
									"[%c] ", enemy->symbol);
						} else {
							pos += snprintf(str + pos, size-pos, "[?] ");
						}
					}
				}
				break;
			}
			default: {
				int enemy_index = i - 7;
				if(enemy_index >= room->current_enemy_count) {
					strcpy(str, " ");
					break;
				}
				int detect_radius = MAX(2, player->lantern.power);

				if(!room->enemies[enemy_index]) {
					strcpy(str, " ");
					break;
				}
				if(room->enemies[enemy_index]->x > player->x-detect_radius &&
					room->enemies[enemy_index]->x < player->x+detect_radius &&
					room->enemies[enemy_index]->y > player->y-(detect_radius) &&
					room->enemies[enemy_index]->y < player->y+(detect_radius)) {

					snprintf(
						str, size,
						"%s : %d",
						room->enemies[enemy_index]->name,
						room->enemies[enemy_index]->health);
					break;
				} else {
					strcpy(str, " ");
					break;
				}
			}
		}
		if(strcmp(str, cmp) != 0) {
			changed = true;
		}
		free(cmp);
		if(changed) {
			SDL_DestroyTexture(hud->data[i].sdl_data.texture);
			SDL_Surface *surface = TTF_RenderText_Solid(font, hud->data[i].data_type.dynamic_data.str, white);
			if(!surface) DEBUG_LOG("%s %s, %d, %d", "surface error: ", TTF_GetError(), i, room->current_enemy_count);
			hud->data[i].sdl_data.rect.w = surface->w;
			hud->data[i].sdl_data.rect.h = surface->h;
			hud->data[i].sdl_data.texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
			SDL_FreeSurface(surface);
		}
		TTF_CloseFont(font);
	}
}


/*
void load_game(world_t *world, player_t *player, menu_t *menu) {
	//TODO
}*/

void menu_use_item(void *ctx1, void *ctx2, void *ctx3) {
	menu_t *menu = ctx1;
	player_t *player = ctx2;
	use_item(menu, player);
}

void menu_loot_item(void *ctx1, void *ctx2, void *ctx3) {
	menu_t *menu = ctx1;
	world_t *world = ctx2;
	player_t *player = ctx3;
	player_take_loot_item(world->room[player->global_x][player->global_y], player, menu);
}

void confirm_class(void *ctx1, void *ctx2, void *ctx3) {
	menu_t *menu = ctx1;
	player_t *player = ctx2;
	player->player_class = class_get_type(menu->data[menu->selected].data_type.const_data.str);
	go_to_dest(ctx1, ctx2, ctx3);
}

void go_to_dest(void *ctx1, void *ctx2, void *ctx3) {
	menu_t *menu = ctx1;
	player_t *player = ctx2;
	player->state = menu->dests[menu->selected];
	// player->menu_manager.current_menu = menu->dests[menu->selected];
}

void menu_cursor_up(menu_t *menu) {
	if(menu->selected == 0) {
		if(menu->offset > 0) {
			menu->offset--;
		}
	} else if(menu->selected > 0) {
		menu->selected--;
	}
}

void menu_cursor_down(menu_t *menu) {
	if(menu->selected+1 >= menu->selectable_size) {
		if(menu->offset + 1 <= menu->offset_max) {
			menu->offset++;
		}
	} else if(menu->selected+1 < menu->selectable_size) {
		menu->selected++;
	}
}

void menu_update_cursor_pos(menu_t *menu) {
	int i = menu->menu_item_size-1;
	if(i < 0) return;
	menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
	menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
}

void menu_render(const menu_t *menu, SDL_Renderer *renderer) {
	for(int i = 0; i < menu->menu_item_size; i++) {
		SDL_RenderCopy(renderer, menu->data[i].sdl_data.texture, NULL, &menu->data[i].sdl_data.rect);
	}
}

void ui_render(ui_data_t *data, int16_t data_size, SDL_Renderer *renderer) {
	for(int i = 0; i < data_size; i++) {
		SDL_RenderCopy(renderer, data[i].sdl_data.texture, NULL, &data[i].sdl_data.rect);
	}
}
