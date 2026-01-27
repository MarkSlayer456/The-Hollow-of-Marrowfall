#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "items/items.h"
#include "player.h"
#include "enemy.h"
#include "types.h"
#include "game_constants.h"
#include "game_manager.h"
#include "map_manager.h"
#include "functions.h"
#include <strings.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

type_map_t grade_map[] = {
    {GRADE_A_NAME, A},
    {GRADE_B_NAME, B},
    {GRADE_C_NAME, C},
    {GRADE_D_NAME, D},
    {GRADE_F_NAME, F},
};

const int grade_map_len = sizeof(grade_map) / sizeof(grade_map[0]);

type_map_t rarity_map[] = {
    {RARITY_COMMON_NAME, COMMON},
    {RARITY_UNCOMMOM_NAME, UNCOMMON},
    {RARITY_RARE_NAME, RARE},
    {RARITY_EPIC_NAME, EPIC},
    {RARITY_LEGENDARY_NAME, LEGENDARY},
};

const int rarity_map_len = sizeof(rarity_map) / sizeof(rarity_map[0]);

type_map_t item_biome_map[] = {
    {CAVE_NAME, CAVE},
    {BOG_NAME, BOG},
    {CATACOMBS_NAME, CATACOMBS},
    {ANCIENT_CITY_NAME, ANCIENT_CITY},
    {ARCANE_LABYRINTH_NAME, ARCANE_LABYRINTH},
    {VOID_HOLLOW_NAME, VOID_HOLLOW},
};

const int item_biome_map_len = sizeof(item_biome_map) / sizeof(item_biome_map[0]);

type_map_t stats_map[] = {
    {STRENGTH_NAME, STRENGTH},
    {DEXTERITY_NAME, DEXTERITY},
    {INTELLIGENCE_NAME, INTELLIGENCE},
    {CONSTITUTION_NAME, CONSTITUTION},
    {SPEED_NAME, SPEED},
};

const int stats_map_len = sizeof(stats_map) / sizeof(stats_map[0]);

