#include <dlfcn.h>
#include <set>
#include <map>
#include <functional>
#include <string>
#include <vector>
#include "libGameLogic.h" // <= this is the header for the target program that was reconstructed using gdb 
#include <set>
#include <sstream>
#include <cmath>
#include <cstring>
#include <iostream>
//hmmmmmmmmmmmmmm
//#include <unistd.h>
//#include <sys/types.h>
//#include <pwd.h>
//hmmmmmmmmmmmmmm

// Compile this code with:
// g++ speedBoost.cpp -shared -fPIC -o speedBoost.so
//
//To run copy the so file to the game dir and run:
// LD_PRELOAD=speedBoost.so ./PwnAdventure3-Linux-Shipping

std::map<unsigned int, ActorRef<IActor>> actorsByID;
std::vector<std::tuple<Actor*, Vector3>> actorList;
bool flappyTime = false;
bool angryTime = false;
int  flappyTick = 0;
Vector3 flappyPos;
// -------------------------FLAPPY-------------------------
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
bool playerJumped;
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
    {
		// GET ALL BEARS OUT OF HERE
		int bearCounter = 0;
		Vector3 playerPos = flappyPos;
		Vector3 base = Vector3(playerPos.x - 2000, playerPos.y + 2000, playerPos.z + 10000);
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				Sprite val = stage[y][x];
				switch (val) {
					case FLAPPY:
					case PIPE:
						std::get<1>(actorList[bearCounter]).x = base.x + (x*100);
						std::get<1>(actorList[bearCounter]).y = (base.y + (-(y*400)));
						bearCounter++;
						break;
					case EMPTY:
						break;
				}
			}
		}
	}
}


int initFlappy(){
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
    playerJumped = false;

    // Must be an even number to avoid cancelling out any of the xors in the xorshift
    startPipe.height = random(0,height/2)*2;
    startPipe.pos = pipeOffset;
}

int flappy() {
    // GAME LOGIC
	if(playerDead){
		flappyTime = false;
	} else {
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

		playerJumped = false;
	}
    return 0;
}
// -------------------------FLAPPY-------------------------


// This method will be called everytime the user jumps (presses space).
void Player::SetJumpState(bool b) {
	playerJumped = true;

    // load a pointer to the GameWorld object:
    ClientWorld* world = *((ClientWorld**)(dlsym(RTLD_NEXT, "GameWorld")));
    std::set<ActorRef<IPlayer>> players = world->m_players;
    // yoink the local player from the world 
    ILocalPlayer* localPlayer = world->m_localPlayer;
    Vector3 pos = localPlayer->GetPosition();
	Rotation rot = GetLookRotation();

	std::cout << "Pitch: " << rot.pitch;
	std::cout << "Yaw: " << rot.yaw;
	std::cout << "Roll: " << rot.roll;

    // print some data to the console that was used to start the program
    printf("localPlayer pos: %f %f %f", pos.x, pos.y, pos.z);
    const Vector3 newPos = Vector3(pos.x,pos.y,pos.z+10);

    localPlayer->SetPosition(newPos);
    //Chat("blink"); // ---------------------------------------------------------- swapped these lines so space = blink

    printf("localPlayer pos: %f %f %f", pos.x, pos.y, pos.z);
    // We now need to call the orginal Set Jump State method, otherwise the server
    // (and other players) will not see us jump. Here is a pointer we will set to
    // that function:
    void (*realSetJumpState)(Player*,bool);

    //To find the address of the real function we need to find its  orginal address
    //For this we need the "mangled" name, which we found open the libGameLogic.so 
    //file in IDA looking at the exprots and right clicking and deselecting "demangle"
    realSetJumpState =(void (*)(Player*,bool))dlsym(RTLD_NEXT,"_ZN6Player12SetJumpStateEb");

    printf("realSetJumpState is at: %p\n", realSetJumpState);
    realSetJumpState(this,b); 
}

