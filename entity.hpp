#pragma once
#include <vector>
#include <string>

// 游戏里的实体：玩家、怪物、尸体等
struct Entity {
    int x;
    int y;
    char glyph;   // 显示用字符，比如 '@', 'g', 'x'
    bool blocks;  // 是否阻挡（活着的怪物和玩家阻挡，尸体可以不阻挡）
    bool isPlayer;

    int hp;
    int maxHp;
    int attack;
};

// 地图/位置相关工具函数声明
bool in_bounds(const std::vector<std::string>& map, int x, int y);
bool is_walkable_tile(const std::vector<std::string>& map, int x, int y);
bool is_blocked(const std::vector<std::string>& map,
                const std::vector<Entity>& entities,
                int x, int y);

void try_move_entity(Entity& e,
                     const std::vector<std::string>& map,
                     const std::vector<Entity>& entities,
                     int dx, int dy);

// 在 (x, y) 找一只怪物（不包括玩家），返回下标，没有返回 -1
int find_monster_at(const std::vector<Entity>& entities, int x, int y);
