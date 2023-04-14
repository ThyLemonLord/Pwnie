// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include "libGameLogic.h"
#include "myMethods.h"
#include <list>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
// #include "Main.h"

#pragma region Globals

// std::thread *inputThread;
// InputServer *inpServer;

#pragma endRegion

using namespace std;

void Player::Tick(float delta)
{
	cout<< "FUCK YES!!" << endl;
	return;
}

void cursorGoTo(int x, int y){
	COORD coord;
	coord.X = x;
	coord.Y = y;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(hConsole, coord);
}

COORD GetConsoleCursorPosition(HANDLE hConsoleOutput){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hConsoleOutput, &csbi)){
		return csbi.dwCursorPosition;
	}

	COORD invalid = {0,0};
	return invalid;
}


/// @brief 
///		Starts the Server for input
/// @param _inpServer - The InputServer pointer that holds the InputServer object
// void startInputServer(InputServer* _inpServer){
// 	_inpServer->start();
// };

/// @brief This method defines a threat that will run concurrently with the game
DWORD WINAPI TeleportThread(HMODULE hModule, FILE* f)
{
	// The following 3 lines enable a writable console
	// We don't actually need a console here, but it is very useful to print debugging information to. 
	AllocConsole();
	FILE* f1 = new FILE;
	freopen_s(&f1, "CONOUT$", "w", stdout);
	freopen_s(&f1, "CONIN$", "r", stdin);

	//try{
	//Initialize the DLLGameSys object
	//This is so we can access all needed pointers from other threads
	//DLLGameSys gameSys = DLLGameSys();

	//Split input thread
	//inpServer = new InputServer();
	//inputThread = new std::thread(startInputServer, (InputServer*) inpServer);

	//inpServer->start();
	// }
	// catch(exception& e){
	// 	cerr << e.what() << endl;
	// }

	cout << "Injection worked\n";
	cout << "Process ID is: " << GetCurrentProcessId() << endl;
	// We can see by looking at the process ID in process explorer that this code is being run by the process it was injected into. 


	// From cheat engine analysis we know that
	// z_coord is at memory address: [[[["PwnAdventure3-Win32-Shipping.exe"+018FCD60] + 20 ] + 238 ] + 280 ] + 98 
	// This code follows that pointer path
	//
	// NB this may lead trying to dereference null pointers or reading memory you don't have access to.
	// In other module you would be expected to catch and handle these possible errors 

	uintptr_t PwnAventAddr = (uintptr_t)GetModuleHandle("PwnAdventure3-Win32-Shipping.exe");
	//printf("PwnAventAddr: %p\n", PwnAventAddr);
	uintptr_t firstStep = *(uintptr_t*)(PwnAventAddr + 0x01900600);
	//printf("PwnAventAddr + 0x01900600 = %p has value %p\n", PwnAventAddr + 0x01900600, firstStep);
	uintptr_t secondStep = *(uintptr_t*)(firstStep + 0xAC);
	//printf("firstStep + 0xAC = %p has value %p\n", firstStep + 0xAC, secondStep);
	uintptr_t thirdStep = *(uintptr_t*)(secondStep + 0x3C);
	//printf("secondStep + 0x3C = %p has value %p\n", secondStep + 0x3C, thirdStep);
	uintptr_t forthStep = *(uintptr_t*)(thirdStep + 0x488);
	//printf("thirdStep + 0x488 = %p has value %p\n", thirdStep + 0x488, forthStep);
	uintptr_t fifthStep = *(uintptr_t*)(forthStep + 0x218);
	//printf("forthStep + 0x218 = %p has value %p\n", thirdStep + 0x218, forthStep);
	//uintptr_t test = *(uintptr_t*)PwnAventAddr;
	//World* world = (World *)firstStep; // maybe?
	//cout << "1st m_health: " << ((Player*)(((ClientWorld*)world)->m_activePlayer.m_object))->m_health << endl;
	//world = (World*)secondStep;
	//cout << "2nd m_health: " << ((Player*)(((ClientWorld*)world)->m_activePlayer.m_object))->m_health << endl;
	//cout << "got world" << endl;
	//ClientWorld* clientWorld = (ClientWorld*) world;
	//cout << "got clientWorld" << endl;
	//Player* player = (Player *)(clientWorld->m_activePlayer.m_object);
	//cout << "got player" << endl;
	////player->m_walkingSpeed = 1000;
	//cout << "player name: " << player->GetPlayerName() << endl;
	//Vector3 pos;

	float* x_coord_Address = (float*)(fifthStep + 0x90);
	printf("x_coord_Address = %p\n", fifthStep + 0x90);
	float x_coord = *x_coord_Address;

	float* y_coord_Address = (float*)(fifthStep + 0x94);
	printf("y_coord_Address = %p\n", fifthStep + 0x94);
	float y_coord = *y_coord_Address;

	float* z_coord_Address = (float*)(fifthStep + 0x98);
	printf("z_coord_Address = %p\n", fifthStep + 0x98);
	float z_coord = *z_coord_Address;

	
	uintptr_t thirdStep2 = *(uintptr_t*)(secondStep + 0x20);
	//printf("secondStep + 0x20 = %p has value %p\n", secondStep + 0x20, thirdStep);
	uintptr_t forthStep2 = *(uintptr_t*)(thirdStep2 + 0x244);
	//printf("thirdStep + 0x244 = %p has value %p\n", thirdStep + 0x244, forthStep);
	uintptr_t fifthStep2 = *(uintptr_t*)(forthStep2 + 0x3E0);
	//printf("forthStep + 0x3E0 = %p has value %p\n", thirdStep + 0x3E0, forthStep);

	int* mana_Address = (int*)(fifthStep2 + 0xBC);
	printf("mana_Address = %p\n", fifthStep2 + 0xBC);
	int mana = *mana_Address;

	int* health_Address = (int*)(fifthStep2 + 0xBC - 0xFC);
	printf("health_Address = %p\n", (fifthStep2 + 0xBC - 0xFC));
	int health = *health_Address;

	// This is the main loop that will run in the background while I play the game
	std::vector<std::string> commands = { "setHealth", "setMana" };
	string input = "";
	string args = "";
	while (input != "exit") {
		// If the player z coordinate (height) changes then print it.
		if (x_coord != *x_coord_Address) {
			x_coord = *x_coord_Address;
			//cout << " X co-ord: " << x_coord << endl;
		}
		if (y_coord != *y_coord_Address) {
			y_coord = *y_coord_Address;
			//cout << " Y co-ord: " << y_coord << endl;
		}
		if (z_coord != *z_coord_Address) {
			z_coord = *z_coord_Address;
			//cout << " Z co-ord: " << z_coord << endl;
		}

		
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			cout << "   NUM1 key pressed" << endl;
			*x_coord_Address = *x_coord_Address - 10000;
			
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			cout << "   NUM3 key pressed" << endl;
			*x_coord_Address = *x_coord_Address + 10000;
			
		}

		if (GetAsyncKeyState(VK_NUMPAD5) & 1) {
			cout << "   NUM5 key pressed" << endl;
			*y_coord_Address = *y_coord_Address - 10000;
			
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			cout << "   NUM2 key pressed" << endl;
			*y_coord_Address = *y_coord_Address + 10000;
			
		}

		if (GetAsyncKeyState(VK_UP) & 1) {
			cout << "   UP key pressed" << endl;
			*z_coord_Address = *z_coord_Address + 10000;
			
		}
		if (GetAsyncKeyState(VK_DOWN) & 1) {
			cout << "   DOWN key pressed" << endl;
			*z_coord_Address = *z_coord_Address - 10000;
			
		}
		if (GetAsyncKeyState(VK_NUMPAD9) & 1) {
			cout << '\n' << "COMMAND MODE" << endl;
			bool check = (find(commands.begin(), commands.end(), input) == commands.end());
			bool out = true;
			do
			{
				if (out) {
					cout << '\n' << "\tCommand name: ";
				}
				out = false;
				cin >> input;
				cout << '\n' << "\tyou inputted: " << input << endl;
				check = (find(commands.begin(), commands.end(), input) == commands.end());
				if (check && input!="") {
					cout << "\tInvalid command: " << input << " try again..." << endl;
					out = true;
				}
			} while (check);
			cout << '\n' << "\targs: ";
			cin >> args;
			vector<string> split;
			tokenize(args, ' ', split);
			if (input == "setHealth") {
				int newHealth = stoi(split[0]);
				*health_Address = newHealth;
				cout << "\tSetting health to: " << newHealth << endl;
				//cout << "\tHealth value is: " << *health_Address << endl;
			}
			else if (input == "setMana") {
				int newMana = stoi(split[0]);
				*mana_Address = newMana;
				cout << "\tSetting mana to: " << newMana << endl;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD0) & 1) {
			//pos = player->GetPosition();
			//cout << "\tx: " << pos.x << "\ty: " << pos.y << "\tz: " << pos.z << endl;
		}
	}
	return 0;


}
/// @brief Function to split a string by some delimiter
/// @param str - String to tokenise
/// @param delim - The character to split the string on
/// @param out - The vector type that is being outputed to
void tokenize(std::string const& str, const char delim,
        std::vector<std::string>& out)
{
        // construct a stream from the string
        std::stringstream ss(str);

        std::string s;
        while (std::getline(ss, s, delim)) {
                out.push_back(s);
        }
}


