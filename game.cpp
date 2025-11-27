#include "game.hpp"
#include <iostream>
#include <cstdlib> 

static void clear_screen() {
    system("cls");
}

Game::Game() {
    init();
}

void Game::init() {
    // 初始化地图
    map = {
        "##########",
        "#........#",
        "#..####..#",
        "#..#..#..#",
        "#..####..#",
        "#........#",
        "##########"
    };

    entities.clear();

    // 玩家
    Entity player;
    player.x = 1;
    player.y = 1;
    player.glyph    = '@';
    player.blocks   = true;
    player.isPlayer = true;
    entities.push_back(player);

    // 怪物1
    Entity monster1;
    monster1.x = 7;
    monster1.y = 3;
    monster1.glyph    = 'g';
    monster1.blocks   = true;
    monster1.isPlayer = false;
    entities.push_back(monster1);

    // 怪物2
    Entity monster2;
    monster2.x = 7;
    monster2.y = 5;
    monster2.glyph    = 'o';
    monster2.blocks   = true;
    monster2.isPlayer = false;
    entities.push_back(monster2);
}

// 渲染地图 + 实体 + 状态
void Game::render() const {
    clear_screen();

    int height = static_cast<int>(map.size());
    int width  = static_cast<int>(map[0].size());

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char ch = map[y][x];

            // 有实体覆盖
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

    const Entity& player = entities[0];
    std::cout << "\nPlayer (@) at (" << player.x << ", " << player.y << ")\n";

    for (std::size_t i = 1; i < entities.size(); ++i) {
        const auto& m = entities[i];
        std::cout << "Monster " << m.glyph << " at (" << m.x << ", " << m.y << ")\n";
    }

    std::cout << "\nControls: WASD move, Q quit.\n";
}

void Game::handleInput(char command, bool& running) {
    // 转小写
    if (command >= 'A' && command <= 'Z') {
        command = command - 'A' + 'a';
    }

    Entity& player = entities[0];
    int dx = 0, dy = 0;

    if (command == 'w') dy = -1;
    else if (command == 's') dy = 1;
    else if (command == 'a') dx = -1;
    else if (command == 'd') dx = 1;
    else if (command == 'q') {
        running = false;
    }

    if (running && (dx != 0 || dy != 0)) {
        try_move_entity(player, map, entities, dx, dy);
    }
}

void Game::updateMonsters() {
    const Entity& player = entities[0];

    for (std::size_t i = 1; i < entities.size(); ++i) {
        Entity& monster = entities[i];

        int dx = 0;
        int dy = 0;

        if (monster.x < player.x) dx = 1;
        else if (monster.x > player.x) dx = -1;

        if (monster.y < player.y) dy = 1;
        else if (monster.y > player.y) dy = -1;

        if (dx != 0) {
            try_move_entity(monster, map, entities, dx, 0);
        }
        if (dy != 0) {
            try_move_entity(monster, map, entities, 0, dy);
        }
    }
}