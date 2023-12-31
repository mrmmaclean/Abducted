#pragma bank 255

#include <rand.h>
#include <gbdk/platform.h>

#include "gbs_types.h"
#include "actor.h"
#include "vm.h"
#include "scroll.h"

#include "states/pointnclick.h"

// #define MAX_LEVELS 20u
// #define NUM_STATS 4u

uint8_t engine_player_str, engine_player_agi, engine_player_hp, engine_player_mp;
uint8_t engine_shield_bonus, engine_armour_bonus, engine_weapon_bonus;
int16_t engine_player_next_xp;

const uint8_t hero_table[] = {
    6, 6, 15, 0,
    7, 6, 22, 0,
    9, 8, 24, 9,
    9, 10, 31, 15,
    14, 12, 35, 19,
    18, 12, 38, 22,
    20, 18, 40, 24,
    24, 21, 46, 27,
    32, 22, 50, 33,
    37, 30, 54, 37,
    42, 34, 62, 46,
    50, 39, 63, 53,
    54, 46, 70, 58,
    62, 52, 78, 64,
    70, 60, 86, 65,
    74, 66, 92, 86,
    74, 73, 100, 91,
    87, 78, 115, 98,
    89, 80, 130, 104,
    94, 82, 138, 116};

const uint16_t xp_table[] = {
    7, 23, 47, 110, 220, 450, 800, 1300,
    2000, 2900, 4000, 5500, 7500, 10000, 13000, 16000,
    19000, 22000, 26000, 0};

const uint8_t shield_bonus_table[] = {0, 5, 10, 20};
const uint8_t armour_bonus_table[] = {0, 4, 10, 16, 24, 28};
const uint8_t weapon_bonus_table[] = {0, 4, 10, 16, 24, 40};

void get_stats(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    uint8_t level = *(int16_t *)VM_REF_TO_PTR(FN_ARG3);
    engine_player_next_xp = xp_table[level - 1];

    level = (level - 1) << 2;
    engine_player_str = hero_table[level + 0];
    engine_player_agi = hero_table[level + 1];
    engine_player_hp = hero_table[level + 2];
    engine_player_mp = hero_table[level + 3];

    uint8_t shield = *(int16_t *)VM_REF_TO_PTR(FN_ARG2);
    engine_shield_bonus = shield_bonus_table[shield];

    uint8_t armour = *(int16_t *)VM_REF_TO_PTR(FN_ARG1);
    engine_armour_bonus = armour_bonus_table[armour];

    uint8_t weapon = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);
    engine_weapon_bonus = weapon_bonus_table[weapon];
}

int16_t engine_enemy;
uint8_t engine_enemy_str, engine_enemy_agi, engine_enemy_hp_max;
uint8_t engine_enemy_sleep_res, engine_enemy_stun_res, engine_enemy_psy_res, engine_enemy_dodge;
uint8_t engine_enemy_pattern, engine_enemy_xp, engine_enemy_junk;
uint8_t engine_enemy_att_status, engine_enemy_att_status_chance, engine_enemy_att_psy, engine_enemy_att_psy_chance;

