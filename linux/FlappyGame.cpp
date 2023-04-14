#include "FlappyGame.h"
#include <string>

FlappyGame::FlappyGame(ClientWorld* world, Player* player) : _world(world), localPlayer(player){ InitialiseLocalGame(); };


void FlappyGame::InitialiseLocalGame(){
    
    #ifdef USE_FAST_TRAVEL
    try{
    _world->FastTravel(localPlayer, std::string("lol"), std::string("lol"));
    }
    catch(std::exception& e){

    }
    #else
    localPlayer->SetPosition(Vector3(10.f,10.f,10.f));
    #endif
}

