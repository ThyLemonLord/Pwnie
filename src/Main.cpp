#include "Main.h"

using namespace std;

mutex MUTEX_LOCK;



void InputServer::start(){
    // MUTEX_LOCK.lock();
    //     AllocConsole();
    //     freopen_s(&gameSys.inp, "CONOUT$", "w", stdout);
    //     freopen_s(&gameSys.inp, "CONIN$", "r", stdin);
        cout << "Starting Input Services" << endl;
    // MUTEX_LOCK.unlock();
    
    loop();
};

void InputServer::loop(){

    for(;;){
        string cmd = NULL;
        cout.write("Enter a command here!! : ", sizeof("Enter a command here!! : "));
        //cin >> cmd;


    }

};