/*
Dragon Warrior/Quest Monster stats
reskinned and edited for this project
----------------------------------
    Str   Agi   HP    Pat   SR    DR    XP    GP
*/
const uint8_t enemy_table[] = {
    0x05, 0x03, 0x03, 0x00, 0x0F, 0x01, 0x01, 0x02, // Lazy Rent-A-Cop
    0x07, 0x03, 0x04, 0x00, 0x0F, 0x01, 0x01, 0x03, // Grumpy Rent-A-Cop
    0x0B, 0x08, 0x07, 0x00, 0x0F, 0x04, 0x03, 0x05, // Low-budget Drone
    0x0B, 0x0C, 0x0D, 0x02, 0x00, 0x01, 0x04, 0x0C, // Twitchy cop
    0x14, 0x12, 0x16, 0x00, 0x0F, 0x02, 0x07, 0x10, // Rabid Rat
    0x12, 0x14, 0x17, 0x03, 0x00, 0x06, 0x08, 0x12, // Sentry Drone
    0x18, 0x18, 0x19, 0x00, 0x0E, 0x02, 0x0A, 0x19, // Rejected Hybrid
    0x1C, 0x16, 0x1E, 0x00, 0x0F, 0x04, 0x0B, 0x1E, // Junior Agent
    0x1C, 0x16, 0x1E, 0x12, 0x31, 0x02, 0x0D, 0x23, // Riot Cop
    0x2C, 0x22, 0x24, 0x90, 0x70, 0x04, 0x11, 0x3C, // Low-Level G-Man
    0x0A, 0xFF, 0x04, 0x03, 0xFF, 0xF1, 0x73, 0x06, // Wily Rent-A-Cop
    0x28, 0x26, 0x24, 0x13, 0x31, 0x04, 0x12, 0x46, // Defense Drone
    0x2F, 0x28, 0x23, 0xB1, 0xF0, 0x04, 0x14, 0x55, // Rat With a Gun!
    0x34, 0x32, 0x26, 0x60, 0x22, 0x01, 0x16, 0x5A, // Screaming Hybrid
    0x38, 0x30, 0x2A, 0x00, 0x4F, 0x02, 0x18, 0x64, // Test Subject
    0x44, 0x38, 0x2E, 0xB0, 0x50, 0x34, 0x1C, 0x78, // Dirty Detective
    0x78, 0x3C, 0x46, 0x00, 0xFF, 0xF0, 0x05, 0x0A, // Rowdy Hunter (boss)
    0x30, 0x28, 0x32, 0x00, 0xDF, 0x01, 0x06, 0xC8, // Poacher
    0x4C, 0x4E, 0x37, 0x60, 0x67, 0x01, 0x21, 0x82, // Mercenary
    0x4E, 0x44, 0x3A, 0x20, 0x20, 0x02, 0x22, 0x8C, // Dull Psychic
    0x4F, 0x40, 0x32, 0x00, 0xFF, 0xFF, 0x25, 0x96, // Superintendent
    0x58, 0x4A, 0x41, 0x09, 0x7F, 0x22, 0x2D, 0xA0, // Exoskeleton (boss)
    0x56, 0x50, 0x41, 0xF9, 0x80, 0x12, 0x2B, 0xA0, // Glowing Psychic
    0x50, 0x46, 0x41, 0x06, 0xF7, 0xF2, 0x32, 0xA5, // S.W.A.T.
    0x5E, 0x52, 0x46, 0x10, 0xF3, 0x11, 0x36, 0xA5, // Covert Assassin
    0x62, 0x54, 0x46, 0x09, 0xFF, 0x72, 0x3C, 0x96, // Nuclear Biped
    0x64, 0x28, 0xA0, 0x00, 0x2F, 0x71, 0x41, 0x8C, // Armed Recluse
    0x69, 0x56, 0x5A, 0xF5, 0xF7, 0x12, 0x46, 0x8C, // Super Soldier
    0x78, 0x5A, 0x64, 0x19, 0xF7, 0xF2, 0x64, 0x8C, // Nuclear Biped MkII
    0x5A, 0x4B, 0x64, 0x57, 0xFF, 0xF0, 0x00, 0x00, // Sibling (boss)
    0x8C, 0xC8, 0x82, 0x0E, 0xFF, 0xF0, 0x00, 0x00  // Sibling
};

const uint8_t encounter_table[] = {
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Lab
    0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    26, 27, 28, 23, 22, 23, 24, 25, 22, 23, 24, 22, 3, 2, 3, 5, // Forest
    0, 1, 1, 2, 0, 2, 3, 1, 1, 2, 3, 2, 1, 2, 2, 3,
    5, 7, 8, 5, 3, 5, 5, 7, 3, 5, 3, 7, 3, 2, 3, 5,
    9, 8, 7, 11, 17, 15, 18, 15, 0, 0, 0, 0, 0, 0, 0, 0,
    4, 4, 4, 6, 4, 5, 4, 6, 9, 11, 9, 12, 9, 11, 9, 12, // Cave
    4, 6, 5, 6, 4, 6, 6, 7, 13, 11, 12, 9, 11, 12, 12, 9,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    9, 11, 15, 11, 9, 11, 15, 11, 14, 15, 11, 15, 9, 14, 11, 15, // Warehouse
    18, 19, 15, 18, 20, 19, 20, 22, 22, 20, 23, 20, 23, 22, 24, 25,
    0, 0, 0, 0, 20, 19, 20, 22, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 18, 20, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0};

int16_t test_x, test_y, test_z;

void get_enemy_stats(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    int16_t zone = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);

    if (zone >= 0)
    {
        uint8_t zone_x = (PLAYER.pos.x >> 7) / 16;
        uint8_t zone_y = ((PLAYER.pos.y >> 7) / 16) - 2;

        engine_enemy = (zone * 64u) + (((zone_y * 4u) + zone_x) * 4u) + (rand() & 3u);
        engine_enemy = encounter_table[engine_enemy];
    }
    else
    {
        engine_enemy = zone >> 7u;
        zone += engine_enemy;
        zone ^= engine_enemy;
        engine_enemy = zone;
    }

    uint8_t enemy = engine_enemy * 8u;

    engine_enemy_str = enemy_table[enemy + 0];
    engine_enemy_agi = enemy_table[enemy + 1];
    engine_enemy_hp_max = enemy_table[enemy + 2] - ((rand() * enemy_table[enemy + 2]) >> 10);
    // engine_enemy_pattern = enemy_table[enemy + 3];

    engine_enemy_att_status_chance = enemy_table[enemy + 3] >> 4 & 0x03;
    engine_enemy_att_status = enemy_table[enemy + 3] >> 6 & 0x03;
    engine_enemy_att_psy_chance = enemy_table[enemy + 3] & 0x03;
    engine_enemy_att_psy = enemy_table[enemy + 3] >> 2 & 0x03;

    engine_enemy_sleep_res = enemy_table[enemy + 4] >> 4;
    engine_enemy_stun_res = enemy_table[enemy + 4] & 0x0F;
    engine_enemy_psy_res = enemy_table[enemy + 5] >> 4;
    engine_enemy_dodge = enemy_table[enemy + 5] & 0x0F;

    engine_enemy_xp = enemy_table[enemy + 6];
    engine_enemy_junk = (enemy_table[enemy + 7] * ((rand() & 0x3F) + 0xC0)) >> 8; //(GP * ((RAND & 0x3F) + 0xC0)) >> 8

    // x2 gold/xp
    engine_enemy_junk *= 2;
    engine_enemy_xp *= 2;
}

