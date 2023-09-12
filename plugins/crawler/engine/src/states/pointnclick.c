#pragma bank 255

// #include "gbdk/platform.h"
#include "data/states_defines.h"
#include "states/pointnclick.h"

#include "actor.h"
#include "data_manager.h"
#include "game_time.h"
#include "input.h"
#include "vm.h"
#include "trigger.h"
#include "palette.h"

const int8_t dirx[4] = {0, 1, 0, -1};
const int8_t diry[4] = {1, 0, -1, 0};
const uint16_t *vram_ptr = 0x9a00;

static uint8_t _saved;

static uint16_t px;
static uint16_t py;
static uint16_t last_px;
static uint16_t last_py;

static uint8_t dir;
static uint8_t strafe;

static uint8_t viewport[3][4];

static uint8_t vx;
static uint8_t vy;
static uint8_t tile;

uint8_t open;
uint8_t wall;
uint8_t door;
uint8_t chest;

static uint8_t l_side, r_side;
static uint8_t set_buffer;

#define cam_offset 16

uint8_t crawler_sprite_near_visible = FALSE;
uint8_t crawler_sprite_far_visible = FALSE;
uint8_t crawler_sprite_middle_visible = FALSE;

uint8_t minimap;
uint8_t is_minimap_selected;
static uint8_t old_minimap;
uint8_t view_dirty;

uint8_t in_combat;
uint8_t is_chest_ahead;
uint8_t facing_x, facing_y;
int16_t chest_index;
static actor_t *chest_actor;

void set_bkg_chunk(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t sx, uint8_t sy) NONBANKED
{
    // x, y, w, h, sx, sy;
    _saved = _current_bank;
    SWITCH_ROM(image_bank);

    set_bkg_submap(x, y + (cam_offset * set_buffer), w, h, image_ptr + (sx - x) + (image_tile_width * (sy - y - (cam_offset * set_buffer))), image_tile_width);
    SWITCH_ROM(_saved);
}

uint8_t get_bkg_tile(uint8_t x, uint8_t y) NONBANKED
{
    tile = ReadBankedUBYTE(image_ptr + (y * image_tile_width) + x, image_bank);
    return tile;
}

void build_view() BANKED
{
    l_side = FALSE;
    r_side = FALSE;

    // draw from back to front
    // Background
    set_bkg_chunk(0u, 2u, 20u, 10u, 27u, 0u);
    fill_bkg_rect(30u, 0u, 2u, 18u, open);

    // Back wall
    for (uint8_t i = 0; i < 3; i++)
    {
        if (viewport[i][3u])
        {
            if (viewport[i][3u] == door)
                set_bkg_chunk((i << 2) + 4, 6u, 4u, 4u, 43u, 10u);
            else if (viewport[i][3u] == wall)
                set_bkg_chunk((i << 2) + 4, 6u, 4u, 4u, 33u, 10u);
        }
    }

    // Back row
    for (uint8_t i = 0; i < 3; i++)
    {
        if (viewport[i][2u] != 0 && viewport[i][2u] != chest)
        {
            for (uint8_t col = 0; col < 6; col++)
            {
                if (viewport[i][2u] == door)
                    set_bkg_chunk(col + (i * 6u) + 1, 5u, 1u, 6u, 37u + col, 10u);
                else if (viewport[i][2u] == wall)
                    set_bkg_chunk(col + (i * 6u) + 1, 5u, 1u, 6u, 27u + col, 10u);
            }
            if (i == 0)
                l_side = TRUE;
            if (i == 2)
                r_side = TRUE;
        }
    }
    if (viewport[1u][2u] == 0 || viewport[1u][2u] == chest)
    {
        if (l_side)
            set_bkg_chunk(7u, 5u, 1u, 6u, 63u, 3u);
        if (r_side)
            set_bkg_chunk(12u, 5u, 1u, 6u, 57u, 13u);
    }

    // Middle row
    l_side = FALSE;
    r_side = FALSE;

    for (uint8_t i = 0; i < 3; i++)
    {
        if (viewport[i][1u] != 0 && viewport[i][1u] != chest)
        {
            for (uint8_t col = 0; col < 10; col++)
            {
                if (col + (i * 10) - 5 >= 1 && col + (i * 10) - 5 < 19)
                {
                    if (viewport[i][1u] == door)
                        set_bkg_chunk(col + (i * 10u) - 5, 3u, 1u, 9u, 47u + col, 1u);
                    else if (viewport[i][1u] == wall)
                        set_bkg_chunk(col + (i * 10u) - 5, 3u, 1u, 9u, 47u + col, 11u);
                }
            }
            if (i == 0)
                l_side = TRUE;
            if (i == 2)
                r_side = TRUE;
        }
    }
    if (viewport[1u][1u] == 0 || viewport[1u][1u] == chest)
    {
        if (l_side)
            set_bkg_chunk(5u, 3u, 2u, 9u, 61u, 1u);
        if (r_side)
            set_bkg_chunk(13u, 3u, 2u, 9u, 58u, 11u);
    }

    // Front row
    if (viewport[1u][0] != 0 && viewport[1u][0] != chest)
    {
        if (viewport[1u][0] == door)
            set_bkg_chunk(1u, 2u, 18u, 10u, 27u, 16u);
        else if (viewport[1u][0] == wall)
            set_bkg_chunk(1u, 2u, 18u, 10u, 46u, 20u);
    }
    else
    {
        if (viewport[0][0])
            set_bkg_chunk(1u, 2u, 4u, 10u, 57u, 0u);
        if (viewport[2u][0])
            set_bkg_chunk(15u, 2u, 4u, 10u, 60u, 10u);
    }
}

