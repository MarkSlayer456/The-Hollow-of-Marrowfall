#include "renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>

void main_menu_init(menu_t *menu, SDL_Context *ctx) {
	static menu_data_item_t main_menu_items[] = {
		{ .sdl_data = { NULL, { .x = 50, .y = 50, .w = 250, .h = 50} }, .str = GAME_TITLE, .font_size = 18},
		{ .sdl_data = { NULL, { .x = 50, .y = 100, .w = 250, .h = 50} }, .str = MENU_DIVIDE_LINE, .font_size = 18},
		{ .sdl_data = { NULL, { .x = 50, .y = 150, .w = 250, .h = 50} }, .str = "New Game", .font_size = 18},
		{ .sdl_data = { NULL, { .x = 50, .y = 200, .w = 250, .h = 50} }, .str = "Load Game", .font_size = 18},
		{ .sdl_data = { NULL, { .x = 50, .y = 250, .w = 250, .h = 50} }, .str = "Log Book", .font_size = 18},
		{ .sdl_data = { NULL, { .x = 0, .y = 0, .w = 0, .h = 0} }, .str = "--> ", .font_size = 18},
	};

	static enum menu dests[] = {
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

	for(int i = 0; i < menu->menu_item_size; i++) {
		TTF_Font *font = TTF_OpenFont("./data/font.ttf", menu->data[i].font_size);
		if(!font) DEBUG_LOG("%s", TTF_GetError());
		SDL_Color white = {255, 255, 255, 255};

		SDL_Surface *surface = TTF_RenderText_Solid(font, menu->data[i].str, white);
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

void menu_handle_input(menu_t menu[MENU_COUNT], enum menu menu_id, SDL_Event event) {
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
					if(menu[menu_id].selected - 1 < 0) {
						menu[menu_id].selected = menu[menu_id].selectable_size-1;
					} else {
						menu[menu_id].selected--;
					}
					break;
				case SDLK_DOWN:
					if(menu[menu_id].selected < menu[menu_id].selectable_size-1) {
						menu[menu_id].selected++;
					} else {
						menu[menu_id].selected = 0;
					}
					break;
				case SDLK_RETURN:
					switch(menu[menu_id].dests[menu[menu_id].selected]) {
						case NULL_MENU:
							break;
						case MAIN_MENU:
							break;
						case LOAD_MENU:
							break;
						case SAVE_MENU:
							break;
						case LOG_BOOK_MENU:
							break;
						case CLASS_MENU:
							break;
						case GAME:
							break;
						case MENU_COUNT:
							break;
					}
					break;
				default:
					break;
			}
			break;
	}

}

void menu_update_cursor_pos(const menu_t *menu) {
	for(int i = 0; i < menu->menu_item_size; i++) {
		if(i+1 == menu->menu_item_size) {
			menu->data[i].sdl_data.rect.x = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.x - menu->data[i].sdl_data.rect.w;
			menu->data[i].sdl_data.rect.y = menu->data[menu->selected+menu->select_start_offset].sdl_data.rect.y;
		}
	}
}

void menu_render(const menu_t *menu, SDL_Renderer *renderer) {
	for(int i = 0; i < menu->menu_item_size; i++) {
		SDL_RenderCopy(renderer, menu->data[i].sdl_data.texture, NULL, &menu->data[i].sdl_data.rect);
	}
}
