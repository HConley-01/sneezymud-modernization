//////////////////////////////////////////////////////////////////////////
//
// SneezyMUD - All rights reserved, SneezyMUD Coding Team
//
//////////////////////////////////////////////////////////////////////////

#pragma once

// I would prefer to decouple these definitions and assign them in low.cc
// but we use these values at compile time sometimes (switch/case) so they
// have to be in here.

class Room {
  private:
    Room();

  public:
    static const int AUTO_RENT = -2; /* for auto-renting */
    static const int NOWHERE = -1;
    static const int VOID = 0;
    static const int IMPERIA = 1;
    static const int HELL = 70;
    static const int STORAGE = 71;
    static const int POLY_STORAGE = 72;
    static const int CORPSE_STORAGE = 73;
    static const int NOCTURNAL_STORAGE = 74;
    static const int Q_STORAGE = 76;
    static const int CS = 100;
    static const int GH_INN = 557;
    static const int MORGUE = 433;
    static const int GREEN_DRAGON_INN = 553;
    static const int KINDRED_INN = 556;
    static const int DONATION = 563;
    static const int PETS_GH = 566;
    static const int PEW1 = 757;
    static const int PEW2 = 758;
    static const int PEW3 = 761;
    static const int PEW4 = 763;
    static const int TOP_OF_TREE = 25487;
    static const int PETS_BM = 1397;
    static const int DUMP = 3700;
    static const int PETS_LOG = 3738;
    static const int TICKET_DESTINATION = 6969;
    static const int PETS_AMB = 7804;
    static const int POKER = 2358;
    static const int HILO = 2359;
    static const int BLACKJACK = 2360;
    static const int BACCARAT = 2361;
    static const int HOLDEM = 2367;
    static const int HEARTS = 8417;
    static const int WARD_1 = 13142;
    static const int WARD_2 = 9734;
    static const int TREE_BRIDGE = 15277;
    static const int SLEEPTAG_CONTROL = 23599;
    static const int GUILD_BUREAU = 395;
    static const int TROLLEY = 15344;
    static const int AERIE_INN = 44853;
    static const int TROLL_INN = 30935;
    static const int KALYSIA_INN = 14233;
    static const int BULLYWUG_INN = 45054;
    static const int NEWBIE = 584;
};

class Obj {
  private:
    Obj();