// not using yet - might be useful to split command input to console and console output into 2 threads
//DWORD WINAPI ReadConsoleThread(HMODULE hModule)
//{
//	// The following 3 lines enable a writable console
//	// We don't actually need a console here, but it is very useful to print debugging information to. 
//	AllocConsole();
//	FILE* f2 = new FILE;
//	freopen_s(&f2, "CONOUT$", "w", stdout);
//	freopen_s(&f2, "CONIN$", "r", stdin);
//
//	std::cout << "Injection worked\n";
//	std::cout << "Process ID is: " << GetCurrentProcessId() << std::endl;
//	// We can see by looking at the process ID in process explorer that this code is being run by the process it was injected into. 
//
//
//	// From cheat engine analysis we know that
//	// z_coord is at memory address: [[[["PwnAdventure3-Win32-Shipping.exe"+018FCD60] + 20 ] + 238 ] + 280 ] + 98 
//	// This code follows that pointer path
//	//
//	// NB this may lead trying to dereference null pointers or reading memory you don't have access to.
//	// In other module you would be expected to catch and handle these possible errors 
//
//
//	uintptr_t PwnAventAddr = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");
//	//printf("PwnAventAddr: %p\n", PwnAventAddr);
//	uintptr_t firstStep = *(uintptr_t*)(PwnAventAddr + 0x01900600);
//	//printf("PwnAventAddr + 0x01900600 = %p has value %p\n", PwnAventAddr + 0x01900600, firstStep);
//	uintptr_t secondStep = *(uintptr_t*)(firstStep + 0xAC);
//	//printf("firstStep + 0xAC = %p has value %p\n", firstStep + 0xAC, secondStep);
//	uintptr_t thirdStep = *(uintptr_t*)(secondStep + 0x20);
//	//printf("secondStep + 0x3C = %p has value %p\n", secondStep + 0x20, thirdStep);
//	uintptr_t forthStep = *(uintptr_t*)(thirdStep + 0x244);
//	//printf("thirdStep + 0x488 = %p has value %p\n", thirdStep + 0x244, forthStep);
//	uintptr_t fifthStep = *(uintptr_t*)(forthStep + 0x3E0);
//	//printf("forthStep + 0x218 = %p has value %p\n", thirdStep + 0x3E0, forthStep);
//
//	int* mana_Address = (int*)(fifthStep + 0xBC);
//	printf("mana_Address = %p\n", fifthStep + 0xBC);
//	int mana = *mana_Address;
//
//	int* health_Address = (int*)(mana_Address - 0xFC);
//	printf("health_Address = %p\n", (mana_Address - 0xFC));
//	int health = *health_Address;
//
//	// This is the main loop that will run in the background while I play the game
//	std::string input = "";
//	while (input != "exit") {
//		printf("You inputted: %s", input);
//
//		scanf_s(":%s", input);
//	}
//	return 0;
//
//
//}




// This is the main method that runs when the DLL is injected.
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		// We run the cheat code in a seperate thread to stop it interupting the game execution. 
		// Again we dont catch a possible NULL, if we are going down then we can go down in flames.
		FILE* f = new FILE();
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)TeleportThread, hModule, 0, nullptr));
		
		//CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ReadConsoleThread, hModule, 0, nullptr));
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
