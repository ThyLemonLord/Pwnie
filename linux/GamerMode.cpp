#include <dlfcn.h>
#include <set>
#include <map>
#include <functional>
#include <string>
#include <cstring>
#include <vector>
#include "libGameLogic.h" // <= this is the header for the target program that was reconstructed using gdb 

// ALL COMMENTS ARE VERY IMPORTANT NO FIDDLING PLEASE 

// Compile this code with:
// g++ GamerMode.cpp -shared -fPIC -o GamerMode.so
//
//To run copy the so file to the game dir and run:
// LD_PRELOAD=/home/sxc1327/Desktop/team10-pwnie/linux/GamerMode.so ../../PwnAdventure3/PwnAdventure3/Binaries/Linux/PwnAdventure3-Linux-Shipping



// Override the chat function cuz we pogging 
void Player::Chat(const char *msg) {
	// case switch not possible :(
	// compare the command to ones we've set
	if(strncmp("tp ", msg, 3) == 0) {
		// Vector3 from libgamelogic  
		Vector3* new_pos = new Vector3();
		// scan the args from the input
		sscanf(msg+3, "%f %f %f", &(new_pos->x), &(new_pos->y), &(new_pos->z));
		// perform the teleport 
		this->SetPosition(*new_pos);
	} 
	
	// teleport relative to current position 
	else if(strncmp("tpr ", msg, 4) == 0) {
		Vector3 new_pos = this->GetPosition();
		Vector3* offset = new Vector3();
		sscanf(msg+4, "%f %f %f", &(offset->x), &(offset->y), &(offset->z));
		new_pos.x +=  offset->x;
		new_pos.y +=  offset->y;
		new_pos.z +=  offset->z;
		this->SetPosition(new_pos);
	} 
	// Matches the names to those you can lookup in lsInv "dupeI <Itemname>"
	// Somethings odd with the printf stream so run lsInv a couple times if unsure
	// "dupeI all" adds one of everyhting possible you already have 
	// "dupeI coin" for example will add one coin  
	else if(strncmp("dupeI ", msg, 6)  == 0){
		int count = m_inventory.size();
		char param[256]; 
		sscanf(msg+6, "%s", &(param));
		std::map<IItem*, ItemAndCount, std::less<IItem*>, std::allocator<std::pair<IItem* const, ItemAndCount> > >::iterator itr;
        for (itr = m_inventory.begin(); itr != m_inventory.end(); ++itr) {
        	if (strncmp("all", param, sizeof(param))  == 0)
        	{
        		this->PerformAddItem(itr->first, 1, true);
        	}
        	else if (strncmp((itr->first->GetName()), param, sizeof(param))  == 0)
        	{
        		this->PerformAddItem(itr->first, 1, true);
        	}
        }
        
	}
	
	else if(strncmp("lsInv", msg, 5)  == 0){
		int count = m_inventory.size();
		printf("\nthere are %d items in the inventory", count);
		std::map<IItem*, ItemAndCount, std::less<IItem*>, std::allocator<std::pair<IItem* const, ItemAndCount> > >::iterator itr;
        for (itr = m_inventory.begin(); itr != m_inventory.end(); ++itr) {
        	printf("\nitem: %s", itr->first->GetName());
        	//this->PerformAddItem(itr->first, 1, true);
        }
        
	}
}

void ClientWorld::EquipItem(Player *, uint8_t, IItem *)
{

}





void World::Tick(float f)
{
	// dlsym will let us find the next occurence of a function 
	// allows preloader to make wrapper functions and find addresses linked to symbols
	// this is pogging 
	ClientWorld* world = *(ClientWorld**)dlsym(RTLD_NEXT, "GameWorld");
	//printf("[world] | %p\n", world);
	IPlayer* iplayer = world->m_activePlayer.m_object; // retuns nil 
    Player* player = ((Player*)(iplayer));
    
    ILocalPlayer* ilocalplayer = world->m_localPlayer;
    //printf("[player] | %p\n", ilocalplayer);
	//ILocalPlayer* ilocalplayer = world->m_localPlayer;
	//Player* player = ((Player*)(ilocalplayer));


	//printf("[tick] %0.2f | %d\n", f, world->HasLocalPlayer());
	//printf("[tick] %0.2f | PlayerName %s\n", f, iplayer->GetPlayerName());
	//printf("[mana] | %d\n", player->m_mana);
}
// 315      class Actor : public IActor :: void SetPosition(const Vector3 &);
// 456		class Spawner : public AIZoneListener :: virtual Actor * Spawn();
// 457		class Spawner : public AIZoneListener :: void RemoveActor(Actor *);
