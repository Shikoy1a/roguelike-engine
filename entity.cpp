#include "entity.hpp"

// 地图范围判断
bool in_bounds(const std::vector<std::string>& map, int x, int y) {
    int height = static_cast<int>(map.size());
    int width  = static_cast<int>(map[0].size());
    return x >= 0 && x < width && y >= 0 && y < height;
}

// 地板判断（不考虑实体，只看地图）
bool is_walkable_tile(const std::vector<std::string>& map, int x, int y) {
    if (!in_bounds(map, x, y)) return false;
    char tile = map[y][x];
    return tile == '.';
}

// 目标格子是否被阻挡（地图 + 实体）
bool is_blocked(const std::vector<std::string>& map,
                const std::vector<Entity>& entities,
                int x, int y) {
    // 1) 先看地图是不是地板
    if (!is_walkable_tile(map, x, y)) {
        return true;
    }

    // 2) 再看有没有阻挡型实体
    for (const auto& e : entities) {
        if (e.blocks && e.x == x && e.y == y && e.hp > 0) {
            return true;
        }
    }
    return false;
}

// 尝试移动某个实体（不能穿墙、不能穿过 blocks=true 的实体）
void try_move_entity(Entity& e,
                     const std::vector<std::string>& map,
                     const std::vector<Entity>& entities,
                     int dx, int dy) {
    int newX = e.x + dx;
    int newY = e.y + dy;

    if (!is_blocked(map, entities, newX, newY)) {
        e.x = newX;
        e.y = newY;
    }
}

// 在 (x, y) 找一只怪物（不包括玩家），返回下标，没有返回 -1
int find_monster_at(const std::vector<Entity>& entities, int x, int y) {
    for (std::size_t i = 1; i < entities.size(); ++i) { // 0 是玩家
        const auto& e = entities[i];
        if (!e.isPlayer && e.x == x && e.y == y && e.hp > 0) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
