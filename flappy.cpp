#include <iostream>

const int width = 40;
const int height = 10;
const int pipeOffset = 10;

enum Sprite {
    FLAPPY, PIPE, EMPTY
};

int random(int min, int max){
    int range = (max-1) - min + 1;
    return rand() % range + min;
}

struct Pipe {
    int height;
    int pos;
};

int playerY;
bool playerDead;
Sprite** stage;
Pipe startPipe;




Pipe getNextPipe(int unboundedPipeHeight, int pos){
    unboundedPipeHeight = (unboundedPipeHeight ^ 13) + 1;
    unboundedPipeHeight = (unboundedPipeHeight ^ 17) + 1;
    unboundedPipeHeight = (unboundedPipeHeight ^ 5) + 1;

    Pipe result{};
    result.height=unboundedPipeHeight;
    result.pos=pos+pipeOffset;
    return result;
}

void displayPipe(int pos, int unboundedPipeHeight){
    int realPipeHeight = (unboundedPipeHeight + 3) % height;
    for (int i = 0; i < height; ++i) {
        if (pos + 1 < width) {
            stage[i][pos+1] = EMPTY;
        }
        if (i < realPipeHeight || i > realPipeHeight + 2) {
            stage[i][pos] = PIPE;
        }
    }
}

void movePlayer(int delta){
    if (startPipe.pos == 0) {
        // Prevent clumping of pipes at the beginning of the canvas
        for (int i = 0; i < height; ++i) {
            stage[i][0] = EMPTY;
        }

        Pipe nextStartPipe = getNextPipe(startPipe.height,startPipe.pos);
        startPipe.height = nextStartPipe.height;
        startPipe.pos = nextStartPipe.pos;
    }

    startPipe.pos -= 1;

    int nextHeight = startPipe.height;
    int nextPos = startPipe.pos;
    for (int i = startPipe.pos; i < width ; i+=pipeOffset) {
        displayPipe(nextPos,nextHeight);
        Pipe nextPipe = getNextPipe(nextHeight,nextPos);
        nextHeight = nextPipe.height;
        nextPos = nextPipe.pos;
    }

    playerY += delta;

    // Check if the player has hit a pipe
    if (stage[playerY][0] == PIPE) {
        playerDead = true;
    }

    // Remove the last frames bird from the grid and draw the new framed bird
    for (int i = 0; i < height; ++i) {
        if (stage[i][0] == FLAPPY) {
            stage[i][0] = EMPTY;
        }
    }
    stage[playerY][0] = FLAPPY;

    // Display stage
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            Sprite val = stage[i][j];
            switch (val) {
                case FLAPPY:
                    std::cout << ">";
                    break;
                case PIPE:
                    std::cout << "|";
                    break;
                case EMPTY:
                    std::cout << " ";
                    break;
            }
        }
        std::cout << std::endl;
    }
}


int main() {
    // Declare stage
    stage = new Sprite*[height];
    for (int i = 0; i < height; i++) {
        stage[i] = new Sprite[width];
    }

    // Initialise stage
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            stage[i][j] = EMPTY;
        }
    }

    playerY = height - 1;

    playerDead = false;
    bool playerJumped = false;

    // Must be an even number to avoid cancelling out any of the xors in the xorshift
    startPipe.height = random(0,height/2)*2;
    startPipe.pos = pipeOffset;


    // GAME LOGIC
    while (true) {
        movePlayer(0);

        while (!playerDead) {

            std::string input = "a";
            char *cStyleInput = const_cast<char *>(input.c_str());

            std::cout << ": ";
            std::cin.getline(cStyleInput,2);
            input = std::string(cStyleInput, input.size());

            if (input == " ") {
                playerJumped = true;
            } else {
                playerJumped = false;
            }

            if (playerJumped) {
                if (playerY > 0) {
                    movePlayer(-1);
                } else {
                    movePlayer(0);
                }
            } else {
                if (playerY < height - 1) {
                    movePlayer(1);
                } else {
                    movePlayer(0);
                }
            }
        }
        std::cout << std::endl;

        std::cout << std::endl << "You Are Dead!!! Press Enter To Continue";


        std::string input = "a";
        char *cStyleInput = const_cast<char *>(input.c_str());
        std::cin.getline(cStyleInput,2);


        std::cout << std::endl;
        playerDead = false;
    }
    return 0;
}