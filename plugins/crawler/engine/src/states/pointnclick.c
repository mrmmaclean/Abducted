#pragma bank 255

#include "data/states_defines.h"
#include "states/pointnclick.h"

#include "data_manager.h"
#include "game_time.h"
#include "input.h"
#include "vm.h"

const int8_t dirx[4] = {0, 1, 0, -1};
const int8_t diry[4] = {-1, 0, 1, 0};

static uint8_t _saved;
static uint8_t view_dirty = TRUE;

static uint8_t px;
static uint8_t py;
static int8_t dir;
static int8_t strafe;

static uint8_t viewport[5][3];
static uint8_t vx;
static uint8_t vy;

static uint8_t dungeon[16][16] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

void build_left() BANKED
{
    // Left
    if (viewport[1][2])
    {
        // Closest side
        set_bkg_chunk(0u, VIEW_TOP, 5u, VIEW_HEIGHT, 20u, 20u);
        // If forward is open...
        if (!viewport[2][1])
        {
            if (viewport[1][1])
            {
                // draw next side
                set_bkg_chunk(5u, VIEW_TOP, 3u, VIEW_HEIGHT, 25u, 20u);
            }
            else
            {
                // draw farthest facing or open
                if (viewport[1][0])
                {
                    set_bkg_chunk(5u, VIEW_TOP, 3u, VIEW_HEIGHT, 5u, 2u);
                }
                else
                {
                    set_bkg_chunk(5u, VIEW_TOP, 3u, VIEW_HEIGHT, 9u, 2u);
                }
            }
        }
    }
    else
    {
        if (viewport[1][1])
        {
            set_bkg_chunk(0, VIEW_TOP, 5u, VIEW_HEIGHT, 25u, 0);
            if (!viewport[2][1])
            {
                set_bkg_chunk(5u, VIEW_TOP, 3u, VIEW_HEIGHT, 25u, 20u);
            }
        }
        else
        {
            if (viewport[0][1])
            {
                set_bkg_chunk(0, VIEW_TOP, 4u, VIEW_HEIGHT, 0, 2u);
            }
            else
            {
                if (viewport[0][0])
                {
                    set_bkg_chunk(0, VIEW_TOP, 4u, VIEW_HEIGHT, 4u, 2u);
                }
                else
                {
                    set_bkg_chunk(0, VIEW_TOP, 4u, VIEW_HEIGHT, 8u, 2u);
                }
            }
            if (viewport[2][1])
            {
                if (viewport[1][0])
                {
                    set_bkg_chunk(4u, VIEW_TOP, 1u, VIEW_HEIGHT, 4u, 2u);
                }
                else
                {
                    set_bkg_chunk(4u, VIEW_TOP, 1u, VIEW_HEIGHT, 8u, 2u);
                }
            }
            else
            {
                if (viewport[1][0])
                {
                    set_bkg_chunk(4u, VIEW_TOP, 4u, VIEW_HEIGHT, 4u, 2u);
                }
                else
                {
                    set_bkg_chunk(4u, VIEW_TOP, 4u, VIEW_HEIGHT, 8u, 2u);
                }
            }
        }
    }
}

void build_right() BANKED
{
    // Right
    if (viewport[3][2])
    {
        // Closest side
        set_bkg_chunk(15u, VIEW_TOP, 5u, VIEW_HEIGHT, 23u, 10u);
        // If forward is open...
        if (!viewport[2][1])
        {
            if (viewport[3][1])
            {
                // draw next side
                set_bkg_chunk(12u, VIEW_TOP, 3u, VIEW_HEIGHT, 20u, 10u);
            }
            else
            {
                // draw farthest facing or open
                if (viewport[3][0])
                {
                    set_bkg_chunk(12u, VIEW_TOP, 3u, VIEW_HEIGHT, 4u, 2u);
                }
                else
                {
                    set_bkg_chunk(12u, VIEW_TOP, 3u, VIEW_HEIGHT, 8u, 2u);
                }
            }
        }
    }
    else
    {
        if (viewport[3][1])
        {
            set_bkg_chunk(15u, VIEW_TOP, 5u, VIEW_HEIGHT, 20u, 0);
            if (!viewport[1][2])
            {
                set_bkg_chunk(12u, VIEW_TOP, 3u, VIEW_HEIGHT, 20u, 10u);
            }
        }
        else
        {
            if (viewport[4][1])
            {
                set_bkg_chunk(16u, VIEW_TOP, 4u, VIEW_HEIGHT, 16u, 2u);
            }
            else
            {
                if (viewport[4][0])
                {
                    set_bkg_chunk(16u, VIEW_TOP, 4u, VIEW_HEIGHT, 4u, 2u);
                }
                else
                {
                    set_bkg_chunk(16u, VIEW_TOP, 4u, VIEW_HEIGHT, 8u, 2u);
                }
            }
            if (viewport[2][1])
            {
                if (viewport[3][0])
                {
                    set_bkg_chunk(15u, VIEW_TOP, 1u, VIEW_HEIGHT, 7u, 2u);
                }
                else
                {
                    set_bkg_chunk(15u, VIEW_TOP, 1u, VIEW_HEIGHT, 11u, 2u);
                }
            }
            else
            {
                if (viewport[3][0])
                {
                    set_bkg_chunk(12u, VIEW_TOP, 4u, VIEW_HEIGHT, 4u, 2u);
                }
                else
                {
                    set_bkg_chunk(12u, VIEW_TOP, 4u, VIEW_HEIGHT, 8u, 2u);
                }
            }
        }
    }
}