void Player::Chat(const char* message)
{
    if (message!=NULL || std::string(message) != "")
    {
        printf("\n\tmessage: %s", message);

        // grab the game world
        ClientWorld* world = *((ClientWorld**)(dlsym(RTLD_NEXT, "GameWorld")));
        // grab the player interface
        IPlayer* iplayer = world->m_activePlayer.m_object;
        // grab the player
        Player* player = ((Player*)(iplayer));
        // grab the local player interface
        ILocalPlayer* localPlayer = world->m_localPlayer;
        Vector3 pos = localPlayer->GetPosition();

        // split the chat input into command and parameter(s)
        std::vector<std::string> split;
        std::stringstream ss(message);
        std::string s;
        while (std::getline(ss, s, ' ')) {
            split.push_back(s);
        }
		std::string args = "";
		for (int i = 1; i < split.size(); i++) {
        	args = args + split[i] + " ";
            //printf("\nargs: %s", args);
        }
		args = args.substr(0,args.length()-1);
        printf("\nargs: %s", args);

        // setHealth function
        if (split[0] == "setHealth") 
        {
           printf("\ngot command setHealth...");
            // TODO add try catch for invalid input
            int newHealth = stoi(split[1]);
            // set health to newHealth
            //int curHealth = iplayer->GetHealth();
            //printf("current health: %d", curHealth);
            printf("\nsetting health to: %d", newHealth);
            //actor->PerformSetHealth(newHealth);
            printf("\nGetHealth() = %d", GetHealth());
            PerformSetHealth(newHealth);
            //printf("\ntestActor m_health: %d", testActor->m_health);
            //testActor->PerformSetHealth(newHealth);
            //printf("\nsetHealth done!");
        } 
        // setMana function
        else if (split[0] == "setMana") 
        {
            printf("\ngot command setMana");
            // TODO add try catch for invalid input
            int newMana = stoi(split[1]);
            // set health to newHealth
            //int curMana = iplayer->GetMana();
            //printf("current mana: %d", curMana);
            printf("\nsetting mana to: %d", newMana);
            //actor->PerformSetHealth(newHealth);
            this->m_mana = newMana;
        } 
        // tp funciton 
        else if (split[0] == "teleport" || split[0] == "tp") {
			float newX;
			float newY;
			float newZ;

			bool failed = false;
            // some handly locations around the world 
			if (split[1] == "fireball") {
				newX = -43651.6f;
				newY = -55911.0f;
				newZ = 324.5f;
			} else if (split[1] == "ground") {
				newX = 0.0f;
				newY = 0.0f;
				newZ = 10000.0f;
			} else if (split[1] == "bears") {
				newX = -7000.0f;
				newY = 64000.0f;
				newZ = 3000.0f;
			} else if (split[1] == "cows") {
				newX = 259398.0f;
				newY = -251983.0f;
				newZ = 1707.0f;
			} else if (split[1] == "pirate") {
				newX = 45587.0f;
				newY = 58554.0f;
				newZ = 534.0f;
			} else {
                // sets our new coords to be those provided
				if (split.size() == 4) {
					newX = stof(split[1]);
					newY = stof(split[2]);
					newZ = stof(split[3]);
				} else {
					failed = true;
				}
			}

			if (failed){
				printf("\nTeleport failed: INVALID NUM OF ARGS - expected 4, received %d", (int)split.size());
			} else {
				Vector3 test = m_remotePosition;
				printf("\noldPos: %f %f %f", test.x, test.y, test.z);
				const Vector3* newPos = new Vector3(newX, newY, newZ);
				printf("\nnewPos: %f %f %f", newPos->x, newPos->y, newPos->z);
				// tp our player using the actor function SetPosition
				this->SetPosition(*newPos);
				Vector3 check = this->GetPosition();
				printf("\nchecking new pos: %f, %f, %f", check.x, check.y, check.z);
			}
        } 
        // realtive teleport 
        else if (split[0] == "tpr") 
        {
            // grab our current position 
            Vector3 new_pos = this->GetPosition();
		    Vector3* offset = new Vector3();
            // load the arguments into the placeholder coords 
		    sscanf(message + 4, "%f %f %f", &(offset->x), &(offset->y), &(offset->z));
            // add offset coords to current position 
		    new_pos.x +=  offset->x;
		    new_pos.y +=  offset->y;
		    new_pos.z +=  offset->z;
            // perform the teleport 
		    this->SetPosition(new_pos);

        }
        // blink is buggy but feel free to look at what we tried :) 
         else if (split[0] == "blink") 
         {
             // standard blink is 100 units 
            float blinkDist = 1000;
            printf("\ngot command blink...");
            Rotation lookRot = this->GetLookRotation();
            printf("\nlook rotation: %f %f %f", lookRot.pitch, lookRot.yaw, lookRot.roll);
            Vector3* lookDir;
            float x = sin(lookRot.yaw);
            float y = (sin(lookRot.pitch)*cos(lookRot.yaw))*-1;
            float z = (cos(lookRot.pitch)*cos(lookRot.yaw))*-1;
            lookDir = new Vector3(x,y,z);
            printf("\nlook position: %f %f %f", x, y, z);
            float mult = blinkDist / lookDir->Magnitude();
            printf("\n%f / %f = %f", blinkDist, lookDir->Magnitude(), mult);
            printf("\n(%f %f %f) * %f", lookDir->x, lookDir->y, lookDir->z, mult);
            Vector3* posChange = lookDir;
            posChange->x = lookDir->x * mult;
            posChange->y = lookDir->y * mult;
            posChange->z = lookDir->z * mult;
            printf("\noldPos = %f %f %f", pos.x, pos.y, pos.z);
            printf("\nposChange = %f %f %f", posChange->x, posChange->y, posChange->z);
            Vector3 newPos = pos;
            newPos.x = newPos.x + posChange->x;
            newPos.y = newPos.y + posChange->y;
            newPos.z = newPos.z + posChange->z;
            printf("\nnewPos = %f %f %f", newPos.x, newPos.y, newPos.z);
            this->SetPosition(newPos);
        } 
        // set player walkspeed 
        else if (split[0] == "setMovementSpeed" || split[0] == "setSpeed") 
        {
            printf("\ngot command setMovementSpeed");
            printf("\ncurrent walking speed: %f", m_walkingSpeed);
            // grabs movement speed specified 
            float newMovementSpeed = stof(split[1]);
            printf("\nsetting movement speed to: %f", newMovementSpeed);
            // update speed 
            m_walkingSpeed = newMovementSpeed;
        } 
        else if (split[0] == "listInventory") 
        {
            printf("\ngot command listInventory");
            try
            {
                int count = m_inventory.size();
                printf("\nthere are %d items in the inventory", count);
                std::map<IItem*, ItemAndCount, std::less<IItem*>, std::allocator<std::pair<IItem* const, ItemAndCount> > >::iterator itr;
                // iterate through the inventory and print the name of each item
                for (itr = m_inventory.begin(); itr != m_inventory.end(); ++itr) {
                    printf("\nitem: %s", itr->first->GetName());
                }
            } catch (...)
            {
                printf("\nfailed to get inventory!");
            }
        } 
        // item duplicator 
        else if (split[0] == "dupeItem") 
        {
            printf("\ngot command dupeItem");
            try {
                int numTimes = stoi(split[2]);
                printf("\ntrying to dupe item: %s %d times", split[1], numTimes);
                int count = m_inventory.size();
                printf("\nthere are %d items in the inventory", count);
                bool found = false;
                ItemAndCount item;
                std::map<IItem*, ItemAndCount, std::less<IItem*>, std::allocator<std::pair<IItem* const, ItemAndCount> > >::iterator itr;
                // iterates htrough all items seeing if it can find them 
                for (itr = m_inventory.begin(); itr != m_inventory.end(); ++itr) {
                    printf("\n\titem: %s", itr->first->GetName());
                    if (itr->first->GetName() == split[1])
                    {
                        found = true;
                        printf("\nfound item: %s!", split[1]);
                        item = itr->second;
                    }
                }
                // probably didnt need this whole found variable but hey ho 
                if (found)
                {
                    printf("\nattempting to add new item...");
                    // adds x many of the item specified to the inventory 
                    PerformAddItem(item.item, numTimes, true);
                } else {
                    printf("item: %s not found in inventory!", split[1]);
                }
            } catch (...) {
                printf("\nfailed to get inventory!");
            }
        } 
        // coin setter (works much the same as the item duper)
        else if (split[0] == "setCoins")
        {
            printf("\ngot command: setCoins");
            int newCoinCount = stoi(split[1]);
            if (m_inventory.size() > 0) {
                std::map<IItem*, ItemAndCount, std::less<IItem*>, std::allocator<std::pair<IItem* const, ItemAndCount> > >::iterator firstItem = m_inventory.begin();
                std::string name = firstItem->first->GetName();
                printf("\nfirst item: %s", name);
                if (name=="Coin") {
                    firstItem->second.count = newCoinCount;
                } else {
                    printf("\ncouldn't set coins: no coins in inventory!");
                }
            } else {
                printf("\ncouldn't set coins: inventory is empty!");
            }
        } 
        // list players, will list all the players in the world 
        else if (split[0] == "listPlayers")
        {
            printf("\ngot command: listPlayers");
            ClientWorld* world = *((ClientWorld**)(dlsym(RTLD_NEXT, "GameWorld")));
            std::set<ActorRef<IPlayer>> players = world->m_players;
            printf("\nfound %d players", players.size());
            int count = 0;
            for (std::set<ActorRef<IPlayer>>::iterator i = players.begin(); i != players.end(); i++) {
                printf("\nplayer index %d in list:", count);
                ActorRef<IPlayer> element = *i;
                IActor* actorInterface = element->GetActorInterface();
                Vector3 lookPos = actorInterface->GetLookPosition();
                printf("\nlookPos: %f %f %f\n\n", lookPos.x, lookPos.y, lookPos.z);
                count++;
            }
        } 
        // list actors, goes thorugh the world object to ge tthe actor list then prints them all out 
        else if (split[0] == "listActors")
        {
            printf("\ngot command: listActors");
            ClientWorld* world = *((ClientWorld**)(dlsym(RTLD_NEXT, "GameWorld")));
            std::set<ActorRef<IActor>> actors = world->m_actors;
            printf("\nfound %d actors", actors.size());
            int count = 0;
            for (std::set<ActorRef<IActor>>::iterator i = actors.begin(); i != actors.end(); i++) {
                printf("\nactor index %d in list:", count);
                ActorRef<IActor> element = *i;
                Vector3 lookPos = element->GetLookPosition();
                printf("\nlookPos: %f %f %f\n\n", lookPos.x, lookPos.y, lookPos.z);
                count++;
            }
        } 
        // this does the same as list actors but only prints out the actors specified in the input 
        else if (split[0] == "listActorsByID")
        {
            printf("\ngot command: listActorsByID");
            ClientWorld* world = *((ClientWorld**)(dlsym(RTLD_NEXT, "GameWorld")));
            //std::map<unsigned int, ActorRef<IActor>> actorsByID = world->m_actorsById;
            actorsByID = world->m_actorsById;
            printf("\nfound %d actors", actorsByID.size());
            for (std::map<unsigned int, ActorRef<IActor>>::iterator i = actorsByID.begin(); i != actorsByID.end(); i++) {
                ActorRef<IActor> actorInterface = i->second;
                int id = i->first;
                printf("\n\nactor with id: %d", id);
                Actor* currentActor = world->GetActorById(id);
                printf("\ngot actor!");
                printf("\nactor %d name: %s", id, currentActor->GetDisplayName());
                Vector3 pos = currentActor->GetPosition();
                printf("\nactor pos: %f %f %f", pos.x, pos.y, pos.z);
            }
        }
        // not massivley useful tbh, grabs all the actors and tps them 10k units into the air 
        else if (split[0] == "tpAll") {
            printf("\ngot command: tpAll");
            //const char* str = ("getAll " + split[1]).c_str();
			const char* str = ("getAll " + args).c_str();
            Chat(str);
            for (std::vector<std::tuple<Actor*, Vector3>>::iterator i = actorList.begin(); i != actorList.end(); i++) {
                Actor* currentActor = std::get<0>(i[0]);
                if (currentActor->GetDisplayName() == args) {
                    //printf("\ngot %s!", currentActor->GetDisplayName());
                    Vector3 pos = currentActor->GetPosition();
                    printf("\n%s pos: %f %f %f", currentActor->GetDisplayName(), pos.x, pos.y, pos.z);
                    Vector3 playerPos = GetPosition();
                    printf("\nplayer is at pos: %f %f %f", playerPos.x, playerPos.y, playerPos.z);
                    Vector3 newPos = *(new Vector3(playerPos.x, playerPos.y, playerPos.z + 10000));
                    printf("\nteleporting %s to pos: %f %f %f", currentActor->GetDisplayName(), newPos.x, newPos.y, newPos.z);
                    currentActor->SetPosition(newPos);
                }
            }
        } 
        // grabs all the actors and whacks em into the actor list 
        else if(split[0] == "getAll") {
            printf("\ngot command: getAll");
            Chat("listActorsByID");
            actorList.clear();
            for (std::map<unsigned int, ActorRef<IActor>>::iterator i = actorsByID.begin(); i != actorsByID.end(); i++) {
                ActorRef<IActor> actorInterface = i->second;
                int id = i->first;
                //printf("\n\nactor with id: %d", id);
                Actor* currentActor = world->GetActorById(id);
                if (currentActor->GetDisplayName() == args) {
					printf("\n%s = %s, so adding to actor list...", currentActor->GetDisplayName(), args);
                    Vector3 playerPos = GetPosition();
                    actorList.push_back(std::make_tuple(currentActor, Vector3(playerPos.x, playerPos.y, playerPos.z + 5000)));
                    printf("\nadded %s to actorList, length: %d", currentActor->GetDisplayName(), actorList.size());
                }
            }
        } 

        else if(split[0] == "flappy"){
            // ends flappy time if you specify end 
			if (split[1] == "end") {
				flappyTime = false;
			} else {
                // grab all the bears 
				const char* str = ("getAll " + args).c_str();
				Chat(str);
                // we need 30 bears for this to work 
				int minBears = 30;

				if (actorList.size() < minBears) {
					std::cout << std::endl << "SORRY: Need 10 Bears to flappy, only have" << actorList.size() << "!!!" << std::endl;
				} else {
                    // gets the player position for the game to be above you 
                    flappyPos = GetPosition();
                    // initialises the game 
					initFlappy();
                    // now we flappin 
					flappyTime = true;
				}
			}
        } 
        else if(split[0] == "chat")
        {
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << message;
			std::cout << std::endl;
            std::cout << std::endl;
        } 
        else if(split[0] == "initflap")
        {
			const char* str = ("setHealth 1000000");
			Chat(str);
			const char* str2 = ("setSpeed 1000000");
			Chat(str2);
			const char* str3 = ("tp bears");
			Chat(str3);
        } 
        else if(split[0] == "angry")
        {
			if (split[1] == "end") {
				angryTime = false;
			} else {
				const char* str = ("getAll " + split[1]).c_str();
				Chat(str);

				angryTime = true;
			}
        } else if(split[0] == "spawnBear") {
			std::string spawnArg = "Bear";
			const char* str = ("getAll " + spawnArg).c_str();
			Chat(str);

	 		printf("\ngot command: spawnBear");
			//Actor* bear = (Actor*)dlsym(RTLD_NEXT,"_ZN4BearD2Ev");
			Actor* bear = std::get<0>(actorList.begin()[0]);
			printf("\ngot the bear!");
			Vector3 pos = GetPosition();
			Rotation rot = GetRotation();
			//Vector3* bearPos = new Vector3(pos.x, pos.y, pos.z+500);
			Vector3 bearPos;
			if(split.size() == 1){
				Vector3 playerPos = GetPosition();
				bearPos = Vector3(playerPos.x+200, playerPos.y, playerPos.z);
			} else {
				bearPos = Vector3(stof(split[1]), stof(split[2]), stof(split[3]));
			}
			printf("\nready to spawn bear...");
			try {
				world->SpawnActor(bear, bearPos, rot);
           	} catch (const std::exception&) {
               	printf("\nlol couldn't spawn bear");
           	}
			//world->SpawnActor(&bear, *bearPos, rot);
			printf("\nspawned bear!!!");
        } else if(split[0] == "spin"){
			const Rotation rot = Rotation(90,0,90);
			SetRemoteLookRotation(rot);
			SetRotation(rot);
        } else if(split[0] == "manySpawnBear") {
			int count = stoi(split[1]);
			for (int i = 0; i < count; i++){
				std::string command = "spawnBear";
				const char* str = (command).c_str();
				Chat(str);
			}
        }
    } else
    {
        printf("\nno message");
		message = "Invalid Command";
    }

    try
    {
        //void (*realChat)(const char*);
        void (*realChat)(Player*,const char*);

        //To find the address of the real function we need to find its  orginal address
        //For this we need the "mangled" name, which we found open the libGameLogic.so 
        //file in IDA looking at the exprots and right clicking and deselecting "demangle"
        //realChat =(void (*)(const char*))dlsym(RTLD_NEXT,"_ZN6Player4ChatEPKc");
        realChat =(void (*)(Player*,const char*))dlsym(RTLD_NEXT,"_ZN6Player4ChatEPKc");
        printf("\nrealChat is at: %p\n", realChat);
        realChat(this,message);
    } catch (const std::exception&)
    {
        printf("it did that error lol...");
    }
}

