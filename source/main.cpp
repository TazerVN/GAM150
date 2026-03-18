// ---------------------------------------------------------------------------
// includes

#include "pch.h"
#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "global.h"
#include "util/GameStateManager.h"
// ---------------------------------------------------------------------------

void load_Sys_Comp();

// main
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialization of your own variables go here
	
	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	// Changing the window title
	AESysSetWindowTitle("Yes!");

	/*SetGameState(game_init,game_update,game_free);*/

	load_Sys_Comp();

	GameStateMgrInit(GameStates::GS_MAINMENU);

	while (gGameStateCurr != GameStates::GS_QUIT)
	{
		// Reset the system modules
		AESysReset();

		// If not restarting, load the gamestate
		if (gGameStateCurr != GameStates::GS_RESTART)
		{
			GameStateMgrUpdate();
			GameStateLoad();
		}
		else
			gGameStateNext = gGameStateCurr = gGameStatePrev;

		// Initialize the gamestate
		GameStateInit();

		while (gGameStateCurr == gGameStateNext)
		{
			AESysFrameStart();

			GameStateUpdate();

			AESysFrameEnd();

			// check if forcing the application to quit
			if (AESysDoesWindowExist() == false)
				gGameStateNext = GameStates::GS_QUIT;
		}
		GameStateFree();

		if (gGameStateNext != GameStates::GS_RESTART)
			GameStateUnload();

		gGameStatePrev = gGameStateCurr;
		gGameStateCurr = gGameStateNext;
	}

	mf.MeshFree();
	// reset the system modules
	AESysReset();
	AESysExit();
}


void load_Sys_Comp()
{
	TF.textureInit();
	mf.MeshFactoryInit();   // builds all meshes including MESH_RECTANGLE_CENTER
	//==========System=============
	CS.init(ecs);
	RM.RenderSystem_init(ecs);
	card_system.init_cards(ecs);

	Entity temp;

	//Add player
	temp = EntityFactory::create_actor_spritesheet(ecs, mf, { 0.f,0.f }, { 192.0f,192.0f }, "Player", 100.f, TF.getTextureChar(2), Components::AnimationType::NONE);
	playerID = temp;//important must set the playerID !!!!!!!!!!!
	for (int i = 0; i < 5; ++i)
	{
		EntityFactory::add_card_player_deck(ecs, playerID, card_system.generate_card_from_bible(ecs, "Slash"));
	}
	EntityFactory::add_card_player_deck(ecs, playerID, card_system.generate_card_from_bible(ecs, "PP up"));
	EntityFactory::add_card_player_deck(ecs, playerID, card_system.generate_card_from_bible(ecs, "Aura Farm"));
	for (int i = 0; i < 3; ++i)
	{
		EntityFactory::add_card_player_deck(ecs, playerID, card_system.generate_card_from_bible(ecs, "Black Hole"));
	}
	//=============================
}