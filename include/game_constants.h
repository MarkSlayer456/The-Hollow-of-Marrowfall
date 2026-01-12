#ifndef GAME_CONSTANTS_H_

#define GAME_CONSTANTS_H_
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MESSAGE_IS_PLAYERS_TURN					"It is your turn!"

#define MAX_TYPE_MAP_NAME_LENGTH				128

/// Symbols ///

#define WALL 									'#'
#define BOG_WALL								'%'
#define PLAYER									'@'

// can walk on //
#define ITEM_SYMBOL								'*'
#define STALAGMITE								'^'
#define TWISTED_ROOT							'"'
#define HOLE									'0'
#define MUD										'~'
#define DOOR									'/'
#define EMPTY									'.'
#define CHEST									'$'
#define POT										'&'

///////////////

#define GAME_TITLE								"The Hollow of Marrowfall"
#define MENU_DIVIDE_LINE						"------------------------"
#define LOAD_MENU_TITLE							"Save Files"

/// traps ///

#define TRAP_STALAGMITE_NAME					"Stalagmite"
#define TRAP_HOLE_NAME							"Hole"
#define TRAP_MUD_NAME							"Mud"
#define TRAP_TWISTED_ROOT_NAME					"Twisted Root"

/// end traps ///

#define MAX_ITEMS								512
#define MAX_TRAPS								32

#define DEFENSE_SCALING_CONSTANT				50 // high defense less powerful, lower more powerful

#define PLAYER_TURN_ORDER_INDEX					-1
#define INVALID_ACTOR_INDEX						999999999

#define DOOR_BLOCKED_MESSAGE					"This door won't seem to budge!"
#define ATTACK_DIRECTION_MESSAGE				"Which direction?"

#define SAVE_MENU_PROMPT						"Name your save file (press enter to confirm or esc to cancel):"

#define ARROW_KEY_MOD							1000
#define DOWN_ARROW								ARROW_KEY_MOD + 66
#define UP_ARROW								ARROW_KEY_MOD + 65
#define RIGHT_ARROW								ARROW_KEY_MOD + 67
#define LEFT_ARROW								ARROW_KEY_MOD + 68
#define KEY_I									105
#define ENTER_KEY								10
#define ESC_KEY									27

#define TEST_SEED								596231
#define SEED_PRIMER								19349663u
#define ENEMY_Y_PRIME							73856093u
#define ITEM_X_PRIME							10000019u
#define ITEM_Y_PRIME							10000079u

#define KEY_A      								97
#define KEY_B									98
#define KEY_S       							115
#define KEY_F_MINE									102

#define KEY_W									119
#define KEY_S									115
#define KEY_D									100
#define KEY_E									101
#define KEY_SPACE								94
#define KEY_Q									113
#define KEY_1									49
#define KEY_2									50
#define KEY_3									51

#define CTRL_Q									17
#define CTRL_S									19

//TODO add more ...

#define WALK_CHAR_LENGTH							16
#define WANT_WALK_CHAR_LENGTH						16

#define ENEMY_CHAR      'E' // used when finding enemies on the map

#define ACTION_BAR_SELECTOR_COUNT				2

#define TIME_TO_ACT								50


#define MAX_ITEM_NAME_LENGTH					32
#define MAX_ITEM_DESC_LEN						256
#define MAX_ITEM_STACK							99
#define DEFAULT_MAX_MESSAGE_STORAGE				4
#define MAX_MESSAGE_LENGTH						59
#define MAX_MESSAGE_LENGTH_WITHOUT_PREFIX		47

// these are how far down each biome goes
#define CAVE_DEPTH							10
#define BOG_DEPTH							20
#define CATACOMBS_DEPTH						30
#define ANCIENT_CITY_DEPTH					40
#define ARCANE_LABYRINTH_DEPTH				50
#define VOID_HOLLOW_DEPTH					60

#define CAVE_ROOM_COUNT						20
#define BOG_ROOM_COUNT						16
#define CATACOMBS_ROOM_COUNT				1
#define ANCIENT_CITY_ROOM_COUNT				1
#define ARCANE_LABYRINTH_ROOM_COUNT			1
#define VOID_HOLLOW_ROOM_COUNT				1
#define ROOM_WIDTH							20
#define ROOM_HEIGHT							20
#define MAX_ENEMIES_PER_LEVEL				12
#define MAX_TRAPS_PER_ROOM					8
#define MAX_POTS_PER_ROOM					32
#define NUMBER_OF_BIOMES					6