type_map_t item_type_map[] = {
    {BLANK_NAME, BLANK},
    
    {TELEPORT_SCROLL_NAME, TELEPORT_SCROLL},
    
    {HEALTH_POTION_NAME, HEALTH_POTION},
    {APPLE_NAME, APPLE},
    {ROTTEN_APPLE_NAME, ROTTEN_APPLE},
    {CHICKEN_DINNER_NAME, CHICKEN_DINNER},
    {RAT_MEAT_NAME, RAT_MEAT},
    {DRAGONFIRE_JERKY_NAME, DRAGONFIRE_JERKY},
    {CELESTIAL_CORNBREAD_NAME, CELESTIAL_CORNBREAD},
    {NIGHTBLOOM_GRAPES_NAME, NIGHTBLOOM_GRAPES},
    {PIZZA_NAME, PIZZA},
    {BEAR_CHILI_NAME, BEAR_CHILI},

    {OIL_NAME, OIL},
    
    {BLACKSTONE_ARMOR_NAME, BLACKSTONE_ARMOR},
    {BRONZE_ARMOR_NAME, BRONZE_ARMOR},
    {IRON_ARMOR_NAME, IRON_ARMOR},
    {STEEL_ARMOR_NAME, STEEL_ARMOR},
    {MITHRIL_ARMOR_NAME, MITHRIL_ARMOR},
    {SOULCRUST_ARMOR_NAME, SOULCRUST_ARMOR},
    {TERRACITE_ARMOR_NAME, TERRACITE_ARMOR},
    {GRAVEMARROW_ARMOR_NAME, GRAVEMARROW_ARMOR},
    {VOIDMARROW_ARMOR_NAME, VOIDMARROW_ARMOR},
    {RAT_HIDE_ARMOR_NAME, RAT_HIDE_ARMOR},
    {SLIME_ARMOR_NAME, SLIME_ARMOR},
    {BOG_IRON_ARMOR_NAME, BOG_IRON_ARMOR},
    {BONE_ARMOR_NAME, BONE_ARMOR},
    {SUNSTEEL_ARMOR_NAME, SUNSTEEL_ARMOR},
    {FALSE_IRON_ARMOR_NAME, FALSE_IRON_ARMOR},
    {BARKMAIL_ARMOR_NAME, BARKMAIL_ARMOR},
    {DRAGON_BONE_ARMOR_NAME, DRAGON_BONE_ARMOR},
    {CLOAK_NAME, CLOAK},
    {LEATHER_CLOAK_NAME, LEATHER_CLOAK},
    {BOGSTICK_VEST_NAME, BOGSTICK_VEST},
    {VOIDLACE_CLOAK_NAME, VOIDLACE_CLOACK},
    
    {BLACK_STONE_SWORD_NAME, BLACK_STONE_SWORD},
    {BRONZE_SWORD_NAME, BRONZE_SWORD},
    {IRON_SWORD_NAME, IRON_SWORD},
    {STEEL_SWORD_NAME, STEEL_SWORD},
    {MITHRIL_SWORD_NAME, MITHRIL_SWORD},
    {SOULCRUST_SWORD_NAME, SOULCRUST_SWORD},
    {TERRACTIE_SWORD_NAME, TERRACTIE_SWORD},
    {GRAVEMARROW_SWORD_NAME, GRAVEMARROW_SWORD},
    {VOIDMARROW_SWORD_NAME, VOIDMARROW_SWORD},
    
    {BLACK_STONE_SPEAR_NAME, BLACK_STONE_SPEAR},
    {BRONZE_SPEAR_NAME, BRONZE_SPEAR},
    {IRON_SPEAR_NAME, IRON_SPEAR},
    {STEEL_SPEAR_NAME, STEEL_SPEAR},
    {MITHRIL_SPEAR_NAME, MITHRIL_SPEAR},
    {SOULCRUST_SPEAR_NAME, SOULCRUST_SPEAR},
    {TERRACTIE_SPEAR_NAME, TERRACTIE_SPEAR},
    {GRAVEMARROW_SPEAR_NAME, GRAVEMARROW_SPEAR},
    {VOIDMARROW_SPEAR_NAME, VOIDMARROW_SPEAR},
    
    {BLACK_STONE_MACE_NAME, BLACK_STONE_MACE},
    {BRONZE_MACE_NAME, BRONZE_MACE},
    {IRON_MACE_NAME, IRON_MACE},
    {STEEL_MACE_NAME, STEEL_MACE},
    {MITHRIL_MACE_NAME, MITHRIL_MACE},
    {SOULCRUST_MACE_NAME, SOULCRUST_MACE},
    {TERRACTIE_MACE_NAME, TERRACTIE_MACE},
    {GRAVEMARROW_MACE_NAME, GRAVEMARROW_MACE},
    {VOIDMARROW_MACE_NAME, VOIDMARROW_MACE},
    
    {BLACK_STONE_AXE_NAME, BLACK_STONE_AXE},
    {BRONZE_AXE_NAME, BRONZE_AXE},
    {IRON_AXE_NAME, IRON_AXE},
    {STEEL_AXE_NAME, STEEL_AXE},
    {MITHRIL_AXE_NAME, MITHRIL_AXE},
    {SOULCRUST_AXE_NAME, SOULCRUST_AXE},
    {TERRACTIE_AXE_NAME, TERRACTIE_AXE},
    {GRAVEMARROW_AXE_NAME, GRAVEMARROW_AXE},
    {VOIDMARROW_AXE_NAME, VOIDMARROW_AXE},
    
    {BLACK_STONE_PICKAXE_NAME, BLACK_STONE_PICKAXE},
    {BRONZE_PICKAXE_NAME, BRONZE_PICKAXE},
    {IRON_PICKAXE_NAME, IRON_PICKAXE},
    {STEEL_PICKAXE_NAME, STEEL_PICKAXE},
    {MITHRIL_PICKAXE_NAME, MITHRIL_PICKAXE},
    {SOULCRUST_PICKAXE_NAME, SOULCRUST_PICKAXE},
    {TERRACTIE_PICKAXE_NAME, TERRACTIE_PICKAXE},
    {GRAVEMARROW_PICKAXE_NAME, GRAVEMARROW_PICKAXE},
    {VOIDMARROW_PICKAXE_NAME, VOIDMARROW_PICKAXE},
    
    {BLACK_STONE_BOW_NAME, BLACK_STONE_BOW},
    {BRONZE_BOW_NAME, BRONZE_BOW},
    {IRON_BOW_NAME, IRON_BOW},
    {STEEL_BOW_NAME, STEEL_BOW},
    {MITHRIL_BOW_NAME, MITHRIL_BOW},
    {SOULCRUST_BOW_NAME, SOULCRUST_BOW},
    {TERRACTIE_BOW_NAME, TERRACTIE_BOW},
    {GRAVEMARROW_BOW_NAME, GRAVEMARROW_BOW},
    {VOIDMARROW_BOW_NAME, VOIDMARROW_BOW},
    
    {BLACK_STONE_STAFF_NAME, BLACK_STONE_STAFF},
    {BRONZE_STAFF_NAME, BRONZE_STAFF},
    {IRON_STAFF_NAME, IRON_STAFF},
    {STEEL_STAFF_NAME, STEEL_STAFF},
    {MITHRIL_STAFF_NAME, MITHRIL_STAFF},
    {SOULCRUST_STAFF_NAME, SOULCRUST_STAFF},
    {TERRACTIE_STAFF_NAME, TERRACTIE_STAFF},
    {GRAVEMARROW_STAFF_NAME, GRAVEMARROW_STAFF},
    {VOIDMARROW_STAFF_NAME, VOIDMARROW_STAFF},

    {FIREBALL_SPELL_BOOK_NAME, FIREBALL_SPELL_BOOK},
    {ICE_SPELL_BOOK_NAME, ICE_SPELL_BOOK},
    {LIGHTNING_SPELL_BOOK_NAME, LIGHTNING_SPELL_BOOK},
    {HEALING_SPELL_BOOK_NAME, HEALING_SPELL_BOOK},
    {POISON_SPELL_BOOK_NAME, POISON_SPELL_BOOK},
    {VOID_SPELL_BOOK_NAME, VOID_SPELL_BOOK}
};

