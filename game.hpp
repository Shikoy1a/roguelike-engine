#pragma once
#include <vector>
#include <string>
#include "entity.hpp"
#include "pathfinding.hpp"

class Game {
public:
    Game();

    void render() const;                 // 渲染地图 + 实体 + HUD + 日志
    void handleInput(char command, bool& running); // 处理玩家输入（移动/攻击/退出）
    void updateMonsters(bool& running);  // 更新怪物 AI，可能打死玩家

private:
    std::vector<std::string> map;    // 地图：# 墙, . 地板
    int width = 0;
    int height = 0;

    std::vector<Entity> entities;   

    // 视野与探索
    std::vector<std::vector<bool>> visible;
    std::vector<std::vector<bool>> explored;
    int fovRadius = 8;

    // 简单日志系统
    std::vector<std::string> logLines;

    void init();                     // 初始化整个游戏（调用地牢生成等）
    void generateDungeon();          // 程序化地牢生成
    void updateFov();                // 计算 FoV
    void addLog(const std::string&); // 向日志里添加一条信息
};
