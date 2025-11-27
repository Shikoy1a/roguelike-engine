#include <iostream>
#include <ctime>
#include <cstdlib>
#include <conio.h>  // _getch

#include "game.hpp"

char get_input() {
    int ch = _getch();             // 不用回车
    return static_cast<char>(ch);
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Game game;
    bool running = true;

    while (running) {
        game.render();
        char command = get_input();
        game.handleInput(command, running);
        if (running) {
            game.updateMonsters();
        }
    }

    std::cout << "Game over!" << std::endl;
    return 0;
}