const int item_type_map_len = sizeof(item_type_map) / sizeof(item_type_map[0]);

item_ids_t item_get_id(const char *name) {
	for(int i = 0; i < item_type_map_len; i++) {
		if(strcasecmp(name, item_type_map[i].name) == 0) {
			return item_type_map[i].value;
		}
	}
	return BLANK;
}

enum grade get_grade(const char *name) {
    for(int i = 0; i < grade_map_len; i++) {
        if(strcasecmp(name, grade_map[i].name) == 0) {
            return grade_map[i].value;
        }
    }
    return Z;
}

stats_t get_stat(const char *name) {
    for(int i = 0; i < stats_map_len; i++) {
        DEBUG_LOG("%s, %s", name, stats_map[i].name);
        if(strcasecmp(name, stats_map[i].name) == 0) {
            return stats_map[i].value;
        }
    }
    return NULL_STAT;
}

rarity_t get_rarity(const char *name) {
    for(int i = 0; i < rarity_map_len; i++) {
        if(strcasecmp(name, rarity_map[i].name) == 0) {
            return rarity_map[i].value;
        }
    }
    return NULL_RARITY;
}

double get_percent_from_grade(enum grade g) {
    switch(g) {
        case A:
            return GRADE_A_PERCENT;
        case B:
            return GRADE_B_PERCENT;
        case C:
            return GRADE_C_PERCENT;
        case D:
            return GRADE_D_PERCENT;
        case F:
            return GRADE_F_PERCENT;
        case Z:
            return GRADE_Z_PERCENT;
    }
    return GRADE_Z_PERCENT;
}

void item_mark_as_unchanged(item_t *item) {
    item->has_changed = false;
}

void item_mark_as_changed(item_t *item) {
    item->has_changed = true;
}

// returns 1 on success and 0 on fail
int use_item(menu_t *menu, player_t *player)
{
	int success = 0;
    int item_index = menu->selected+menu->offset;
    item_t item = player->inventory[item_index];
    item_mark_as_changed(&item);
	if(item.stack > 0) {
        if(item.value_type == VALUE_TYPE_ARMOR) {
            success = handle_armor_change(player, item_index);
        } else if(item.value_type == VALUE_TYPE_WEAPON) {
            success = handle_weapon_change(player, item_index);
        } else if(item.value_type == VALUE_TYPE_FOOD) {
            //TODO effects with durations
            player_increase_health(player, item.stat_type.food.heal_amount);
            player_increase_mana(player, item.stat_type.food.mana_heal_amount);
            remove_item(menu, player);
            success = 1;
        } else if(item.value_type == VALUE_TYPE_SPELL) {
            player->state = PLAYER_STATE_EQUIPPING_SPELL;
        } else {
            switch(item.id) {
                case BLANK:
                    success = 0;
                case TELEPORT_SCROLL:
                    use_teleport_scroll(player);
                    success = 1;
                default:
                    success = 0;
            }
            remove_item(menu, player);
        }
        if(success) {
            player_close_inventory(player);
        }
	}
	return success;
}