// #pragma region TickAttr



// #pragma endRegion

void moveTowards(Actor* actor, Vector3 dest, int amount){
	Vector3 pos = actor->GetPosition();
	float xdist = dest.x - pos.x;
	float ydist = dest.y - pos.y;
	float zdist = dest.z - pos.z;
	float distance = std::sqrt(std::pow(xdist,2) + std::pow(ydist,2) + std::pow(zdist,2));
	actor->SetPosition(Vector3(pos.x+((xdist/distance)*amount),pos.y+((ydist/distance)*amount),pos.z+((zdist/distance)*amount)));
}

void Player::Tick(float delta){
    if (flappyTime) {
		// if on flappy tick then update actor positions
		if(flappyTick == 0){
			printf("Flappy Tick!!!");

			// Clear out all bears
			for (std::vector<std::tuple<Actor*, Vector3>>::iterator i = actorList.begin(); i != actorList.end(); i++) {
					Actor* currentActor = std::get<0>(i[0]);
					Vector3 pos = Vector3(-4781,64000,2648);
					currentActor->SetPosition(pos);
			}

			// Updates actor positions
			flappy();
		}
		// Update flappy Tick
		flappyTick = (flappyTick + 1) % 100;
		// Move Actors to and KEEP them in their positions
		// Also keep player in position
        for (std::vector<std::tuple<Actor*, Vector3>>::iterator i = actorList.begin(); i != actorList.end(); i++) {
				//Vector3 playerPos = Vector3(-4781,65786,2648);
				Vector3 playerPos = flappyPos;
				SetPosition(playerPos);

                Actor* currentActor = std::get<0>(i[0]);
                Vector3 pos = std::get<1>(i[0]);
                currentActor->SetPosition(pos);
				(currentActor->m_spawner)->m_maxSpawnTimer = 0;
        }
    }

    if (angryTime) {
        for (std::vector<std::tuple<Actor*, Vector3>>::iterator i = actorList.begin(); i != actorList.end(); i++) {
                Actor* currentActor = std::get<0>(i[0]);
				moveTowards(currentActor,GetPosition(),30);
        }
    }

	void (*realTick)(Player*,float);
	realTick = (void (*)(Player*,float))dlsym(RTLD_NEXT,"_ZN6Player4TickEf");
	//printf("\nrealTick is at: %p\n", realTick);
	realTick(this,delta);
}

//std::vector<std::string> tokenize(std::string const& str, const char delim)
//{
//        std::vector<std::string> out;
//        // construct a stream from the string
//        std::stringstream ss(str);
//
//        std::string s;
//        while (std::getline(ss, s, delim)) {
//                out.push_back(s);
//        }
//    return out;
//}