void build_view() BANKED
{
    // Tile immediately in front
    if (viewport[2][2])
    {
        set_bkg_chunk(0u, VIEW_TOP, VIEW_WIDTH, VIEW_HEIGHT, 0u, 18u);
    }
    else
    {
        build_left();
        build_right();

        // Build middle
        if (viewport[2][1])
        {
            set_bkg_chunk(5u, VIEW_TOP, 10u, VIEW_HEIGHT, 20u, 0);
        }
        else
        {
            if (viewport[2][0])
            {
                set_bkg_chunk(8u, VIEW_TOP, 4u, VIEW_WIDTH, 4u, 2u);
            }
            else
            {
                set_bkg_chunk(8u, VIEW_TOP, 4u, VIEW_WIDTH, 8u, 2u);
            }
        }
    }
    view_dirty = FALSE;
}

void solve_view() BANKED
{
    switch (dir)
    {
    case 0:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px - 2 + i) % 16;
                vy = (py - 3 + j) % 16;
                viewport[i][j] = dungeon[vx][vy];
            }
        }
        break;
    case 1:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px + 1 + j) % 16;
                vy = (py - 2 + i) % 16;
                viewport[i][2 - j] = dungeon[vx][vy];
            }
        }
        break;
    case 2:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px + 2 - i) % 16;
                vy = (py + 3 - j) % 16;
                viewport[i][j] = dungeon[vx][vy];
            }
        }
        break;
    case 3:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px - 1 - j) % 16;
                vy = (py + 2 - i) % 16;
                viewport[i][2 - j] = dungeon[vx][vy];
            }
        }
        break;
    default:
        break;
    }
    view_dirty = TRUE;
}

void set_bkg_chunk(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t sx, uint8_t sy) NONBANKED
{
    _saved = _current_bank;
    SWITCH_ROM(image_bank);

    set_bkg_submap(x, y, w, h, image_ptr + (sx - x) + (image_tile_width * (sy - y)), image_tile_width);
    SWITCH_ROM(_saved);
}

void pointnclick_init() BANKED
{
    px = 2u;
    py = 3u;
    dir = 0;
    solve_view();
}

void pointnclick_update() BANKED
{
    if (INPUT_UP_PRESSED && !view_dirty)
    {
        px += dirx[dir];
        py += diry[dir];
        if (dungeon[px][py])
        {
            px -= dirx[dir];
            py -= diry[dir];
        }
        else
        {
            solve_view();
        }
    }
    else if (INPUT_RIGHT_PRESSED && !view_dirty)
    {
        if (INPUT_B)
        {
            // Strafe
            strafe = (dir + 1) % 4;
            px += dirx[strafe];
            py += diry[strafe];
            if (dungeon[px][py])
            {
                px -= dirx[strafe];
                py -= diry[strafe];
            }
            else
            {
                solve_view();
            }
        }
        else
        {
            dir = (dir + 1) % 4;
            solve_view();
        }
    }
    else if (INPUT_LEFT_PRESSED && !view_dirty)
    {
        if (INPUT_B)
        {
            // Strafe
            strafe = (dir - 1) % 4;
            px += dirx[strafe];
            py += diry[strafe];
            if (dungeon[px][py])
            {
                px -= dirx[strafe];
                py -= diry[strafe];
            }
            else
            {
                solve_view();
            }
        }
        else
        {
            dir = (dir - 1) % 4;
            solve_view();
        }
    }
    else if (INPUT_DOWN_PRESSED && !view_dirty)
    {
        px -= dirx[dir];
        py -= diry[dir];
        if (dungeon[px][py])
        {
            px += dirx[dir];
            py += diry[dir];
        }
        else
        {
            solve_view();
        }
    }

    if (view_dirty)
    {
        build_view();
    }
}
