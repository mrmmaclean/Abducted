#pragma bank 255

#include <gbdk/platform.h>
#include "gbs_types.h"
#include "actor.h"
#include "vm.h"

#define MAX_LEVELS 20u
#define NUM_STATS 4u

uint8_t engine_player_str, engine_player_agi, engine_player_hp, engine_player_mp;

const uint8_t hero_table[MAX_LEVELS * NUM_STATS] = {
    6, 6, 15, 0,
    7, 6, 22, 0,
    9, 8, 24, 5,
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

void get_stats(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    uint8_t level = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);
    level = (level - 1) << 2;
    engine_player_str = hero_table[level + 0];
    engine_player_agi = hero_table[level + 1];
    engine_player_hp = hero_table[level + 2];
    engine_player_mp = hero_table[level + 3];
}

uint8_t engine_enemy_str, engine_enemy_agi, engine_enemy_hp_max;
uint8_t engine_enemy_sleep_res, engine_enemy_stun_res, engine_enemy_psy_res, engine_enemy_dodge;
uint8_t engine_enemy_pattern, engine_enemy_xp, engine_enemy_data;
uint8_t engine_enemy_att_status, engine_enemy_att_status_chance, engine_enemy_att_psy, engine_enemy_att_psy_chance;

/*
ID  Enemy Name     Str Agi HP  Pat SR  DR  XP  GP
00  Slime          05  03  03  00  0F  01  01  02
*/
const uint8_t enemy_table[] = {
    0x05, 0x03, 0x03, 0x00, 0x0F, 0x01, 0x01, 0x02};

void get_enemy_stats(SCRIPT_CTX *THIS) OLDCALL BANKED
{
    uint8_t enemy = *(int16_t *)VM_REF_TO_PTR(FN_ARG0);
    enemy = (enemy - 1) * 8;

    engine_enemy_str = enemy_table[enemy + 0];
    engine_enemy_agi = enemy_table[enemy + 1];
    engine_enemy_hp_max = enemy_table[enemy + 2];
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
    engine_enemy_data = enemy_table[enemy + 7];
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