#pragma once
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <Windows.h>
#include <mutex>

/// @brief Used to store all of our game system information - easier to access in different threads 
/// DOESN'T NEED TO BE USED FOR ANYTHING ELSE EXCEPT THREADED INPUT!!!!
static class DLLGameSys{

    public:
    inline static FILE* inp;



    DLLGameSys() {
        inp = new FILE();
    };

    ~DLLGameSys() {
        delete inp;
    };
};

enum Command : int32_t{
    SET_HEATH,
    GET_HEALTH,

};


class InputServer{

    private:

    DLLGameSys gameSys;

    

    public:
    InputServer(){};
    void start();
    void loop();
    std::vector<std::string> sanatiseText(std::string text);
    bool isValidCommand();

};