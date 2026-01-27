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
	static menu_data_item_t main_menu_items[] = {
		{ .sdl_data = { NULL, { .x = 50, .y = 50, .w = 250, .h = 50} }, .data_type.const_data = { .str = GAME_TITLE, .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 100, .w = 250, .h = 50} }, .data_type.const_data = { .str = MENU_DIVIDE_LINE, .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 150, .w = 250, .h = 50} }, .data_type.const_data = { .str = "New Game", .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 200, .w = 250, .h = 50} }, .data_type.const_data = { .str = "Load Game", .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 50, .y = 250, .w = 250, .h = 50} }, .data_type.const_data = { .str = "Log Book", .font_size = 18}},
		{ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.const_data = { .str = "--> ", .font_size = 18}},
	};

	static enum menu_id dests[] = {
		CLASS_MENU,
		LOAD_MENU,
		LOG_BOOK_MENU
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
	static menu_data_item_t class_menu_items[CLASS_COUNT+1];
	static enum menu_id dests[CLASS_COUNT];
	for(int i = 0; i < CLASS_COUNT; i++) {
		class_menu_items[i] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 25*(i+1), .w = 250, .h = 50} }, .data_type.const_data = { .str = class_get_name(i), .font_size = 18}};
		dests[i] = GAME;
	}
	class_menu_items[CLASS_COUNT] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.const_data = { .str = "--> ", .font_size = 18}};


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

	menu_data_item_t *load_menu_items = calloc(filenames_count+1, sizeof(menu_data_item_t));
	enum menu_id *dests = calloc(filenames_count, sizeof(enum menu_id));
	for(int i = 0; i < filenames_count; i++) {
		load_menu_items[i] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 25*(i+1), .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 18}};
		strcpy(load_menu_items[i].data_type.dynamic_data.str, filenames[i]);
		dests[i] = GAME;
	}

	load_menu_items[filenames_count] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};

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
	menu_data_item_t *inventory_menu_items = calloc(size, sizeof(menu_data_item_t));
	enum menu_id *dests = calloc(size, sizeof(enum menu_id));

	inventory_menu_items[0] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 25, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(inventory_menu_items[0].data_type.dynamic_data.str, "Inventory");
	inventory_menu_items[1] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 50, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(inventory_menu_items[1].data_type.dynamic_data.str, MENU_DIVIDE_LINE);

	for(int i = 2; i < lines_for_desc+2; i++) {
		inventory_menu_items[i] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 25*(i+12), .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
	}

	int inv_index = 0;
	for(int i = 12; i < size-1; i++) {
		inventory_menu_items[i] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 25*((i-10)+1), .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 14}};
		strcpy(inventory_menu_items[i].data_type.dynamic_data.str, player->inventory[inv_index].name);
		dests[i] = GAME; //TODO.. this probably won't be used anyway
		inv_index++;
	}

	inventory_menu_items[size-1] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
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

void loot_inventory_menu_init(world_t *world, player_t *player, menu_t *menu, SDL_Context *ctx, SDL_Color color) {
	const int size = 10+13;
	const int lines_for_desc = 10;
	menu_data_item_t *loot_inventory_menu_items = calloc(size, sizeof(menu_data_item_t));
	enum menu_id *dests = calloc(size, sizeof(enum menu_id));

	loot_inventory_menu_items[0] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 462, .y = 25, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(loot_inventory_menu_items[0].data_type.dynamic_data.str, "Loot Menu");
	loot_inventory_menu_items[1] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 462, .y = 50, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(loot_inventory_menu_items[1].data_type.dynamic_data.str, MENU_DIVIDE_LINE);

	for(int i = 2; i < lines_for_desc+2; i++) {
		loot_inventory_menu_items[i] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 462, .y = 25*(i+12), .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
	}
	int loot_index = 0;
	for(int i = 12; i < size-1; i++) {
		loot_inventory_menu_items[i] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 462, .y = 25*((i-10)+1), .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 14}};
		dests[i] = GAME; //TODO.. this probably won't be used anyway
		loot_index++;
	}

	loot_inventory_menu_items[size-1] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
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

void spell_menu_init(player_t *player, menu_t *menu, SDL_Context *ctx) {
	const int size = 6;
	menu_data_item_t *spell_menu_items = calloc(size, sizeof(menu_data_item_t));
	enum menu_id *dests = calloc(size, sizeof(enum menu_id));

	spell_menu_items[0] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 25, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(spell_menu_items[0].data_type.dynamic_data.str, "Which Spell Slot?");
	spell_menu_items[1] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 50, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 18}};
	strcpy(spell_menu_items[1].data_type.dynamic_data.str, MENU_DIVIDE_LINE);

	for(int i = 2; i < size-1; i++) {
		spell_menu_items[i] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 50, .y = 25*i+1, .w = 250, .h = 50} }, .data_type.dynamic_data = { .font_size = 14}};
		strcpy(spell_menu_items[i].data_type.dynamic_data.str, " ");
		dests[i] = GAME; //TODO.. this probably won't be used anyway
	}

	spell_menu_items[size-1] = (menu_data_item_t){ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .data_type.dynamic_data = { .font_size = 14}};
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
	player->menu_manager.current_menu = menu->dests[menu->selected];
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

void menu_handle_input(player_t *player, menu_t menu[MENU_COUNT], enum menu_id menu_id, SDL_Event event) {
	switch(event.type) {
		case SDL_QUIT:
			// TODO
			// end_game(world, player);
			// running = 0;
			break;
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym) {
				case SDLK_LEFT:
					break;
				case SDLK_RIGHT:
					break;
				case SDLK_UP:
					menu_cursor_up(&menu[menu_id]);
					break;
				case SDLK_DOWN:
					menu_cursor_down(&menu[menu_id]);
					break;
				case SDLK_RETURN:
					switch(player->menu_manager.current_menu) {
						case NULL_MENU:
							break;
						case MAIN_MENU:
							menu[menu_id].operation(menu[menu_id].operation_ctx1, menu[menu_id].operation_ctx2, menu[menu_id].operation_ctx3);
							break;
						case LOAD_MENU:
							menu[menu_id].operation(menu[menu_id].operation_ctx1, menu[menu_id].operation_ctx2, menu[menu_id].operation_ctx3);
							break;
						case SAVE_MENU:
							break;
						case LOG_BOOK_MENU:
							break;
						case CLASS_MENU:
							menu[menu_id].operation(menu[menu_id].operation_ctx1, menu[menu_id].operation_ctx2, menu[menu_id].operation_ctx3);
							break;
						case GAME:
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
			break;
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
