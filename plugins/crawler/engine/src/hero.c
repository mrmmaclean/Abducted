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

uint8_t engine_enemy_str, engine_enemy_agi, engine_enemy_hp_max;
uint8_t engine_enemy_sleep_res, engine_enemy_stun_res, engine_enemy_psy_res, engine_enemy_dodge;
uint8_t engine_enemy_pattern, engine_enemy_xp, engine_enemy_junk;
uint8_t engine_enemy_att_status, engine_enemy_att_status_chance, engine_enemy_att_psy, engine_enemy_att_psy_chance;

/*
Dragon Warrior/Quest Monster stats
----------------------------------
ID  Enemy Name     Str Agi HP  Pat SR  DR  XP  GP
00  Slime          05  03  03  00  0F  01  01  02
01  Red Slime      07  03  04  00  0F  01  01  03
02  Drakee         09  06  06  00  0F  01  02  03
03  Ghost          0B  08  07  00  0F  04  03  05
04  Magician       0B  0C  0D  02  00  01  04  0C
05  Magidrakee     0E  0E  0F  02  00  01  05  0C
06  Scorpion       12  10  14  00  0F  01  06  10
07  Druin          14  12  16  00  0F  02  07  10

08  Poltergeist    12  14  17  03  00  06  08  12
09  Droll          18  18  19  00  0E  02  0A  19
0A  Drakeema       16  1A  14  92  20  06  0B  14
0B  Skeleton       1C  16  1E  00  0F  04  0B  1E
0C  Warlock        1C  16  1E  12  31  02  0D  23
0D  Metal Scorpion 24  2A  16  00  0F  02  0E  28
0E  Wolf           28  1E  22  00  1F  02  10  32
0F  Wraith         2C  22  24  90  70  04  11  3C

10  Metal Slime    0A  FF  04  03  FF  F1  73  06
11  Specter        28  26  24  13  31  04  12  46
12  Wolflord       32  24  26  60  47  02  14  50
13  Druinlord      2F  28  23  B1  F0  04  14  55
14  Drollmagi      34  32  26  60  22  01  16  5A
15  Wyvern         38  30  2A  00  4F  02  18  64
16  Rogue Scorpion 3C  5A  23  00  7F  02  1A  6E
17  Wraith Knight  44  38  2E  B0  50  34  1C  78

18  Golem          78  3C  46  00  FF  F0  05  0A
19  Goldman        30  28  32  00  DF  01  06  C8
1A  Knight         4C  4E  37  60  67  01  21  82
1B  Magiwyvern     4E  44  3A  20  20  02  22  8C
1C  Demon Knight   4F  40  32  00  FF  FF  25  96
1D  Werewolf       56  46  3C  00  7F  07  28  9B
1E  Green Dragon   58  4A  41  09  7F  22  2D  A0
1F  Starwyvern     56  50  41  F9  80  12  2B  A0

20  Wizard         50  46  41  06  F7  F2  32  A5
21  Axe Knight     5E  52  46  10  F3  11  36  A5
22  Blue Dragon    62  54  46  09  FF  72  3C  96
23  Stoneman       64  28  A0  00  2F  71  41  8C
24  Armored Knight 69  56  5A  F5  F7  12  46  8C
25  Red Dragon     78  5A  64  19  F7  F2  64  8C
26  Dragonlord 1   5A  4B  64  57  FF  F0  00  00
27  Dragonlord 2   8C  C8  82  0E  FF  F0  00  00
*/
const uint8_t enemy_table[] = {
    0x05, 0x03, 0x03, 0x06, 0x0F, 0x01, 0x04, 0x04,
    0x28, 0x26, 0x24, 0x13, 0x31, 0x04, 0x12, 0x46};

void get_enemy_stats(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    uint8_t enemy = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);
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
int8_t counter;

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

    if (get_bkg_tile(px, py) == wall || get_bkg_tile(px, py) == door)
    {
        actor_set_dir(&PLAYER, FLIPPED_DIR(PLAYER.dir), FALSE);
        minimap = FALSE;
        view_dirty = TRUE;
        is_facing_cell = 1;
    };
}