#define CAVE_NAME							"Cave"
#define BOG_NAME							"Bog"
#define CATACOMBS_NAME						"Catacombs"
#define ANCIENT_CITY_NAME					"Ancient City"
#define ARCANE_LABYRINTH_NAME				"Arcane Labyrinth"
#define VOID_HOLLOW_NAME					"Void Hollow"

#define POTENTIAL_ENEMY_SPAWN_CHANCE1		 1
#define POTENTIAL_ENEMY_SPAWN_CHANCE2		.8
#define POTENTIAL_ENEMY_SPAWN_CHANCE3 		.6
#define POTENTIAL_ENEMY_SPAWN_CHANCE4 		.4
#define POTENTIAL_ENEMY_SPAWN_CHANCE5 		.2

#define POTENTIAL_ENEMY_SPAWN_CHAR1			'A'
#define POTENTIAL_ENEMY_SPAWN_CHAR2			'B'
#define POTENTIAL_ENEMY_SPAWN_CHAR3			'C'
#define POTENTIAL_ENEMY_SPAWN_CHAR4			'D'
#define POTENTIAL_ENEMY_SPAWN_CHAR5			'F'
#define POTENTIAL_CHEST_SPAWN_CHAR			'$'
#define POTENTIAL_ITEM_SPAWN_CHAR			'*'
#define DOOR_SPAWN_CHAR						'/'
#define POT_SPAWN_CHAR						'&'

/// Enemy ///

#define MAX_ENEMIES_PER_TILE				8
#define MAX_ITEMS_PER_TILE					8
#define ROOM_FILE_NAME_MAX_SIZE				128
#define MAX_ITEMS_NEARBY_PLAYER				MAX_ITEMS_PER_TILE*9
#define MAX_ENEMIES							100

#define ENEMY_TYPE_COUNT					17
#define MAX_ENEMY_NAME_LENGTH				32
#define NULL_ENEMY_NAME						"NULL"
#define SLIME_ENEMY_NAME 					"Slime"
#define BAT_ENEMY_NAME						"Bat"
#define RAT_ENEMY_NAME						"Rat"
#define SKELETON_ENEMY_NAME					"Skeleton"
#define DRAGON_ENEMY_NAME					"Dragon"
#define BABY_DRAGON_ENEMY_NAME				"Baby Dragon"
#define GOLBIN_ENEMY_NAME					"Goblin"
#define GHOST_ENEMY_NAME					"Ghost"
#define LOOT_GOBLIN_ENEMY_NAME				"Loot Goblin"
#define MOSS_BEAST_ENEMY_NAME				"Moss Beast"
#define JESTER_ENEMY_NAME					"Jester"
#define VOIDLING_ENEMY_NAME					"Voidling"
#define MARROW_CRAWLER_ENEMY_NAME			"Marrow Crawler"
#define VOID_MAW_ENEMY_NAME					"Void Maw"
#define MARROW_LEECH_ENEMY_NAME				"Marrow Leech"
#define MUD_CRAWLER_ENEMY_NAME				"Mud Crawler"
#define BOG_LURKER_ENEMY_NAME				"Bog Lurker"

#define ENEMY_GROWTH_MODIFER							0.8
#define ENEMY_LIGHT_CONSTITUTION_BUFF					5
#define ENEMY_LIGHT_STRENGTH_BUFF						5
#define ENEMY_LIGHT_SPEED_BUFF							5
#define ENEMY_DARK_CONSTITUTION_BUFF					5
#define ENEMY_DARK_STRENGTH_BUFF						5
#define ENEMY_DARK_SPEED_BUFF							5


#define ENEMY_TRAIT_PASSIVE_NAME				"Passive"
#define ENEMY_TRAIT_AGGRESSIVE_NAME				"Aggressive"
#define ENEMY_TRAIT_LIGHT_CENTERED_NAME			"Light Centered"
#define ENEMY_TRAIT_DARK_CENTERED_NAME			"Dark Centered"
#define ENEMY_TRAIT_SURVIVAL_NAME				"Survival"

#define ENEMY_SURVIVAL_RUN_FACTOR				.2
#define ENEMY_SURVIVAL_RUN_MIN_RANGE			6

#define ENEMY_PATH_LIST_DEFAULT_SIZE			16

#define SPOT_FIND_RANGE							4

#define MAX_KNOCKBACK							10

///////////

/////////////// items //////////////////

#define INV_SIZE					32