  public:
    static const int DEITY_TOKEN = 2;
    static const int GENERIC_L_BAG = 6;
    static const int BANDAGE = 9;
    static const int ROSEPETAL = 10;
    static const int GENERIC_TALEN = 13;
    static const int BATS_JACUZZI = 31;
    static const int GENERIC_TOOTH = 39;
    static const int GENERIC_COMMODITY = 50;
    static const int SALTPILE = 99;
    static const int GENERIC_FLARE = 199;
    static const int WEAPON_L_SWORD = 300;
    static const int WEAPON_S_SWORD = 304;
    static const int WEAPON_DAGGER = 305;
    static const int WEAPON_BOW = 307;
    static const int WEAPON_STAFF = 309;
    static const int WEAPON_CLUB = 310;
    static const int WEAPON_MACE = 311;
    static const int WEAPON_HAMMER = 312;
    static const int WEAPON_AXE = 314;
    static const int WEAPON_WHIP = 316;
    static const int WEAPON_WHIP_STUD = 317;
    static const int WEAPON_KNIFE = 318;
    static const int WEAPON_AVENGER1 = 319;
    static const int WEAPON_AVENGER2 = 320;
    static const int WEAPON_T_STAFF = 324;
    static const int WEAPON_T_DAGGER = 325;
    static const int WEAPON_T_SWORD = 329;
    static const int WEAPON_AVENGER3 = 326;
    static const int CARAVAN = 519;
    static const int TRADE_GOOD = 520;
    static const int GENERIC_MONEYPOUCH = 604;
    static const int GENERIC_TRASH_PILE = 648;
    static const int ST_FLINT = 900;
    static const int ST_SULPHUR = 901;
    static const int ST_BAG = 902;
    static const int ST_HYDROGEN = 903;
    static const int ST_POISON = 904;
    static const int ST_SHARDS = 905;
    static const int ST_NEEDLE = 906;
    static const int ST_NOZZLE = 907;
    static const int ST_FUNGUS = 908;
    static const int ST_SPRING = 909;
    static const int ST_BELLOWS = 910;
    static const int ST_GAS = 911;
    static const int ST_HOSE = 912;
    static const int ST_ACID_VIAL = 913;
    static const int ST_CON_POISON = 914;  // also used in poison weapon
    static const int ST_SPIKE = 915;
    static const int ST_TRIPWIRE = 916;
    static const int ST_RAZOR_BLADE = 917;
    static const int ST_CONCRETE = 918;
    static const int ST_WEDGE = 919;
    static const int ST_FROST = 920;
    static const int ST_PENTAGRAM = 921;
    static const int ST_BLINK = 922;
    static const int ST_ATHANOR = 923;
    static const int ST_LANDMINE = 924;
    static const int ST_GRENADE = 925;
    static const int ST_CASE_MINE = 926;
    static const int ST_CASE_GRENADE = 927;
    static const int ST_CGAS = 928;
    static const int ST_TUBING = 929;
    static const int ST_PEBBLES = 930;
    static const int ST_BOLTS = 931;
    static const int ST_CANISTER = 932;
    static const int ST_CRYSTALINE = 933;
    static const int ST_RAZOR_DISK = 934;
    static const int GENERIC_DAGGER = 1000;
    static const int GENERIC_NOTE = 3090;
    static const int GENERIC_PEN = 3091;
    static const int VENISON = 3114;
    static const int BROWN_MUSHROOM = 3401;
    static const int WEAPON_ARROW = 3409;
    static const int BOULDER_ITEM = 4181;
    static const int GENERIC_GRAVE = 4200;
    static const int GOLD_FEATHER = 4791;
    static const int PHOENIX_FEATHER = 4847;
    static const int ITEM_DAYGATE = 5890;
    static const int ITEM_MOONGATE = 5891;
    static const int MINELIFT_UP = 7085;
    static const int MINELIFT_DOWN = 7086;
    static const int TALISMAN = 9700;
    static const int PILE_OFFAL = 10030;  // horse poop, etc...
    static const int ITEM_RAINBOW_BRIDGE1 = 10041;
    static const int ITEM_RAINBOW_BRIDGE2 = 10042;
    static const int HERALD = 10543;
    static const int GENERIC_POOL = 10559;
    static const int GENERIC_GAS = 10560;
    static const int INERT_STICK = 10561;
    static const int MONK_QUEST_DOG_COLLAR = 12468;
    static const int FLAMING_PORTAL = 16157;
    static const int BM_TRASHCAN = 18583;
    static const int RIVER_SERP_VENOM = 20409;
    static const int STONE_LEG_CLUB = 20425;
    static const int ICE_SHARD_LARGE = 20437;
    static const int ICE_SHARD_SMALL = 20440;
    static const int ARCTIC_BEAR_HIDE = 20441;
    static const int SNOW_BEAST_HORN = 20442;
    static const int LARGE_ANIMAL_HIDE = 20443;
    static const int SEAL_SKIN_HIDE = 20450;
    static const int EARMUFF = 20456;
    static const int AQUA_DRAG_HEAD = 22522;
    static const int SLEEPTAG_STAFF = 23500;
    static const int RAINBOW_MIST = 27491;
    static const int QUEST_ORE = 29020;
    static const int RED_ORE = 29022;
    static const int GENERIC_SCROLL = 29990;
    static const int STATS_POTION = 29991;
    static const int YOUTH_POTION = 29992;
    static const int GENERIC_POTION = 29993;
    static const int LEARNING_POTION = 29997;
    static const int CRAPS_DICE = 29999;
    static const int MASK1 = 1415;
    static const int MASK2 = 1416;
    static const int MASK3 = 1417;
    static const int MASK4 = 1418;
    static const int MASK5 = 1419;
    static const int MASK6 = 1420;
    static const int GENERIC_STEAK = 31365;
    static const int GENERIC_PLANT = 15347;
    static const int ANGEL_HEART = 9634;
    static const int MYSTERY_POTION = 31300;
    static const int IMMORTAL_EXCHANGE_COIN = 37142;
    static const int PREEN_FEATHER = 44845;
    static const int GENERIC_L_TOKEN = 9995;
};