void equip_spell(player_t *player, int spell_slot, int inv_index) {
    if(spell_slot == 1) {
        handle_spell_one_change(player, inv_index);
    } else if(spell_slot == 2) {
        handle_spell_two_change(player, inv_index);
    } else if(spell_slot == 3) {
        handle_spell_three_change(player, inv_index);
    }
}

int handle_armor_change(player_t *player, int new_armor) {
    int *old_armor = &player->equipment.armor;
    if(*old_armor == new_armor) {
        for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
            sub_player_equipment_stats(player, player->inventory[*old_armor].stat_type.armor.modifier_stats[i].stat, player->inventory[*old_armor].stat_type.armor.modifier_stats[i].modifier);
        }
        player->inventory[*old_armor].stat_type.armor.equipped = false;
        player->equipment.armor = -1;
    } else {
        if(*old_armor >= 0) {
            for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
                sub_player_equipment_stats(player, player->inventory[*old_armor].stat_type.armor.modifier_stats[i].stat, player->inventory[*old_armor].stat_type.armor.modifier_stats[i].modifier);
            }
        }
        player->equipment.armor = new_armor;
        player->inventory[player->equipment.armor].stat_type.armor.equipped = true;
        for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
            add_player_equipment_stats(player, player->inventory[*old_armor].stat_type.armor.modifier_stats[i].stat, player->inventory[*old_armor].stat_type.armor.modifier_stats[i].modifier);
        }
    }
    return 1;
}

int handle_weapon_change(player_t *player, int new_weapon) {
    int *main_hand = &player->equipment.main_hand;
    int *off_hand = &player->equipment.off_hand;
    int *attack_weapon = &player->equipment.attack_weapon;
    if(player->inventory[player->inventory_manager.inv_selector].stat_type.weapon.main_hand == true) {
        if(*main_hand == new_weapon) {
            if(player->inventory[*main_hand].stat_type.weapon.two_handed == true) {
                player->inventory[*off_hand].stat_type.weapon.equipped = false;
                *off_hand = -1;
            }
            for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
                sub_player_equipment_stats(player, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].stat, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].modifier);
            }
            player->inventory[*main_hand].stat_type.weapon.equipped = false;
            *main_hand = -1;
        } else {
            if(player->inventory[player->inventory_manager.inv_selector].stat_type.weapon.two_handed == true) {
                if(*main_hand >= 0) {
                    for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
                        sub_player_equipment_stats(player, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].stat, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].modifier);
                    }
                }
                if(*main_hand >= 0) {
                    player->inventory[*main_hand].stat_type.weapon.equipped = false;
                }
                if(*off_hand >= 0) {
                    player->inventory[*off_hand].stat_type.weapon.equipped = false;
                }
                *main_hand = new_weapon;
                *attack_weapon = *main_hand;
                *off_hand = new_weapon;
                player->inventory[*main_hand].stat_type.weapon.equipped = true;
                player->inventory[*off_hand].stat_type.weapon.equipped = true;
                for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
                    add_player_equipment_stats(player, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].stat, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].modifier);
                }
            } else {
                if(*main_hand >= 0) {
                    if(player->inventory[*main_hand].stat_type.weapon.two_handed == true) {
                        player->inventory[*off_hand].stat_type.weapon.equipped = false;
                        *off_hand = -1;
                    }
                    for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
                        sub_player_equipment_stats(player, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].stat, player->inventory[*main_hand].stat_type.weapon.modifier_stats[i].modifier);
                    }
                    player->inventory[*main_hand].stat_type.weapon.equipped = false;
                }
                *main_hand = new_weapon;
                *attack_weapon = *main_hand;
                player->inventory[*main_hand].stat_type.weapon.equipped = true;
            }
        }
    } else if(*off_hand == new_weapon) {
        for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
            sub_player_equipment_stats(player, player->inventory[*off_hand].stat_type.weapon.modifier_stats[i].stat, player->inventory[*off_hand].stat_type.weapon.modifier_stats[i].modifier);
        }
        player->inventory[*off_hand].stat_type.weapon.equipped = false;
        *off_hand = -1;
    } else {
        if(*off_hand >= 0) {
            for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
                sub_player_equipment_stats(player,player->inventory[*off_hand].stat_type.weapon.modifier_stats[i].stat, player->inventory[*off_hand].stat_type.weapon.modifier_stats[i].modifier);
            }
            player->inventory[*off_hand].stat_type.weapon.equipped = false;
        }
        *off_hand = new_weapon;
        player->inventory[*off_hand].stat_type.weapon.equipped = true;
        for(int i = 0; i < MAX_ARMOR_MODIFIERS; i++) {
            add_player_equipment_stats(player, player->inventory[*off_hand].stat_type.weapon.modifier_stats[i].stat, player->inventory[*off_hand].stat_type.weapon.modifier_stats[i].modifier);
        }
    }
    return 1;
}