#define BLANK_NAME							"Blank"
#define TELEPORT_SCROLL_NAME				"Teleport Scroll"
#define TELEPORT_SCROLL_TELEPORT_LOCATION 	 ((location_t){ .x = 1, .y = 1, .global_x = 0, .global_y = 0 })
#define HEALTH_POTION_NAME					"Health Potion"
#define	APPLE_NAME							"Apple"
#define CHICKEN_DINNER_NAME					"Chicken Dinner"
#define ROTTEN_APPLE_NAME 					"Rotten Apple"
#define RAT_MEAT_NAME						"Rat Meat"
#define DRAGONFIRE_JERKY_NAME				"Dragonfire Jerky"
#define CELESTIAL_CORNBREAD_NAME			"Celestial Cornbread"
#define NIGHTBLOOM_GRAPES_NAME				"Nightbloom Grapes"
#define PIZZA_NAME							"Pizza"
#define BEAR_CHILI_NAME						"Bear Chili"


#define OIL_NAME							"Oil"

#define PLAYER_MIN_VISION_RANGE				2

// heavy armor
#define BLACKSTONE_ARMOR_NAME				"Blackstone Armor"
#define BRONZE_ARMOR_NAME					"Bronze Armor"
#define IRON_ARMOR_NAME						"Iron Armor"
#define STEEL_ARMOR_NAME					"Steel Armor"
#define MITHRIL_ARMOR_NAME					"Mithril Armor"
#define SOULCRUST_ARMOR_NAME				"Soulcrust Armor"
#define TERRACITE_ARMOR_NAME				"Terracite Armor"
#define GRAVEMARROW_ARMOR_NAME				"Gravemarrow Armor"
#define VOIDMARROW_ARMOR_NAME				"Voidmarrow Armor"

// medium armor
#define RAT_HIDE_ARMOR_NAME					"Rat Hide Armor"
#define SLIME_ARMOR_NAME					"Slime Armor"
#define BOG_IRON_ARMOR_NAME					"Bog Iron Armor"
#define BONE_ARMOR_NAME						"Bone Armor"
#define SUNSTEEL_ARMOR_NAME					"Sunsteel Armor"
#define FALSE_IRON_ARMOR_NAME				"False Iron Armor"
#define BARKMAIL_ARMOR_NAME					"Barkmail Armor"
#define DRAGON_BONE_ARMOR_NAME				"Dragon Bone Armor"

// light armor
#define CLOAK_NAME							"Cloak"
#define LEATHER_CLOAK_NAME					"Leather Cloak"
#define BOGSTICK_VEST_NAME					"Bogstick Vest"
#define VOIDLACE_CLOAK_NAME					"Voidlace Cloak"

// spells
#define FIREBALL_SPELL_BOOK_NAME						"Fireball Spell Book"
#define ICE_SPELL_BOOK_NAME								"Ice Spell Book"
#define LIGHTNING_SPELL_BOOK_NAME						"Lightning Spell Book"
#define HEALING_SPELL_BOOK_NAME							"Healing Spell Book"
#define POISON_SPELL_BOOK_NAME							"Poison Spell Book"
#define VOID_SPELL_BOOK_NAME							"Void Spell Book"


// armor types
#define ARMOR_TYPE_LIGHT_NAME				"light"
#define ARMOR_TYPE_MEDIUM_NAME				"medium"
#define ARMOR_TYPE_HEAVY_NAME				"heavy"

#define MAX_ARMOR_MODIFIERS					4

#define STRENGTH_NAME						"Strength"
#define DEXTERITY_NAME						"Dexterity"
#define INTELLIGENCE_NAME					"Intelligence"
#define CONSTITUTION_NAME					"Constitution"
#define SPEED_NAME							"Speed"

// weapon scaling grades
#define GRADE_A_NAME						"A"
#define GRADE_B_NAME						"B"
#define GRADE_C_NAME						"C"
#define GRADE_D_NAME						"D"
#define GRADE_F_NAME						"F"
#define GRADE_A_PERCENT						.80
#define GRADE_B_PERCENT						.50
#define GRADE_C_PERCENT						.40
#define GRADE_D_PERCENT						.30
#define GRADE_F_PERCENT						.20
#define GRADE_Z_PERCENT						0.0

// rarities
#define RARITY_COMMON_NAME					"Common"
#define RARITY_UNCOMMOM_NAME				"Uncommon"
#define RARITY_RARE_NAME					"Rare"
#define RARITY_EPIC_NAME					"Epic"
#define RARITY_LEGENDARY_NAME				"Legendary"