class Mob {
  private:
    Mob();

  public:
    static const int ANY = -1;
    static const int NONE = 0;
    static const int FIRE_ELEMENTAL = 16;
    static const int WATER_ELEMENTAL = 17;
    static const int EARTH_ELEMENTAL = 18;
    static const int AIR_ELEMENTAL = 19;
    static const int FACTION_FAERY = 20;
    static const int WOOD_GOLEM = 26;
    static const int ROCK_GOLEM = 27;
    static const int IRON_GOLEM = 28;
    static const int DIAMOND_GOLEM = 29;
    static const int ANIMATION = 41;
    static const int MALE_CHURCH_GOER = 50;
    static const int FEMALE_CHURCH_GOER = 51;
    static const int MALE_HOPPER = 52;
    static const int FEMALE_HOPPER = 53;
    static const int NEWSBOY = 84;
    static const int LAMPBOY = 99;
    static const int WATCHMAN = 100;
    static const int CITYGUARD = 101;
    static const int ELITE = 102;
    static const int SWEEPER = 103;
    static const int SMALL_CAT = 104;
    static const int CITIZEN_M = 108;
    static const int CITIZEN_F = 109;
    static const int PEASANT = 110;
    static const int OLD_SAGE = 113;
    static const int DWARF_ADV = 125;
    static const int GNOME_MISS = 126;
    static const int OGRE_VIG = 127;
    static const int HOBBIT_EMI = 128;
    static const int ELF_TRADER = 138;
    static const int DEITY_JUDGMENT = 146;
    static const int PAWNGUY = 159;
    static const int PETGUY_GH = 166;
    static const int PIGEON = 172;
    static const int SWEEPER2 = 193;
    static const int BOUNCER = 194;
    static const int MAGE_GM_LEVEL15 = 200;
    static const int MONK_GM_LEVEL15 = 207;
    static const int MAGE_GM_LEVEL40 = 216;
    static const int MONK_GM_LEVEL40 = 223;
    static const int FIRST_RANGER_BASIC_TRAINER = 261;
    static const int BOUNCER_HEAD = 265;
    static const int COMBAT_TRAINER = 286;
    static const int FIRST_ANIMAL_TRAINER = 308;
    static const int TALAR = 324;
    static const int KING_GH = 353;
    static const int QUEEN_GH = 354;
    static const int HUANG_LO = 385;
    static const int OLD_WOMAN = 404;
    static const int TROLL_GIANT = 405;
    static const int FISTLAND = 407;
    static const int TREE_SPIRIT = 408;
    static const int BALCOR = 528;
    static const int ROAD_THIEF = 650;
    static const int ROAD_ROBBER = 651;
    static const int ROAD_THIEF_LARGE = 700;
    static const int BEHOLDER = 904;
    static const int DOPPLEGANGER = 914;
    static const int BANSHEE = 928;
    static const int EAGLE = 953;
    static const int BULGE = 1024;
    static const int PETGUY_BM = 1209;
    static const int FONG_CHUN = 1304;
    static const int BISHOP_BMOON = 1361;
    static const int APOC_PESTILENCE = 1601;
    static const int APOC_WAR = 1602;
    static const int APOC_FAMINE = 1603;
    static const int APOC_DEATH = 1604;
    static const int APOC_PESTHORSE = 1605;
    static const int APOC_WARHORSE = 1606;
    static const int APOC_FAMINEHORSE = 1607;
    static const int APOC_DEATHHORSE = 1608;
    static const int APOC_WARRIOR = 1609;
    static const int CARAVAN_MASTER = 1610;
    static const int DWARVEN_HIGH_PRIEST = 2113;
    static const int TAILLE = 2273;
    static const int CREED = 2495;
    static const int SPARTAGUS = 2874;
    static const int BUCK_OLD = 3109;
    static const int BUCK_YOUNG = 3114;
    static const int GANGMEMBER_GIBBETT = 3210;
    static const int RANGER_LORD = 3415;
    static const int SULTRESS = 3459;
    static const int PETGUY_LOG = 3707;
    static const int ASSASSIN = 3857;
    static const int SHAMAN_BULLY = 4304;
    static const int CHIEF_AARAKOCRA = 4745;
    static const int ROC = 4791;
    static const int DRAGON_GOLD = 4796;
    static const int RALIKI = 4822;
    static const int PHOENIX = 4847;
    static const int DRAGON_BRONZE = 4858;
    static const int SNAKE = 5112;
    static const int RAT_KING = 5131;
    static const int HOBBIT_ADVENTURER = 5347;
    static const int OVERLORD = 5443;
    static const int MARCUS = 5555;
    static const int MISHRAK = 5567;
    static const int MALOVHOR = 5740;
    static const int GRUUM = 5570;
    static const int NITELLION = 5720;
    static const int LEPER = 6602;
    static const int BLACKWIDOW = 6605;
    static const int PRIEST_HOLY = 6611;
    static const int SCAR = 6761;
    static const int DRAGON_WORKER = 6844;
    static const int MOURNING_WIDOW = 7325;
    static const int GONCHAR = 7404;
    static const int CYRIN_FORGEHAMMER = 7429;
    static const int CYVIK = 7472;
    static const int SONGBIRD = 7500;
    static const int GHOST_BISHOP = 7510;
    static const int NESMUM = 7531;
    static const int BLINDMAN = 7800;
    static const int NIGHTMARE = 7803;
    static const int PEACOCK = 7806;
    static const int GREYHOUND = 7816;
    static const int CHAMELEON = 7818;
    static const int LOCUSTS25 = 7850;
    static const int LOCUSTS30 = 7851;
    static const int LOCUSTS35 = 7852;
    static const int LOCUSTS40 = 7853;
    static const int LOCUSTS50 = 7854;
    static const int SNAKES25 = 7855;
    static const int SNAKES30 = 7856;
    static const int SNAKES35 = 7857;
    static const int SNAKES40 = 7858;
    static const int SNAKES50 = 7859;
    static const int SIMON_SPELLCRAFTER = 7876;
    static const int BOUNCER2 = 8418;
    static const int MISER_BEN = 8435;
    static const int CHEETAH = 8511;
    static const int ELEPHANT = 8525;
    static const int MEDICINE_MAN = 8677;
    static const int PETGUY_AMB = 8701;
    static const int WARRIOR_AMBER = 8704;
    static const int DWARVEN_AMBASSADOR = 8784;
    static const int LENGE_MERCHANT = 8796;
    static const int KHELFENN_SMITH = 8859;
    static const int CROP_MANAGER = 8910;
    static const int YOLA = 9148;
    static const int RATTLESNAKE = 9314;
    static const int UNDEAD_CHIEF = 9319;
    static const int RAULDOPLIC = 9322;
    static const int ABNOR = 9326;
    static const int GRIZWALD = 9331;
    static const int PROP_MASTER = 9540;
    static const int MOAT_MONSTER = 9700;
    static const int CLERIC_VOLCANO = 9727;
    static const int ASH = 9954;
    static const int FREEZING_MIST = 10022;
    static const int GERSARD = 10119;
    static const int FALCON = 10913;
    static const int HERMIT_JED = 10919;
    static const int POACH_KOBOLD = 10922;
    static const int POACH_ORC = 10923;
    static const int GM_IRIS = 11001;
    static const int DAKINOR = 11005;
    static const int THALIA = 11006;
    static const int MAGE_GM_LEVEL50 = 11039;
    static const int CAT_SHADOW = 11105;
    static const int CAPTAIN_RYOKEN = 11113;
    static const int ORC_MAGI = 11300;
    static const int VAMP_BAT = 11340;
    static const int BELIMUS = 12402;
    static const int DRAGON_SILVER = 12404;
    static const int SIREN = 12406;
    static const int KRAKEN = 12407;
    static const int WHALE_SPERM = 12411;
    static const int TIGER_SHARK = 12413;
    static const int WHALE_BLUE = 12415;
    static const int EEL = 12416;
    static const int SHARK_YOUNG = 12431;
    static const int ASCALLION = 12433;
    static const int MERMAID = 12440;
    static const int MONK_GM_LEVEL50 = 12509;
    static const int EEL2 = 14125;
    static const int JAQUIN = 14159;
    static const int BARARAKNA = 14227;
    static const int WORKER_WINERY = 14404;
    static const int BERARDINIS = 15103;
    static constexpr int ABANOS = 15410;
    static const int POACHER = 15251;
    static const int MERRITT = 15284;
    static const int BONE_PYGMY = 15107;
    static const int MERIN_LIGHTTOES = 16102;
    static const int RASMIN_LIGHTTOES = 16109;
    static const int FRIGHTENED_PRISONER = 16453;
    static const int SILVERCLAW = 17106;
    static const int ROCK_GIANT = 20408;
    static const int RIVER_SERPENT = 20409;
    static const int LOST_ICE_COM = 20420;
    static const int LOST_ICE_SCOUT = 20421;
    static const int ARCTIC_BEAR1 = 20422;
    static const int ARCTIC_BEAR2 = 20423;
    static const int SNOW_BEAST = 20424;
    static const int PENGUIN_ADULT = 20425;
    static const int PENGUIN_YOUNG = 20426;
    static const int MOUNTAIN_CAT = 20427;
    static const int SEA_LION = 20430;
    static const int AQUATIC_DRAGON = 22517;
    static const int SPIRIT_OF_WARRIOR = 22518;
    static const int LOGRUS_INITIATION = 22713;
    static const int CLERIC_ARDEN = 23209;
    static const int DREAD_PIRATE = 22796;
    static const int BET_TAKER = 23615;
    static const int ANIMAL_KEEPER = 23619;
    static const int WEAPONS_GUARD = 23620;
    static const int NETHKER = 23626;
    static const int DRAGON_CLOUD = 23633;
    static const int GNOME_BAKER = 24417;
    static constexpr int BAKERS_WIFE = 24418;
    static const int LORTO = 28876;
    static const int HERMIT_GHPARK = 25404;
    static const int GNOBLE_FARMER = 22410;
    static const int GNOME_FARMER = 22414;
    static const int GNOME_CHILD = 22419;
    static const int GNOME_FARMHAND = 22421;
    static const int JOHN_RUSTLER = 22427;
    static const int CALDONIA = 24100;
    static const int SMALL_HOBBIT = 27160;
    static const int ELDERLY_SHAMAN = 30701;
    static const int CHIEF_MALE_VILLAGER = 30702;
    static const int ANCESTOR_SPIRIT_APPARATION = 30713;
    static const int THRALL_SPECTRE = 31300;
    static const int THRALL_GHAST = 31301;
    static const int THRALL_GHOUL = 31302;
    static const int THRALL_DEMON = 31303;
    static const int JUJU_SHAMAN_GM = 206;
    static const int TOTEM_TRADER = 31306;
    static const int FACTION_REGISTRAR = 360;
    static const int SAMEDI = 31310;
    static const int PTOK = 31311;
    static const int FORSAKEN = 31312;
    static const int ELRIC = 31313;
    static const int SPIRIT_FATHER = 31314;
    static const int GANDOLFO = 31315;
    static const int STRANGE_LIZARD = 31316;
    static const int CATRINA = 32758;
    static const int BUSH_COOK = 33698;
    static const int ENSLAVED_PALADIN = 33805;
    static const int GYPSY_ROMANTIC = 29405;
    static const int FLORIST_BUSY = 29406;
    static const int CLOWN_SCARY = 29407;
    static const int MIME_ANIMATED = 29408;
    static const int GUY_LAZY = 29409;
    static const int WIFE_FARMER = 29416;
    static const int DRUNK_TRADER_RICH = 29410;
    static const int CANDY_HEART_EATER = 37138;
    static const int FAT_ELF_1 = 3905;
    static const int FAT_ELF_2 = 3906;
    static const int FAT_ELF_3 = 3907;
    static const int DEE = 7600;
    static constexpr int FIRST_WARRIOR_GM = 202;
    static constexpr int FIRST_THIEF_GM = 203;
    static const int FOPPISH_RAKE = 44855;
    static const int OONA = 44856;
    static const int PANICKED_THIEF = 44857;
    static const int LUMTARB = 44858;
    static const int CARAVAN_DRIVER = 44859;
    static const int ALISAARD = 9955;
    static const int GHOST_WPN_MASTER = 7516;
    static const int ROTHWAR = 34038;
    static const int ILLUSION_SHROUD = 27209;
    static const int GNELIX = 14275;
    static const int GILTHANTHIL = 6811;
    static const int LT_CAERIOLL = 9095;
};
