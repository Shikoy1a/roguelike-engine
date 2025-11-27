#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>  // system, rand
#include <ctime>    // time

#ifdef _WIN32
#include <conio.h>  // _getch()
#endif

// 实体（玩家、怪物等）
struct Entity {
    int x;
    int y;
    char glyph;   // 显示用字符，比如 '@', 'g'
    bool blocks;  // 是否阻挡移动
    bool isPlayer;
};

// 清屏函数
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

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

// 渲染地图 + 所有实体 + 状态信息
void render(const std::vector<std::string>& map,
            const std::vector<Entity>& entities) {
    clear_screen();

    int height = static_cast<int>(map.size());
    int width  = static_cast<int>(map[0].size());

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char ch = map[y][x];

            // 有实体的话用实体覆盖
            for (const auto& e : entities) {
                if (e.x == x && e.y == y) {
                    ch = e.glyph;
                    break;
                }
            }
            std::cout << ch;
        }
        std::cout << '\n';
    }

    // 画完地图，在下面显示一些调试信息
    const Entity& player = entities[0];
    std::cout << "\nPlayer (@) at (" << player.x << ", " << player.y << ")\n";

    for (std::size_t i = 1; i < entities.size(); ++i) {
        const auto& m = entities[i];
        std::cout << "Monster " << m.glyph << " at (" << m.x << ", " << m.y << ")\n";
    }

    std::cout << "\nControls: WASD to move, Q to quit.\n";
}

// Windows 下的“立即读取一个按键”（不需要回车）
char get_input() {
#ifdef _WIN32
    int ch = _getch(); // 不会回显，也不需要回车
    // 如果你以后用方向键，可以在这里多处理一次 0 / 224 的情况
    return static_cast<char>(ch);
#else
    char c;
    std::cin >> c;
    return c;
#endif
}

// 简单怪物 AI：朝玩家靠近
void update_monsters(const std::vector<std::string>& map,
                     std::vector<Entity>& entities) {
    const Entity& player = entities[0];

    for (std::size_t i = 1; i < entities.size(); ++i) {
        Entity& monster = entities[i];

        int dx = 0;
        int dy = 0;

        if (monster.x < player.x) dx = 1;
        else if (monster.x > player.x) dx = -1;

        if (monster.y < player.y) dy = 1;
        else if (monster.y > player.y) dy = -1;

        // 先尝试水平移动
        if (dx != 0) {
            try_move_entity(monster, map, entities, dx, 0);
        }
        // 再尝试垂直移动
        if (dy != 0) {
            try_move_entity(monster, map, entities, 0, dy);
        }
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // 地图
    std::vector<std::string> map = {
        "##########",
        "#........#",
        "#..####..#",
        "#..#..#..#",
        "#..####..#",
        "#........#",
        "##########"
    };

    std::vector<Entity> entities;

    // 玩家
    Entity player;
    player.x = 1;
    player.y = 1;
    player.glyph   = '@';
    player.blocks  = true;
    player.isPlayer = true;
    entities.push_back(player);

    // 怪物1
    Entity monster1;
    monster1.x = 7;
    monster1.y = 3;
    monster1.glyph   = 'g';
    monster1.blocks  = true;
    monster1.isPlayer = false;
    entities.push_back(monster1);

    // 怪物2
    Entity monster2;
    monster2.x = 7;
    monster2.y = 5;
    monster2.glyph   = 'o';
    monster2.blocks  = true;
    monster2.isPlayer = false;
    entities.push_back(monster2);

    bool running = true;

    while (running) {
        // 1. 渲染
        render(map, entities);

        // 2. 读取按键（不用回车）
        char command = get_input();

        // 转小写
        if (command >= 'A' && command <= 'Z') {
            command = command - 'A' + 'a';
        }

        // 3. 控制玩家移动
        Entity& playerRef = entities[0];
        int dx = 0, dy = 0;

        if (command == 'w') dy = -1;
        else if (command == 's') dy = 1;
        else if (command == 'a') dx = -1;
        else if (command == 'd') dx = 1;
        else if (command == 'q') {
            running = false;
        }

        if (dx != 0 || dy != 0) {
            try_move_entity(playerRef, map, entities, dx, dy);
        }

        // 4. 更新怪物（会向玩家靠近）
        if (running) {
            update_monsters(map, entities);
        }
    }

    std::cout << "Game over!" << std::endl;
    return 0;
}
