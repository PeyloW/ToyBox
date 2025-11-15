//
//  tilemap_scene.cpp
//  toybox
//
//  Created by Fredrik on 2025-11-12.
//

#include "tilemap_scene.hpp"
#include "demo_assets.hpp"

static constexpr uint16_t is_target = 1 << 0;
enum entity_type {
    PLAYER = 0,
    BOX = 1
};

enum tile_type {
    EMPTY = 0,  // Color #0
    WALL = 1,   // Tile  #1
    FLOOR = -7, // Color #7
    TARGET = -9 // Color #9
};

static void player_control(tilemap_level_c& level, entity_s& entity) {
}

tilemap_level_c* make_tilemaplevel() {
    static constexpr const char *recipe[] = {
        "    #####          ",
        "    #---#          ",
        "    #$--#          ",
        "  ###--$##         ",
        "  #--$-$-#         ",
        "###-#-##-#   ######",
        "#---#-##-#####--..#",
        "#-$--$----------..#",
        "#####-###-#@##--..#",
        "    #-----#########",
        "    #######        ",
    };
    auto level_ptr = new tilemap_level_c(rect_s(point_s(), size_s(19,11)), nullptr);
    auto& level = *level_ptr;
    
    // Setup available actions
    level.actions().emplace_back(&actions::idle);
    level.actions().emplace_back(&player_control);
    
    // Setup entity type defs:
    auto& player = level.entity_type_defs().emplace_back();
    player.tileset = &asset_manager_c::shared().tileset(TILESET_SPR);
    player.frame_defs.push_back({ 0, {-8, -8} });
    auto& box = level.entity_type_defs().emplace_back();
    box.tileset = &asset_manager_c::shared().tileset(TILESET_SPR);
    box.frame_defs.push_back({ 1, {-8, -8} });

    for (int y = 0; y < 11; ++y) {
        const char* line = recipe[y];
        for (int x = 0; x < 19; x++) {
            auto& tile = level[x,y];
            auto center = [&]() {
                return fpoint_s(x * 16 + 8, y * 16 + 8);
            };
            switch (line[x]) {
                case ' ':
                    tile.index = EMPTY;
                    break;;
                case '#':
                    tile.index = WALL;
                    tile.type = tile_s::solid;
                    break;
                case '-':
                    tile.flags = FLOOR;
                    break;
                case '.':
                    tile.index = TARGET;
                    tile.flags = is_target;
                    break;
                case '@':
                    tile.flags = 1;
                    level.all_entities().emplace(level.all_entities().begin(), (entity_s){
                        .type=PLAYER, .group=PLAYER,
                        .action = 1,
                        .position=fcrect_s{ center(), {16,16} }
                    });
                    break;
                case '$':
                    tile.flags = 1;
                    level.all_entities().emplace_back((entity_s){
                        .type=BOX, .group=BOX,
                        .position=fcrect_s{ center(), {16,16} }
                    });
                    break;
                default:
                    break;
            }
        }
    }
    
    return level_ptr;
}


tilemap_scene::tilemap_scene() :
    _level(asset_manager_c::shared().tilemap_level(LEVEL))
{
}

scene_c::configuration_s& tilemap_scene::configuration() const {
    static scene_c::configuration_s config{size_s(320, 208), 2, true};
    return config;
}

void tilemap_scene::will_appear(bool obsured) {
}

void tilemap_scene::update(display_list_c& display, int ticks) {
    auto& viewport = display.get(PRIMARY_VIEWPORT).viewport();
    _level.update(viewport, PRIMARY_VIEWPORT, ticks);
}
