#include "game.hpp"
#include "entity.hpp"
#include <iostream>
#include <cstdlib>   
#include <ctime>     
#include <algorithm> 
#include <cmath>

// ------- 控制台清屏 -------

static void clear_screen() {

    system("cls");

}

// ------- ANSI 颜色（简单彩色控制台） -------

static const char* COLOR_RESET   = "\x1b[0m";
static const char* COLOR_WALL    = "\x1b[37m"; // 白
static const char* COLOR_FLOOR   = "\x1b[90m"; // 暗灰
static const char* COLOR_PLAYER  = "\x1b[32m"; // 绿
static const char* COLOR_MONSTER = "\x1b[31m"; // 红
static const char* COLOR_CORPSE  = "\x1b[35m"; // 紫
static const char* COLOR_DARK    = "\x1b[90m"; // 暗灰

// ------- 一个简单的矩形房间结构，用于地牢生成 -------

struct Rect {
    int x, y, w, h;

    int centerX() const { return x + w / 2; }
    int centerY() const { return y + h / 2; }

    bool intersects(const Rect& other) const {
        return !(x + w <= other.x ||
                 other.x + other.w <= x ||
                 y + h <= other.y ||
                 other.y + other.h <= y);
    }
};

// ------- Bresenham 直线，用于 FoV -------

static std::vector<std::pair<int,int>> bresenhamLine(int x0, int y0, int x1, int y1) {
    std::vector<std::pair<int,int>> line;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int x = x0;
    int y = y0;

    while (true) {
        line.emplace_back(x, y);
        if (x == x1 && y == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }

    return line;
}

// ------- Game 成员函数实现 -------

Game::Game() {
    init();
}

void Game::init() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    generateDungeon();

    height = static_cast<int>(map.size());
    width  = static_cast<int>(map[0].size());

    visible.assign(height, std::vector<bool>(width, false));
    explored.assign(height, std::vector<bool>(width, false));

    logLines.clear();
    addLog("Welcome to the dungeon!");
}

void Game::generateDungeon() {
    const int mapW = 40;
    const int mapH = 20;
    const int maxRooms = 8;
    const int roomMinSize = 4;
    const int roomMaxSize = 8;

    map.assign(mapH, std::string(mapW, '#'));

    std::vector<Rect> rooms;

    for (int i = 0; i < maxRooms; ++i) {
        int w = roomMinSize + std::rand() % (roomMaxSize - roomMinSize + 1);
        int h = roomMinSize + std::rand() % (roomMaxSize - roomMinSize + 1);
        int x = 1 + std::rand() % (mapW - w - 1);
        int y = 1 + std::rand() % (mapH - h - 1);

        Rect newRoom{x, y, w, h};

        bool failed = false;
        for (const auto& other : rooms) {
            if (newRoom.intersects(other)) {
                failed = true;
                break;
            }
        }
        if (failed) continue;

        // 挖房间
        for (int ry = y; ry < y + h; ++ry) {
            for (int rx = x; rx < x + w; ++rx) {
                map[ry][rx] = '.';
            }
        }

        if (!rooms.empty()) {
            // 用走廊连接上一个房间
            int prevCx = rooms.back().centerX();
            int prevCy = rooms.back().centerY();
            int newCx  = newRoom.centerX();
            int newCy  = newRoom.centerY();

            if (std::rand() % 2) {
                // 先水平后垂直
                for (int tx = std::min(prevCx, newCx); tx <= std::max(prevCx, newCx); ++tx) {
                    map[prevCy][tx] = '.';
                }
                for (int ty = std::min(prevCy, newCy); ty <= std::max(prevCy, newCy); ++ty) {
                    map[ty][newCx] = '.';
                }
            } else {
                // 先垂直后水平
                for (int ty = std::min(prevCy, newCy); ty <= std::max(prevCy, newCy); ++ty) {
                    map[ty][prevCx] = '.';
                }
                for (int tx = std::min(prevCx, newCx); tx <= std::max(prevCx, newCx); ++tx) {
                    map[newCy][tx] = '.';
                }
            }
        }

        rooms.push_back(newRoom);
    }

    // 创建玩家和怪物
    entities.clear();

    if (!rooms.empty()) {
        // 玩家在第一个房间中心
        Rect& first = rooms[0];
        int px = first.centerX();
        int py = first.centerY();

        Entity player;
        player.x = px;
        player.y = py;
        player.glyph    = '@';
        player.blocks   = true;
        player.isPlayer = true;
        player.maxHp    = 30;
        player.hp       = 30;
        player.attack   = 6;
        entities.push_back(player);

        // 每个其他房间中心放一只怪物
        for (std::size_t i = 1; i < rooms.size(); ++i) {
            Rect& rm = rooms[i];
            int mx = rm.centerX();
            int my = rm.centerY();

            Entity m;
            m.x = mx;
            m.y = my;
            m.glyph    = (i % 2 == 0) ? 'g' : 'o';
            m.blocks   = true;
            m.isPlayer = false;
            m.maxHp    = 12;
            m.hp       = 12;
            m.attack   = 4;
            entities.push_back(m);
        }
    }

    height = mapH;
    width  = mapW;
}