void flash_sprite(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    for (uint8_t i = *(int16_t *)VM_REF_TO_PTR(FN_ARG0); i != 0; i--)
    {
        // BGP_REG = ~BGP_REG;
        OBP1_REG = ~OBP1_REG;
        // OBP0_REG = ~OBP0_REG;

        if (i)
        {
            wait_vbl_done();
            wait_vbl_done();
        }
    }
}

void flash_bkg(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    for (uint8_t i = *(int16_t *)VM_REF_TO_PTR(FN_ARG0); i != 0; i--)
    {
        BGP_REG = ~BGP_REG;
        // OBP1_REG = ~OBP1_REG;
        // OBP0_REG = ~OBP0_REG;

        if (i)
        {
            wait_vbl_done();
            wait_vbl_done();
        }
    }
}

const int8_t wobble_table[] = {4, 7, 9, 10, 9, 7, 4, 0 - 4, -7, -9, -10, -9, -7, -4, 0};
int8_t wobble_index;
int16_t counter;

UBYTE camera_wobble_frames(void *THIS, UBYTE start, UWORD *stack_num) OLDCALL BANKED
{
    start; // to avoid error
    if (counter < stack_num[0])
    {
        if (wobble_index < sizeof(wobble_table))
        {
            scroll_offset_x = wobble_table[wobble_index];
            wobble_index++;
            ((SCRIPT_CTX *)THIS)->waitable = TRUE;
            return FALSE;
        }
        wobble_index = 0;
        counter++;
        if (counter < stack_num[0])
        {
            return FALSE;
        }
    }

    scroll_offset_x = 0;
    return TRUE;
}

uint8_t is_facing_cell;

void get_is_facing_cell(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    THIS;
    uint8_t px, py;
    is_facing_cell = 0;

    px = (PLAYER.pos.x >> 7) + dirx[PLAYER.dir];
    py = (PLAYER.pos.y >> 7) + diry[PLAYER.dir];

    // if (get_bkg_tile(px, py) == wall || get_bkg_tile(px, py) == door)
    if (get_bkg_tile(px, py) != open)
    {
        actor_set_dir(&PLAYER, FLIPPED_DIR(PLAYER.dir), FALSE);
        minimap = FALSE;
        view_dirty = TRUE;
        is_facing_cell = 1;
    };
}

// index based on zone, x, and y
const int16_t chest_lut[] = {
    0x9cb, 0xb48, 0xc08, 0xc83, 0xdc4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, // lab
    0x8c2, 0x8d7, 0x97c, 0xa94, 0xadd, 0xb2a, 0xc89, 0xcee,
    0xd53, 0x108a, 0x1172, 0x12fd, 0x15a8, 0x1752, 0x1753, 0, // forest
    0x90b, 0x913, 0x97d, 0xa70, 0xb18, 0xb52, 0xc0b, 0xdb5,
    0xe67, 0xed7, 0, 0, 0, 0, 0, 0, // caves
    0x936, 0x964, 0x9f8, 0xa23, 0xa27, 0xa43, 0xa4e, 0xc92,
    0xcaa, 0xce3, 0xce4, 0xce5, 0xd4a, 0xeeb, 0x1092, 0x1453 // warehouse
};

/* idx  item
    -1  MedGel
    -2  Flare
    -3  Em Port
    -4  Cloak
    -5  Lockpick

    -6 Silo key 1
    -8 Silo key 2

    -7 Shield 1
    -7 Shield 2
    -7 Shield 3

    -9 Raygun boost
    -10 Armour boost

*/
const int16_t treasure_lut[] = {
    120, -1, -2, -5, 80, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, // lab
    -3, -1, -2, 500, -1, -4, 60, -7,
    -2, -10, -1, -7, -6, 200, -1, 0, // forest
    -1, -2, -2, 300, -5, -1, 100, 0,
    -7, -3, 0, 0, 0, 0, 0, 0, // caves
    -8, -1, -1, -5, -1, -3, 50, -2,
    -4, 300, -1, 250, -9, -3, 100, 0 // warehouse
};

int16_t chest_contents;
int16_t new_flags, flags;

void get_treasure_chest(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    chest_contents = 0; // empty
    uint8_t index = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);

    for (;; index++)
    {
        if (chest_lut[index] == chest_index)
        {
            counter = 1 << (index % 16u);
            if (!(flags & counter))
            {
                new_flags = flags | counter;
                chest_contents = treasure_lut[index];
            }
            return;
        }
        if (index >= sizeof(chest_lut))
            return;
    }
}
