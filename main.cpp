#include "Game.h"


int main()
{
    Game game;  
    while (game.getWindow().isOpen()) {
        game.PollEvents();
        game.update();
        game.render();
    }
    
    return 0;
}