// 视野计算（FoV）：从玩家出发，以半径 fovRadius 做线性 FoV
void Game::updateFov() {
    if (entities.empty()) return;
    const Entity& player = entities[0];

    // 清空可见性
    for (int y = 0; y < height; ++y) {
        std::fill(visible[y].begin(), visible[y].end(), false);
    }

    int px = player.x;
    int py = player.y;

    int r = fovRadius;
    int r2 = r * r;

    for (int y = py - r; y <= py + r; ++y) {
        for (int x = px - r; x <= px + r; ++x) {
            if (!in_bounds(map, x, y)) continue;

            int dx = x - px;
            int dy = y - py;
            if (dx*dx + dy*dy > r2) continue; // 圆形视野

            auto line = bresenhamLine(px, py, x, y);

            bool blocked = false;
            for (std::size_t i = 0; i < line.size(); ++i) {
                int lx = line[i].first;
                int ly = line[i].second;

                if (!in_bounds(map, lx, ly)) break;

                visible[ly][lx]  = true;
                explored[ly][lx] = true;

                if (map[ly][lx] == '#' && i + 1 < line.size()) {
                    blocked = true;
                    break;
                }
            }
            (void)blocked; // 此处我们不专门使用 blocked 变量
        }
    }
}

void Game::addLog(const std::string& msg) {
    logLines.push_back(msg);
    const std::size_t MAX_LINES = 6;
    if (logLines.size() > MAX_LINES) {
        logLines.erase(logLines.begin());
    }
}

void Game::render() const {
    clear_screen();

    // 重新计算 FoV
    // 注意：updateFov 修改成员，因此这里不能是 const 函数调用；
    // 我们在 const 函数外调用它，在 main 里控制调用顺序。
    // 这里为了简单起见，我们假装 render 之前刚刚调用了 updateFov。
    // 如果你想在 render 内部调用，需要把 updateFov 去掉 const 限制。

    int h = height;
    int w = width;

    // 画地图 + 实体
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            bool isExplored = explored[y][x];
            bool isVisible  = visible[y][x];

            if (!isExplored) {
                std::cout << ' ';
                continue;
            }

            char baseTile = map[y][x];
            char drawCh   = baseTile;

            const Entity* entToDraw = nullptr;

            if (isVisible) {
                for (const auto& e : entities) {
                    if (e.hp > 0 && e.x == x && e.y == y) {
                        entToDraw = &e;
                        break;
                    }
                }
            }

            const char* color = COLOR_FLOOR;

            if (!isVisible) {
                color = COLOR_DARK;
            } else if (entToDraw) {
                if (entToDraw->isPlayer)      color = COLOR_PLAYER;
                else if (entToDraw->hp <= 0)  color = COLOR_CORPSE;
                else                          color = COLOR_MONSTER;
                drawCh = entToDraw->glyph;
            } else {
                if (baseTile == '#') color = COLOR_WALL;
                else                 color = COLOR_FLOOR;
            }

            std::cout << color << drawCh << COLOR_RESET;
        }
        std::cout << '\n';
    }

    // HUD：玩家状态
    if (!entities.empty()) {
        const Entity& player = entities[0];
        std::cout << "HP: " << player.hp << " / " << player.maxHp << "\n";
    }

    // 日志输出
    std::cout << "---- Log ----\n";
    for (const auto& line : logLines) {
        std::cout << line << "\n";
    }
}

