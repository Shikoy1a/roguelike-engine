#pragma once
#include <vector>
#include <string>
#include "entity.hpp"

class Game {
public:
    Game();

    void render() const;

    void handleInput(char command, bool& running);

    void updateMonsters();

private:
    std::vector<std::string> map;
    std::vector<Entity> entities;

    void init();
};