#define RARITY_COMMON_CHANCE				.50
#define RARITY_UNCOMMON_CHANCE				.35
#define RARITY_RARE_CHANCE					.1
#define RARITY_EPIC_CHANCE					.045
#define RARITY_LEGENDARY_CHANCE				.005

// swords
#define BLACK_STONE_SWORD_NAME				"Black Stone Sword"
#define BRONZE_SWORD_NAME					"Bronze Sword"
#define IRON_SWORD_NAME						"Iron Sword"
#define STEEL_SWORD_NAME					"Steel Sword"
#define MITHRIL_SWORD_NAME					"Mithril Sword"
#define SOULCRUST_SWORD_NAME				"Soulcrust Sword"
#define TERRACTIE_SWORD_NAME				"Terracite Sword"
#define GRAVEMARROW_SWORD_NAME				"Gravemarrow Sword"
#define VOIDMARROW_SWORD_NAME				"Voidmarrow Sword"

#define BLACK_STONE_SPEAR_NAME				"Black Stone Spear"
#define BRONZE_SPEAR_NAME					"Bronze Spear"
#define IRON_SPEAR_NAME						"Iron Spear"
#define STEEL_SPEAR_NAME					"Steel Spear"
#define MITHRIL_SPEAR_NAME					"Mithril Spear"
#define SOULCRUST_SPEAR_NAME				"Soulcrust Spear"
#define TERRACTIE_SPEAR_NAME				"Terracite Spear"
#define GRAVEMARROW_SPEAR_NAME				"Gravemarrow Spear"
#define VOIDMARROW_SPEAR_NAME				"Voidmarrow Spear"

#define BLACK_STONE_MACE_NAME				"Black Stone Mace"
#define BRONZE_MACE_NAME					"Bronze Mace"
#define IRON_MACE_NAME						"Iron Mace"
#define STEEL_MACE_NAME						"Steel Mace"
#define MITHRIL_MACE_NAME					"Mithril Mace"
#define SOULCRUST_MACE_NAME					"Soulcrust Mace"
#define TERRACTIE_MACE_NAME					"Terracite Mace"
#define GRAVEMARROW_MACE_NAME				"Gravemarrow Mace"
#define VOIDMARROW_MACE_NAME				"Voidmarrow Mace"

#define BLACK_STONE_AXE_NAME				"Black Stone Axe"
#define BRONZE_AXE_NAME						"Bronze Axe"
#define IRON_AXE_NAME						"Iron Axe"
#define STEEL_AXE_NAME						"Steel Axe"
#define MITHRIL_AXE_NAME					"Mithril Axe"
#define SOULCRUST_AXE_NAME					"Soulcrust Axe"
#define TERRACTIE_AXE_NAME					"Terracite Axe"
#define GRAVEMARROW_AXE_NAME				"Gravemarrow Axe"
#define VOIDMARROW_AXE_NAME					"Voidmarrow Axe"

#define BLACK_STONE_PICKAXE_NAME			"Black Stone Pickaxe"	
#define BRONZE_PICKAXE_NAME					"Bronze Pickaxe"
#define IRON_PICKAXE_NAME					"Iron Pickaxe"
#define STEEL_PICKAXE_NAME					"Steel Pickaxe"
#define MITHRIL_PICKAXE_NAME				"Mithril Pickaxe"
#define SOULCRUST_PICKAXE_NAME				"Soulcrust Pickaxe"
#define TERRACTIE_PICKAXE_NAME				"Terracite Pickaxe"
#define GRAVEMARROW_PICKAXE_NAME			"Gravemarrow Pickaxe"	
#define VOIDMARROW_PICKAXE_NAME				"Voidmarrow Pickaxe"

#define BLACK_STONE_BOW_NAME				"Black Stone Bow"
#define BRONZE_BOW_NAME						"Bronze Bow"
#define IRON_BOW_NAME						"Iron Bow"
#define STEEL_BOW_NAME						"Steel Bow"
#define MITHRIL_BOW_NAME					"Mithril Bow"
#define SOULCRUST_BOW_NAME					"Soulcrust Bow"
#define TERRACTIE_BOW_NAME					"Terracite Bow"
#define GRAVEMARROW_BOW_NAME				"Gravemarrow Bow"
#define VOIDMARROW_BOW_NAME					"Voidmarrow Bow"

