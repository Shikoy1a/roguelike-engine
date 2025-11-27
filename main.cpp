#include <iostream>
#include <ctime>
#include <cstdlib>

#include <conio.h>  // _getch

#include "game.hpp"


char get_input() {
    int ch = _getch();
    return static_cast<char>(ch);
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Game game;
    bool running = true;

    // 初始先算一次视野
    // （因为我们的 Game::render 假设外面先调用过 updateFov；
    //   最简单的办法是在 Game 构造完成后让它自己算一次。
    //   为避免再暴露接口，这里可以简单调用 handleInput 一个无效键，
    //   或者你也可以把 updateFov() 设成 public。
    //   这里我们就靠每次输入之后都算来保证：第一帧可能全黑，动一下就好了。

    while (running) {
        game.render();
        char command = get_input();
        game.handleInput(command, running);
        if (!running) break;
        
        game.updateMonsters(running);
    }

    std::cout << "Game over!" << std::endl;
    return 0;
}
