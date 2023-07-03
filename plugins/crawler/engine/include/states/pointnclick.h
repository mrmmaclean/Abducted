#ifndef STATE_POINT_N_CLICK_H
#define STATE_POINT_N_CLICK_H

#include <gb/gb.h>

#define VIEW_HEIGHT 10
#define VIEW_TOP 2
#define VIEW_WIDTH 20

void pointnclick_init();
void pointnclick_update();
void build_left() BANKED;
void build_right() BANKED;
void build_view() BANKED;
void solve_view() BANKED;
void set_bkg_chunk(UBYTE x, UBYTE y, UBYTE w, UBYTE h, UBYTE sx, UBYTE sy) NONBANKED;

#endif