void solve_view() BANKED
{
    view_dirty = TRUE;
    px = PLAYER.pos.x >> 7;
    py = PLAYER.pos.y >> 7;

    switch (PLAYER.dir)
    {
    case 0:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 4; j++)
            {
                vx = (px + 1 - i);
                vy = (py + 1 + j);
                viewport[i][j] = get_bkg_tile(vx, vy) == open ? 0 : get_bkg_tile(vx, vy);
            }
        }
        break;
    case 1:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 4; j++)
            {
                vx = (px + 1 + j);
                vy = (py - 1 + i);
                viewport[i][j] = get_bkg_tile(vx, vy) == open ? 0 : get_bkg_tile(vx, vy);
            }
        }
        break;
    case 2:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 4; j++)
            {
                vx = (px - 1 + i);
                vy = (py - 1 - j);
                viewport[i][j] = get_bkg_tile(vx, vy) == open ? 0 : get_bkg_tile(vx, vy);
            }
        }
        break;
    case 3:
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 4; j++)
            {
                vx = (px - 1 - j);
                vy = (py + 1 - i);
                viewport[i][j] = get_bkg_tile(vx, vy) == open ? 0 : get_bkg_tile(vx, vy);
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

void get_sprite_visibility() BANKED
{
    // If there's an actor, then show it
    actor_t *hit_actor;

    // Get chest actor once
    if (chest_actor == NULL)
        chest_actor = actor_at_tile(3u, 32u, FALSE);

    is_chest_ahead = FALSE;

    if (viewport[1u][0] != wall)
    {
        hit_actor = actor_in_front_of_player(8, FALSE);
        if (hit_actor != NULL && hit_actor->collision_group && hit_actor->active)
        {
            // crawler_actor = 0;
            crawler_sprite_near_visible = TRUE;
            script_execute(hit_actor->script.bank, hit_actor->script.ptr, 0, 1, (uint16_t)(COLLISION_GROUP_1));
            return;
        }
        if (viewport[1u][0] == chest)
        {
            is_chest_ahead = TRUE;
            crawler_sprite_near_visible = TRUE;

            facing_x = (PLAYER.pos.x >> 7) + dirx[PLAYER.dir];
            facing_y = (PLAYER.pos.y >> 7) + diry[PLAYER.dir];
            chest_index = facing_y * image_tile_width + facing_x;

            script_execute(chest_actor->script.bank, chest_actor->script.ptr, 0, 1, (uint16_t)(COLLISION_GROUP_1));
            return;
        }
    }
    else
        return;

    if (viewport[1u][1u] != wall)
    {
        hit_actor = actor_in_front_of_player(16, FALSE);
        if (hit_actor != NULL && hit_actor->collision_group == COLLISION_GROUP_1 && hit_actor->active)
        {
            crawler_sprite_middle_visible = TRUE;
            script_execute(hit_actor->script.bank, hit_actor->script.ptr, 0, 1, (uint16_t)(COLLISION_GROUP_2));
            return;
        }
    }
    else
        return;

    if (viewport[1u][2u] != wall)
    {
        hit_actor = actor_in_front_of_player(24, FALSE);
        if (hit_actor != NULL && hit_actor->collision_group == COLLISION_GROUP_1 && hit_actor->active)
        {
            crawler_sprite_far_visible = TRUE;
            script_execute(hit_actor->script.bank, hit_actor->script.ptr, 0, 1, (uint16_t)(COLLISION_GROUP_3));
            return;
        }
    }
    else
        return;
}

void pointnclick_init() BANKED
{
    // Render view in first update
    view_dirty = TRUE;

    minimap = FALSE;
    is_minimap_selected = FALSE;
    old_minimap = FALSE;
    in_combat = FALSE;

    // Set "open" tile to top left tile in map area
    open = get_bkg_tile(0u, 32u);
    wall = get_bkg_tile(1u, 32u);
    door = get_bkg_tile(2u, 32u);
    chest = get_bkg_tile(3u, 32u);

    // Set second sprite palette to default
    DMG_palette[2] = DMG_PALETTE(DMG_BLACK, DMG_WHITE, DMG_LITE_GRAY, DMG_BLACK);
}

void pointnclick_update() BANKED
{
    actor_t *hit_actor;
    if (trigger_activate_at_intersection(&PLAYER.bounds, &PLAYER.pos, FALSE))
    {
        return;
    }

    // Input
    if (!view_dirty)
    {
        if (INPUT_UP_PRESSED || INPUT_A_PRESSED)
        {
            px = (PLAYER.pos.x >> 7) + dirx[PLAYER.dir];
            py = (PLAYER.pos.y >> 7) + diry[PLAYER.dir];
            if (get_bkg_tile(px, py) != wall)
            {
                player_move_to(px, py);
            }
        }
        else if (INPUT_DOWN_PRESSED)
        {
            px = (PLAYER.pos.x >> 7) - dirx[PLAYER.dir];
            py = (PLAYER.pos.y >> 7) - diry[PLAYER.dir];
            if (get_bkg_tile(px, py) == open)
            {
                player_move_to(px, py);
            }
        }
        else if (INPUT_RIGHT_PRESSED)
        {
            // if (INPUT_B)
            // {
            //     // Strafe
            //     strafe = (PLAYER.dir - 1) & 3;
            //     px = (PLAYER.pos.x >> 7) + dirx[strafe];
            //     py = (PLAYER.pos.y >> 7) + diry[strafe];
            //     if (get_bkg_tile(px, py) == open)
            //     {
            //         player_move_to(px, py);
            //     }
            // }
            // else
            {
                // Turn
                PLAYER.dir = (PLAYER.dir - 1) & 3;
                view_dirty = TRUE;
            }
        }
        else if (INPUT_LEFT_PRESSED)
        {
            // if (INPUT_B)
            // {
            //     // Strafe
            //     strafe = (PLAYER.dir + 1) & 3;
            //     px = (PLAYER.pos.x >> 7) + dirx[strafe];
            //     py = (PLAYER.pos.y >> 7) + diry[strafe];
            //     if (get_bkg_tile(px, py) == open)
            //     {
            //         player_move_to(px, py);
            //     }
            // }
            // else
            {
                // Turn
                PLAYER.dir = (PLAYER.dir + 1) & 3;
                view_dirty = TRUE;
            }
        }
    }

    // Update view
    if (view_dirty)
    {
        // Check for actor collision
        hit_actor = NULL;

        hit_actor = actor_overlapping_player(FALSE);
        if (hit_actor != NULL && hit_actor->collision_group && hit_actor->active)
        {
            // Put player back to last space
            player_move_to(last_px >> 7, last_py >> 7);
            // Set the player to face the actor
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

            // Turn off minimap
            minimap = FALSE;
            player_register_collision_with(hit_actor);
        }

        crawler_sprite_far_visible = FALSE;
        crawler_sprite_middle_visible = FALSE;
        crawler_sprite_near_visible = FALSE;

        // Rotate the visible map to screen space
        solve_view();

        // Draw view to buffer area
        set_buffer = 1u;
        build_view();
        set_buffer = 0;

        // Update sprites
        get_sprite_visibility();

        // Minimap
        if (minimap)
        {
            if (old_minimap != minimap)
            {
                // if minimap is just appearing, draw border
                set_buffer = 1u;
                set_bkg_chunk(13u, 11u, 7u, 1u, 20u, 12u);
                set_buffer = 0;

                set_bkg_chunk(13u, 12u, 7u, 6u, 20u, 13u);                                         // border
                set_bkg_chunk(14u, 12u, 5u, 5u, (PLAYER.pos.x >> 7) - 2, (PLAYER.pos.y >> 7) - 2); // map
                set_bkg_chunk(16u, 14u, 1u, 1u, 23u, 15u);                                         // player icon
            }
            else
            {
                // otherwise just refresh the map and fix the top edge (avoid flickering)
                set_buffer = 1u;
                set_bkg_chunk(13u, 11u, 7u, 1u, 20u, 12u);
                set_buffer = 0;

                set_bkg_chunk(14u, 12u, 5u, 5u, (PLAYER.pos.x >> 7) - 2, (PLAYER.pos.y >> 7) - 2); // map
                set_bkg_chunk(16u, 14u, 1u, 1u, 23u, 15u);                                         // player icon
            }
        }
        else
        {
            if (old_minimap != minimap)
            {
                // clear away map border
                for (uint8_t i = 0; i < 7; i++)
                {
                    set_bkg_chunk(13u + i, 12u, 1u, 6u, 12u, 12u);
                }
                set_bkg_chunk(19u, 11u, 1u, 2u, 19u, 11u);
            }
        }
        old_minimap = minimap;

        // Copy offscreen to screen
        set_bkg_submap(1, 2u, 18u, 10u, vram_ptr, 32u); // 32u for VRAM width rather than image width
        // Fix minimap corner after buffer copy
        if (minimap)
            set_bkg_chunk(19u, 11u, 1u, 1u, 26u, 12u);

        //  Done
        if (!minimap && !in_combat && is_minimap_selected)
        {
            minimap = is_minimap_selected;
            // Do one more update
        }
        else
        {
            view_dirty = FALSE;
        }
    }
}
