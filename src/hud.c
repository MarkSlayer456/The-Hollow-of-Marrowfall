#include "hud.h"
#include "game_manager.h"
#include "map_manager.h"
#include "items/items.h"
#include "player.h"
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "functions.h"
// TODO leaving this here so I can reference for sdl version
// void hud_update_messages(world_t *world, player_t *player) {
// 	int start_y = PLAYER_STATS_HUD_SPACE+ENEMY_STATS_HUD_SPACE+MESSAGE_HUD_SPACE-1;
// 	wmove(hud, start_y, 0);
// 	int printed_messages = 0;
// 	for(int i = world->messages_size-1; i > -1; i--) {
// 		if(printed_messages == MESSAGE_HUD_SPACE) return;
// 		if(world->messages[i] != NULL) {
// 			int y = 0;
// 			int x = 0;
// 			getyx(hud, y, x);
// 			x++; // this is required for a compiler error x not used, despite it literally being used
// 			wmove(hud, y-1, 0);
// 			char *clear = calloc(HUD_LENGTH, sizeof(char));
// 			waddstr(hud, clear);
// 			wmove(hud, y-1, 0);
// 			waddstr(hud, world->messages[i]);
// 		} else {
// 			DEBUG_LOG("%s", "hud.c: hud_update_messages: world->messages[i] is null!");
// 		}
// 		printed_messages++;
// 	}
// }