#define BLACK_STONE_STAFF_NAME				"Black Stone Staff"
#define BRONZE_STAFF_NAME					"Bronze Staff"
#define IRON_STAFF_NAME						"Iron Staff"
#define STEEL_STAFF_NAME					"Steel Staff"
#define MITHRIL_STAFF_NAME					"Mithril Staff"
#define SOULCRUST_STAFF_NAME				"Soulcrust Staff"
#define TERRACTIE_STAFF_NAME				"Terracite Staff"
#define GRAVEMARROW_STAFF_NAME				"Gravemarrow Staff"
#define VOIDMARROW_STAFF_NAME				"Voidmarrow Staff"


///////////// pot //////////////
#define POT_ITEM_SPAWN_CHANCE				.3

///////////// player //////////////

#define NULL_CLASS_NAME					"NULL"
#define SWORDSMAN_CLASS_NAME			"Swordsman"
#define BARBARIAN_CLASS_NAME			"Barbarian"
#define SPEARMAN_CLASS_NAME				"Spearman"
#define ARCHER_CLASS_NAME				"Archer"
#define WIZARD_CLASS_NAME				"Wizard"
#define MERCHANT_CLASS_NAME				"Merchant"
#define ROGUE_CLASS_NAME				"Rogue"
#define MONK_CLASS_NAME					"Monk"
#define PALADIN_CLASS_NAME				"Paladin"
#define BRAWLER_CLASS_NAME				"Brawler"
#define SAMURAI_CLASS_NAME				"Samurai"
#define VOID_EMBRACE_CLASS_NAME			"Void Embrace"
#define VOID_KNIGHT_CLASS_NAME			"Void Knight"
#define VOID_ASSASSIN_CLASS_NAME		"Void Asssassin"

#define BASE_STRENGTH					10
#define BASE_DEXTERITY					10
#define BASE_INTELLIGENCE				10
#define BASE_CONSTITUTION				10
#define BASE_SPEED						10
#define BASE_CRIT_CHANCE				.05

#define MAX_CLASSES						32

#define STARTING_OIL					100

#define MAX_SPELL_SLOTS					3
#define SPELL_EQUIP_MENU_WIDTH			31
#define SPELL_EQUIP_MENU_HEIGHT			7
#define DEATH_MENU_WIDTH			31
#define DEATH_MENU_HEIGHT			7

#define STARTING_BUFF_SIZE			   8
#define BUFF_NAME_MAX_LEN				4
#define BUFF_NULL_NAME					"NIL"
#define BUFF_BLEED_NAME					"BLD"
#define BUFF_BURN_NAME					"BRN"
#define BUFF_FROST_NAME					"ICE"
#define BUFF_POISON_NAME				"PSN"
#define BUFF_BLACKENING_NAME			"BLK"
#define BUFF_HEAL_NAME					"RGN"

#define MAX_MENUS						16

#define NOT_ENOUGH_MANA_MESSAGE				"You don't have enough mana to cast this spell!"

#define SAVE_FILE_MAX_LEN				64

#define HUD_WIDTH					59
#define HUD_HEIGHT					24

#define PLAYER_STATS_HUD_SPACE		7
#define ENEMY_STATS_HUD_SPACE		7
#define MESSAGE_HUD_SPACE			10
#define HUD_LENGTH					50

#define WORLD_HEIGHT				50
#define WORLD_WIDTH					50

#define SCREEN_WIDTH				80
#define SCREEN_HEIGHT				24

#define INVENTORY_HEIGHT			SCREEN_HEIGHT/2
#define INVENTORY_WIDTH				SCREEN_WIDTH

#define LOAD_MENU_VISIBLE_ENTRIES						SCREEN_HEIGHT-2

// Lantern
#define LANTERN_MAX_POWER								10
#define LANTERN_NORMAL_POWER							5
#define LANTERN_OVER_LOAD_POWER							10
#define LANTERN_OVER_LOAD_THRESHOLD						4
#define LANTERN_OVER_LOAD_LOWER_BOUNDS					6
#define LANTERN_TURNS_TO_DIM							20
#define LANTERN_OIL_FOR_NORMAL_POWER					1
#define LANTERN_OIL_FOR_OVER_LOAD_POWER 				10
#define LANTERN_CAN_AFFORD_REFUEL						"You don't have enough oil to light you lantern!"

#define ENTRANCE_MESSAGE_CHANCE							.1

#define CAVE_ENTRANCE1									"You hear the flickering sound of your lantern"
#define CAVE_ENTRANCE2									"The path ahead twists into darkness"
#define CAVE_ENTRANCE3									"A faint breeze smells of minerals and decay"
#define CAVE_ENTRANCE4									"Drips of water echo through the darkness ahead"

// menus
#define MENU_CLASS_PROMPT								"Please select a class!"

#endif
