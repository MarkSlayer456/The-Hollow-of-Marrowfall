#ifndef ENUMS_H_
#define ENUMS_H_

enum buff_type {
	BUFF_NULL = -1,
	BUFF_BLEED,
	BUFF_BURN,
	BUFF_FROST,
	BUFF_POISON,
	BUFF_BLACKENING,
	BUFF_REGEN
};

enum target_type {
	TARGET_PLAYER,
	TARGET_ENEMY
};

enum trap_id {
	NULL_TRAP = -1,
	TRAP_STALAGMITE,
	TRAP_HOLE,
	TRAP_MUD,
	TRAP_TWISTED_ROOT
};

enum grade {
	Z = -1,
	A,
	B,
	C,
	D,
	F
};

enum menu_id {
	NULL_MENU = -1,
	MAIN_MENU,
	LOAD_MENU,
	SAVE_MENU,
	LOG_BOOK_MENU,
	CLASS_MENU,
	GAME,
	INVENTORY_MENU,
	LOOT_MENU,
	SPELL_MENU,
	MENU_COUNT
};

typedef enum biome {
	BIOME_NULL = -1,
	CAVE,
	BOG,
	CATACOMBS,
	ANCIENT_CITY,
	ARCANE_LABYRINTH,
	VOID_HOLLOW
} biome_t;

typedef enum stats {
	NULL_STAT,
	STRENGTH,
	DEXTERITY,
	INTELLIGENCE,
	CONSTITUTION,
	SPEED
} stats_t;

typedef enum direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
} direction_t;

typedef enum {
	PLAYER_STATE_NULL = -1,
	// not in a menu
	PLAYER_STATE_MOVING,
	PLAYER_STATE_ATTACKING,
	PLAYER_STATE_VIEWING, // in view mode
	// in a menu
	PLAYER_STATE_INVENTORY,
	PLAYER_STATE_LOOTING,
	PLAYER_STATE_TALKING,
	PLAYER_STATE_MENU, // settings menu not an inventory
	PLAYER_STATE_EQUIPPING_SPELL,
	PLAYER_STATE_DEAD
} player_state_t;

typedef enum action_bar_selectors { 
	NOT_OPEN = -1, 
	INVENTORY = 0, 
	SPELLS = 1,
	LOOT = 2
} action_bar_selector_t;

typedef enum inventory_category {
	ITEM = 0,
	FOOD = 1,
	POTIONS = 2,
	SCROLLS = 3
} inv_cat_t;

typedef enum class_type {
	CLASS_NONE = -1,
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
	VOID_ASSASSIN,
	CLASS_COUNT
} class_type_t;

typedef enum trait {
	PASSIVE,
	AGGRESSIVE,
	DARK_CENTERED,
	LIGHT_CENTERED,
	SURVIVAL
} trait_t;

typedef enum enemy_type {
	ENEMY_NONE = -1,
	SLIME,
	BAT,
	SKELETON,
	RAT,
	DRAGON,
	BABY_DRAGON,
	GOBLIN,
	GHOST,
	LOOT_GOBLIN,
	MOSS_BEAST,
	JESTER,
	VOIDLING,
	MARROW_CRAWLER,
	VOID_MAW,
	MARROW_LEECH,
	MUD_CRAWLER,
	BOG_LURKER,
} enemy_type_t;

#endif
