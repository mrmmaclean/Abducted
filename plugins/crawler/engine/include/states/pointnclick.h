#ifndef STATE_POINT_N_CLICK_H
#define STATE_POINT_N_CLICK_H

#include <gb/gb.h>

#define VIEW_HEIGHT 10u
#define VIEW_TOP 2u
#define VIEW_WIDTH 20u

extern uint8_t crawler_sprite_near_visible;
extern uint8_t crawler_sprite_far_visible;
extern uint8_t crawler_sprite_middle_visible;
// extern uint8_t crawler_actor;

void pointnclick_init();
void pointnclick_update();
void build_left() BANKED;
void build_right() BANKED;
void build_view() BANKED;
void solve_view() BANKED;
void player_move_to(uint8_t px, uint8_t py) BANKED;
void set_bkg_chunk(UBYTE x, UBYTE y, UBYTE w, UBYTE h, UBYTE sx, UBYTE sy) NONBANKED;
uint8_t get_bkg_tile(uint8_t x, uint8_t y) NONBANKED;

#endif
