#pragma bank 255

#include "data/states_defines.h"
#include "states/pointnclick.h"

#include "actor.h"
#include "data_manager.h"
#include "game_time.h"
#include "input.h"
#include "vm.h"
#include "trigger.h"

const int8_t dirx[4] = {0, 1, 0, -1};
const int8_t diry[4] = {1, 0, -1, 0};

static uint8_t _saved;
static uint8_t view_dirty;

static uint16_t px;
static uint16_t py;
static uint16_t last_px;
static uint16_t last_py;

static uint8_t dir;
static uint8_t strafe;

static uint8_t viewport[5][3];

static uint8_t vx;
static uint8_t vy;
static uint8_t tile;

static uint8_t open;

static uint8_t door_near_visible;
static uint8_t door_far_visible;

void push_doors_to_stack() BANKED
{
    
}

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
            if (!viewport[2][1])
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
                set_bkg_chunk(8u, VIEW_TOP, 4u, VIEW_HEIGHT, 4u, 2u);
            }
            else
            {
                set_bkg_chunk(8u, VIEW_TOP, 4u, VIEW_HEIGHT, 8u, 2u);
            }
        }
    }
    view_dirty = FALSE;
}

void set_bkg_chunk(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t sx, uint8_t sy) NONBANKED
{
    // x, y, w, h, sx, sy;
    _saved = _current_bank;
    SWITCH_ROM(image_bank);

    set_bkg_submap(x, y, w, h, image_ptr + (sx - x) + (image_tile_width * (sy - y)), image_tile_width);
    SWITCH_ROM(_saved);
}

uint8_t get_bkg_tile(uint8_t x, uint8_t y) NONBANKED
{
    tile = ReadBankedUBYTE(image_ptr + (y * image_tile_width) + x, image_bank);
    return tile;
}

void solve_view() BANKED
{
    view_dirty = TRUE;
    px = PLAYER.pos.x >> 7;
    py = PLAYER.pos.y >> 7;

    switch (PLAYER.dir)
    {
    case 0:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px + 2 - i);
                vy = (py + 3 - j);
                viewport[i][j] = get_bkg_tile(vx, vy) == open ? 0 : 1;
            }
        }
        break;
    case 1:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px + 1 + j);
                vy = (py - 2 + i);
                viewport[i][2 - j] = get_bkg_tile(vx, vy) == open ? 0 : 1;
            }
        }
        break;
    case 2:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px - 2 + i);
                vy = (py - 3 + j);
                viewport[i][j] = get_bkg_tile(vx, vy) == open ? 0 : 1;
            }
        }
        break;
    case 3:
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                vx = (px - 1 - j);
                vy = (py + 2 - i);
                viewport[i][2 - j] = get_bkg_tile(vx, vy) == open ? 0 : 1;
            }
        }
        break;
    default:
        break;
    }
}

void player_move_to(uint8_t px, uint8_t py) BANKED
{
    last_px = PLAYER.pos.x;
    last_py = PLAYER.pos.y;
    PLAYER.pos.x = px << 7;
    PLAYER.pos.y = py << 7;
    view_dirty = TRUE;
}

void pointnclick_init() BANKED
{
    // Render view in first update
    view_dirty = TRUE;
    // Set "open" tile to top left tile in map area
    open = get_bkg_tile(0, 32);
}

void pointnclick_update() BANKED
{
    actor_t *hit_actor;

    // Check for trigger collisions
    if (trigger_activate_at_intersection(&PLAYER.bounds, &PLAYER.pos, FALSE))
    {
        // Landed on a trigger
        return;
    }

    // Input
    if (INPUT_UP_PRESSED && !view_dirty)
    {
        px = (PLAYER.pos.x >> 7) + dirx[PLAYER.dir];
        py = (PLAYER.pos.y >> 7) + diry[PLAYER.dir];
        if (get_bkg_tile(px, py) == open)
        {
            player_move_to(px, py);
        }
    }
    else if (INPUT_DOWN_PRESSED && !view_dirty)
    {
        px = (PLAYER.pos.x >> 7) - dirx[PLAYER.dir];
        py = (PLAYER.pos.y >> 7) - diry[PLAYER.dir];
        if (get_bkg_tile(px, py) == open)
        {
            player_move_to(px, py);
        }
    }
    else if (INPUT_RIGHT_PRESSED && !view_dirty)
    {
        if (INPUT_B)
        {
            // Strafe
            strafe = (PLAYER.dir - 1) & 3;
            px = (PLAYER.pos.x >> 7) + dirx[strafe];
            py = (PLAYER.pos.y >> 7) + diry[strafe];
            if (get_bkg_tile(px, py) == open)
            {
                player_move_to(px, py);
            }
        }
        else
        {
            PLAYER.dir = (PLAYER.dir - 1) & 3;
            view_dirty = TRUE;
        }
    }
    else if (INPUT_LEFT_PRESSED && !view_dirty)
    {
        if (INPUT_B)
        {
            // Strafe
            strafe = (PLAYER.dir + 1) & 3;
            px = (PLAYER.pos.x >> 7) + dirx[strafe];
            py = (PLAYER.pos.y >> 7) + diry[strafe];
            if (get_bkg_tile(px, py) == open)
            {
                player_move_to(px, py);
            }
        }
        else
        {
            PLAYER.dir = (PLAYER.dir + 1) & 3;
            view_dirty = TRUE;
        }
    }

    // Check for actor collision
    hit_actor = actor_overlapping_player(FALSE);
    if (hit_actor != NULL && hit_actor->collision_group)
    {
        player_move_to(last_px >> 7, last_py >> 7);
        // Set the player to face the actor (in case of strafe)
        if (last_px > PLAYER.pos.x)
        {
            actor_set_dir(&PLAYER, DIR_RIGHT, FALSE);
        }
        else if (last_px < PLAYER.pos.x)
        {
            actor_set_dir(&PLAYER, DIR_LEFT, FALSE);
        }
        if (last_py > PLAYER.pos.y)
        {
            actor_set_dir(&PLAYER, DIR_DOWN, FALSE);
        }
        else if (last_py < PLAYER.pos.y)
        {
            actor_set_dir(&PLAYER, DIR_UP, FALSE);
        }

        player_register_collision_with(hit_actor);
    }

    // Update view
    if (view_dirty)
    {
        solve_view();
        build_view();
    }
}