int handle_spell_one_change(player_t *player, int new_spell1) {
    int *spell1 = &player->equipment.spell1;
    if(*spell1 == new_spell1) {
        player->inventory[*spell1].stat_type.spell.equipped = false;
        *spell1 = -1;
        player_cycle_attack_weapon(player);
        return 1;
    }

    if(*spell1 != new_spell1) {
        if(*spell1 >= 0) {
            player->inventory[*spell1].stat_type.spell.equipped = false;
        }
        *spell1 = new_spell1;
        player->inventory[*spell1].stat_type.spell.equipped = true;
        player->equipment.attack_weapon = *spell1;
        return 1;
    }
    return 1;
}

int handle_spell_two_change(player_t *player, int new_spell2) {
    int *spell2 = &player->equipment.spell2;
    if(*spell2 == new_spell2) {
        player->inventory[*spell2].stat_type.spell.equipped = false;
        *spell2 = -1;
        player_cycle_attack_weapon(player);
        return 1;
    }

    if(*spell2 != new_spell2) {
        if(*spell2 >= 0) {
            player->inventory[*spell2].stat_type.spell.equipped = false;
        }
        *spell2 = new_spell2;
        player->inventory[*spell2].stat_type.spell.equipped = true;
        player->equipment.attack_weapon = *spell2;
        return 1;
    }
    return 1;
}

int handle_spell_three_change(player_t *player, int new_spell3) {
    int *spell3 = &player->equipment.spell3;
    if(*spell3 == new_spell3) {
        player->inventory[*spell3].stat_type.spell.equipped = false;
        *spell3 = -1;
        player_cycle_attack_weapon(player);
        return 1;
    }

    if(*spell3 != new_spell3) {
        if(*spell3 >= 0) {
            player->inventory[*spell3].stat_type.spell.equipped = false;
        }
        *spell3 = new_spell3;
        player->inventory[*spell3].stat_type.spell.equipped = true;
        player->equipment.attack_weapon = *spell3;
        return 1;
    }
    return 1;
}

void add_player_equipment_stats(player_t *player, stats_t stat, int modifier) {
    switch(stat) {
        case STRENGTH:
            player->strength += modifier;
            break;
        case DEXTERITY:
            player->dexterity += modifier;
            break;
        case INTELLIGENCE:
            player->intelligence += modifier;
            break;
        case CONSTITUTION:
            player->constitution += modifier;
            break;
        case SPEED:
            player->speed += modifier;
            break;
        case NULL_STAT:
            break;
    }
}

void sub_player_equipment_stats(player_t *player, stats_t stat, int modifier) {
    switch(stat) {
        case STRENGTH:
            player->strength -= modifier;
            break;
        case DEXTERITY:
            player->dexterity -= modifier;
            break;
        case INTELLIGENCE:
            player->intelligence -= modifier;
            break;
        case CONSTITUTION:
            player->constitution -= modifier;
            break;
        case SPEED:
            player->speed -= modifier;
            break;
        case NULL_STAT:
            break;
    }
}

int use_teleport_scroll(player_t *player)
{
	player->x = TELEPORT_SCROLL_TELEPORT_LOCATION.x;
	player->y = TELEPORT_SCROLL_TELEPORT_LOCATION.x;
	player->global_x = TELEPORT_SCROLL_TELEPORT_LOCATION.global_x;
	player->global_y = TELEPORT_SCROLL_TELEPORT_LOCATION.global_y;
	return 0;
}

