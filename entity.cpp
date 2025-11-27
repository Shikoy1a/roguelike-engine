#include "entity.hpp"

// 地图是否在范围内
bool in_bounds(const std::vector<std::string>& map, int x, int y) {
    int height = static_cast<int>(map.size());
    int width  = static_cast<int>(map[0].size());
    return x >= 0 && x < width && y >= 0 && y < height;
}

// 地图该格子是否是地板（不考虑实体）
bool is_walkable_tile(const std::vector<std::string>& map, int x, int y) {
    if (!in_bounds(map, x, y)) return false;
    char tile = map[y][x];
    return tile == '.';
}

// 目标格子是否被阻挡（考虑地图 + 实体）
bool is_blocked(const std::vector<std::string>& map,
                const std::vector<Entity>& entities,
                int x, int y) {
    // 1) 看地图是不是地板
    if (!is_walkable_tile(map, x, y)) {
        return true;
    }

    // 2) 看有没有阻挡型实体
    for (const auto& e : entities) {
        if (e.blocks && e.x == x && e.y == y) {
            return true;
        }
    }
    return false;
}

// 尝试移动某个实体
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