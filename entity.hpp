#pragma once
#include <vector>
#include <string>

// 实体（玩家、怪物等）
struct Entity {
    int x;
    int y;
    char glyph;   // 显示用字符，比如 '@', 'g'
    bool blocks;  // 是否阻挡移动
    bool isPlayer;
};

bool is_blocked(const std::vector<std::string>& map, 
                const std::vector<Entity>& entities, 
                int x, int y);

void try_move_entity(Entity& e, const std::vector<std::string>& map,
                    const std::vector<Entity>& entities,
                    int dx, int dy);