void remove_item(menu_t *menu, player_t *player)
{
    int item_index = menu->selected+menu->offset;
	player->inventory[item_index].stack--;
	if(player->inventory[item_index].stack <= 0) {
		player_organize_inv(player, menu, item_index);
	}
}
void item_spawn(item_ids_t id, room_t *room, tile_t *tile, item_data_t *item_data) {
    item_t *item = tile->items[tile->item_count];
    for(int i = 0; i < MAX_ITEMS; i++) {
        if(!item) continue;
        if(item_data[i].id == BLANK) continue;
        if(item_data[i].id == id) {
            strcpy(item->name, item_data[i].name);
            strcpy(item->desc, item_data[i].desc);
            item->id = id;
            item->stack = 1;
            item->value_type = item_data[i].value_type;
            switch(item_data[i].value_type) {
                case VALUE_TYPE_NONE:
                    break;
                case VALUE_TYPE_ARMOR:
                    item->stat_type.armor = item_data[i].stat_type.armor;
                    break;
                case VALUE_TYPE_WEAPON:
                    item->stat_type.weapon = item_data[i].stat_type.weapon;
                    break;
                case VALUE_TYPE_FOOD:
                    item->stat_type.food = item_data[i].stat_type.food;
                    break;
                case VALUE_TYPE_SCROLL:
                    item->stat_type.scroll = item_data[i].stat_type.scroll;
                    break;
                case VALUE_TYPE_SPELL:
                    item->stat_type.spell = item_data[i].stat_type.spell;
                    break;
            }
            tile->item_count++;
            break;
        }
    }
}

rarity_t item_generate_rarity() {
    float random = (float) rand() / (float) RAND_MAX;
    if(random >= RARITY_COMMON_CHANCE) {
        return COMMON;
    } else if(random >= RARITY_UNCOMMON_CHANCE + RARITY_COMMON_CHANCE) {
        return UNCOMMON;
    } else if(random >= RARITY_RARE_CHANCE + RARITY_UNCOMMON_CHANCE + RARITY_COMMON_CHANCE) {
        return RARE;
    } else if(random >= RARITY_EPIC_CHANCE + RARITY_RARE_CHANCE + RARITY_UNCOMMON_CHANCE + RARITY_COMMON_CHANCE) {
        return EPIC;
    } else if(random >= RARITY_LEGENDARY_CHANCE + RARITY_EPIC_CHANCE + RARITY_RARE_CHANCE + RARITY_UNCOMMON_CHANCE + RARITY_COMMON_CHANCE) {
        return LEGENDARY;
    }
    return COMMON;
}

item_ids_t item_generate_type(unsigned int *seed, item_data_t *item_data, biome_t biome) {
    // TODO needs to check for rarity
    rarity_t rarity = item_generate_rarity();
    item_ids_t items[MAX_ITEMS];
    int items_size = 0;
    for(int i = 0; i < MAX_ITEMS; i++) {
        if(item_data[i].spawn_biomes[biome] && item_data[i].rarity[biome] == rarity) {
            items[items_size++] = item_data[i].id;
        }
    }
    assert(items_size > 0);
    int random_number = (rand_r_portable(seed) % items_size);
    return items[random_number];
}

int tile_contains_item(tile_t *tile, item_ids_t item_id) {
    for(int i = 0; i < tile->item_count; i++) {
        if(tile->items[i]->id == item_id) {
            return i;
        }
    }
    return -1;
}

void drop_item(tile_t *tile, item_data_t *item_data, item_ids_t item_id, int quantity) {
    if(tile->item_count >= MAX_ITEMS_PER_TILE) {
        // TODO move to adjacent tile or something
        return;
    }
    int pos = tile_contains_item(tile, item_id);
    if(pos != -1) {
        tile->items[pos]->stack += quantity;
        return;
    }
    item_t *item = tile->items[tile->item_count];
    if(!item) {
        return;
    }
    tile->item_count++;
    for(int i = 0; i < MAX_ITEMS; i++) {
        if(item_data[i].id == BLANK) continue;
        if(item_data[i].id == item_id) {
            strcpy(item->name, item_data[i].name);
            strcpy(item->desc, item_data[i].desc);
            item->id = item_id;
            item->stack = quantity;
            item->value_type = item_data[i].value_type;
            switch(item_data[i].value_type) {
                case VALUE_TYPE_NONE:
                    break;
                case VALUE_TYPE_ARMOR:
                    item->stat_type.armor = item_data[i].stat_type.armor;
                    break;
                case VALUE_TYPE_WEAPON:
                    item->stat_type.weapon = item_data[i].stat_type.weapon;
                    break;
                case VALUE_TYPE_FOOD:
                    item->stat_type.food = item_data[i].stat_type.food;
                    break;
                case VALUE_TYPE_SCROLL:
                    item->stat_type.scroll = item_data[i].stat_type.scroll;
                    break;
                case VALUE_TYPE_SPELL:
                    item->stat_type.spell = item_data[i].stat_type.spell;
                    break;
            }
            break; 
        }
    }
}
