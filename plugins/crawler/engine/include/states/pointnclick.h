#ifndef STATE_POINT_N_CLICK_H
#define STATE_POINT_N_CLICK_H

#include "gbs_types.h"
#include "math.h"
#include <gb/gb.h>

#define VIEW_HEIGHT 10u
#define VIEW_TOP 2u
#define VIEW_WIDTH 20u

extern uint8_t view_dirty;
extern uint8_t minimap;

extern uint8_t crawler_sprite_near_visible;
extern uint8_t crawler_sprite_far_visible;
extern uint8_t crawler_sprite_middle_visible;

void set_bkg_chunk(UBYTE x, UBYTE y, UBYTE w, UBYTE h, UBYTE sx, UBYTE sy) NONBANKED;
uint8_t get_bkg_tile(uint8_t x, uint8_t y) NONBANKED;

void build_view() BANKED;
void solve_view() BANKED;
void player_move_to(uint8_t px, uint8_t py) BANKED;
void get_sprite_visibility() BANKED;

void pointnclick_init() BANKED;
void pointnclick_update() BANKED;

#endif
