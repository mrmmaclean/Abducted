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

void get_stats(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    uint8_t level = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);
    engine_player_next_xp = xp_table[level - 1];

    level = (level - 1) << 2;
    engine_player_str = hero_table[level + 0];
    engine_player_agi = hero_table[level + 1];
    engine_player_hp = hero_table[level + 2];
    engine_player_mp = hero_table[level + 3];
}

uint8_t engine_enemy;
uint8_t engine_enemy_str, engine_enemy_agi, engine_enemy_hp_max;
uint8_t engine_enemy_sleep_res, engine_enemy_stun_res, engine_enemy_psy_res, engine_enemy_dodge;
uint8_t engine_enemy_pattern, engine_enemy_xp, engine_enemy_junk;
uint8_t engine_enemy_att_status, engine_enemy_att_status_chance, engine_enemy_att_psy, engine_enemy_att_psy_chance;

/*
Dragon Warrior/Quest Monster stats
----------------------------------


ID  Enemy Name     Str Agi HP  Pat SR  DR  XP  GP
// 00  Slime          05  03  03  00  0F  01  01  02 * Lazy Rent-A-Cop
// 01  Red Slime      07  03  04  00  0F  01  01  03 * Grumpy Rent-A-Cop
// 03  Ghost          0B  08  07  00  0F  04  03  05 * Low-Budget Drone
// 04  Magician       0B  0C  0D  02  00  01  04  0C * Twitchy Cop
07  Druin          14  12  16  00  0F  02  07  10 * Rabit Rat
08  Poltergeist    12  14  17  03  00  06  08  12 * Sentry Drone
09  Droll          18  18  19  00  0E  02  0A  19 * Rejected Hybrid
0B  Skeleton       1C  16  1E  00  0F  04  0B  1E * Junior Agent
0C  Warlock        1C  16  1E  12  31  02  0D  23 * Riot Cop
0F  Wraith         2C  22  24  90  70  04  11  3C * Low-Level G-Man
10  Metal Slime    0A  FF  04  03  FF  F1  73  06 * Wily Rent-A-Cop
11  Specter        28  26  24  13  31  04  12  46 * Defense Drone
13  Druinlord      2F  28  23  B1  F0  04  14  55 * Rat With a Gun!
14  Drollmagi      34  32  26  60  22  01  16  5A * Screaming Hybrid
15  Wyvern         38  30  2A  00  4F  02  18  64 * Test Subject
17  Wraith Knight  44  38  2E  B0  50  34  1C  78 * Dirty Detective
18  Golem          78  3C  46  00  FF  F0  05  0A * Rowdy Hunter
19  Goldman        30  28  32  00  DF  01  06  C8 * Poacher
1A  Knight         4C  4E  37  60  67  01  21  82 * Mercenary
1B  Magiwyvern     4E  44  3A  20  20  02  22  8C * Dull Psychic
1C  Demon Knight   4F  40  32  00  FF  FF  25  96 * Soldier of Fortune
1E  Green Dragon   58  4A  41  09  7F  22  2D  A0 * Exoskeleton
1F  Starwyvern     56  50  41  F9  80  12  2B  A0 * Glowing Psychic
20  Wizard         50  46  41  06  F7  F2  32  A5 * S.W.A.T.
21  Axe Knight     5E  52  46  10  F3  11  36  A5 * Covert Assassin
22  Blue Dragon    62  54  46  09  FF  72  3C  96 * Nuclear Biped
23  Stoneman       64  28  A0  00  2F  71  41  8C * Armed Recluse
24  Armored Knight 69  56  5A  F5  F7  12  46  8C * Super Soldier
25  Red Dragon     78  5A  64  19  F7  F2  64  8C * Nuclear Biped Rex
26  Dragonlord 1   5A  4B  64  57  FF  F0  00  00 * Sibling
27  Dragonlord 2   8C  C8  82  0E  FF  F0  00  00 * Sibling
*/
const uint8_t enemy_table[] = {
    0x05, 0x03, 0x03, 0x00, 0x0F, 0x01, 0x02, 0x04, // 0  - Lazy Rent-A-Cop
    0x07, 0x03, 0x04, 0x00, 0x0F, 0x01, 0x02, 0x06, // 1  - Grumpy Rent-A-Cop
    0x0B, 0x08, 0x07, 0x00, 0x0F, 0x04, 0x03, 0x05, // 2  - Low-budget Drone
    0x0B, 0x0C, 0x0D, 0x02, 0x00, 0x01, 0x04, 0x0C, // 3  - Twitchy cop
    0x14, 0x12, 0x16, 0x00, 0x0F, 0x02, 0x07, 0x10  // 4  - Rabid Rat
};

const uint8_t encounter_table[] = {
    10, 11, 12, 13, 20, 21, 22, 23, 0, 0, 0, 0, 0, 0, 0, 0,
    30, 31, 32, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t test_x, test_y;

void get_enemy_stats(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    uint8_t zone = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);

    uint8_t zone_x = (PLAYER.pos.x >> 7) / 16;
    uint8_t zone_y = ((PLAYER.pos.y >> 7) / 16) - 2;

    engine_enemy = (zone * 32u) + (((zone_y * 4u) + zone_x) * 4u) + (rand() & 3u);

    uint8_t enemy = encounter_table[engine_enemy] * 8u;

    // uint8_t enemy = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);
    enemy = (enemy - 1) * 8;

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
    0x9CB, 0xB48, 0xC08, 0xC83, 0xDC4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                 // lab
    0x8C2, 0x96C, 0xA94, 0xADD, 0xB2A, 0xB2C, 0xC89, 0xCEE, 0xD53, 0, 0, 0, 0, 0, 0, 0, // forest
    0x90B, 0x913, 0x97D, 0xC0B, 0xDB5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                 // caves
    0x95E, 0xA1D, 0xA21, 0xA43, 0xA4E, 0xC5F, 0xD21, 0, 0, 0, 0, 0, 0, 0, 0, 0          // warehouse
};

/* idx  item
    -1  MedGel
    -2  Flare
    -3  Em Port
    -4  Cloak
    -5  Lockpick

*/
const int16_t treasure_lut[] = {
    120, -1, -2, -5, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // lab
    -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,       // forest
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,        // caves
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0         // warehouse
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
