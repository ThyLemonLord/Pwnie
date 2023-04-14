#pragma once
#include "libGameLogic.h"
#include <vector>

//Use this for the flappy game implementation? - might make things easier to read...

class FlappyGame {

    public:

    FlappyGame(ClientWorld* , Player* );
    ~FlappyGame();

    private:
    
    void InitialiseLocalGame();// Set up the scene thats all

    protected:

    Player* localPlayer = nullptr;
    ClientWorld* _world = nullptr;
    std::vector<IActor*> enemies; 

};