// 玩家输入处理（移动 / 攻击 / 退出）
void Game::handleInput(char command, bool& running) {
    if (entities.empty()) return;
    Entity& player = entities[0];

    // 转小写
    if (command >= 'A' && command <= 'Z') {
        command = command - 'A' + 'a';
    }

    int dx = 0, dy = 0;

    if (command == 'w') dy = -1;
    else if (command == 's') dy = 1;
    else if (command == 'a') dx = -1;
    else if (command == 'd') dx = 1;
    else if (command == 'q') {
        running = false;
        return;
    } else {
        // 其他按键忽略
        return;
    }

    if (dx == 0 && dy == 0) return;

    int targetX = player.x + dx;
    int targetY = player.y + dy;

    int monsterIndex = find_monster_at(entities, targetX, targetY);
    if (monsterIndex != -1) {
        // 攻击怪物
        Entity& m = entities[monsterIndex];
        m.hp -= player.attack;

        addLog("You hit " + std::string(1, m.glyph) +
               " for " + std::to_string(player.attack) +
               " damage (HP=" + std::to_string(m.hp) + ")");

        if (m.hp <= 0) {
            addLog(std::string("Monster ") + m.glyph + " dies!");
            m.blocks = false;
            m.glyph  = 'x'; // 尸体
        }
    } else {
        // 没有怪物，就尝试移动
        try_move_entity(player, map, entities, dx, dy);
    }

    // 玩家走完之后，重算一次视野
    updateFov();
}

// 怪物 AI：朝玩家靠近，如果要走到玩家位置就攻击
void Game::updateMonsters(bool& running) {
    Entity& player = entities[0];

    for (std::size_t i = 1; i < entities.size(); ++i) {
        Entity& monster = entities[i];
        if (monster.hp <= 0) continue; // 死了就不动

        // 1. 用 A* 计算从怪物到玩家的路径
        Path path = find_path(map,
                              monster.x, monster.y,
                              player.x,  player.y);

        // [0] = 当前怪物位置, [1] = 下一步, ..., [N-1] = 玩家位置
        if (path.size() >= 2) {
            int nextX = path[1].first;
            int nextY = path[1].second;

            // 2. 如果下一步就是玩家所在的格子 → 攻击玩家
            if (nextX == player.x && nextY == player.y) {
                player.hp -= monster.attack;
                addLog("Monster " + std::string(1, monster.glyph) +
                       " hits you for " + std::to_string(monster.attack) +
                       " damage! (HP = " + std::to_string(player.hp) + ")");

                if (player.hp <= 0) {
                    addLog("You died!");
                    running = false;
                    return;
                }
            } else {
                // 3. 否则尝试向该格子移动（考虑其他怪物/墙的阻挡）
                int dx = nextX - monster.x;
                int dy = nextY - monster.y;
                try_move_entity(monster, map, entities, dx, dy);
            }
        } else {
            // 没有路径（被墙完全隔开），可以退回到原来的“贪心靠近”或者原地不动
            // 这里我让怪物原地不动，你也可以改成简单追踪逻辑
            // （例如你之前的那套 dx/dy = sign(player - monster)）
        }
    }

    // 怪物行动后重新计算视野
    updateFov();
}