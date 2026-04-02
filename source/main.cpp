// ---------------------------------------------------------------------------
// includes

#include "pch.h"
#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "global.h"
#include "util/GameStateManager.h"
#include "UI/UIObject.h"
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
	AESysSetWindowTitle("Beyond the Nexus! You should checkout Zombat");

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
			IM.update(ecs, CS.id());
			GameStateUpdate();
			//========(Render)====================
			console.update();

			PS.update(0.2);
			TS.update(ecs);
			RM.RM_render(ecs, CS.id());

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
	AF.free();
	console.free();
	// reset the system modules
	AESysReset();
	AESysExit();
}


void load_Sys_Comp()
{
	for (int itr = 0; itr < commandCount; ++itr)
	{
		ConsoleEvents.template_pool.push_back(EventTemplate<CommandTypes>{});
	}

	card_system.init_cards();
	beastiary.init_data();
	AF = AudioFactory::AudioFactory();
	AF.init();
	TF.textureInit();
	mf.MeshFactoryInit();   // builds all meshes including MESH_RECTANGLE_CENTER
	//==========System=============
	CS.init();
	RM.RenderSystem_init(ecs);

	Entity t = UIO::ui_blank_solid_center(0, 0, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, 100, 0.0f, 0.0f, 0.3f, 0.15f);
	Components::TagClass tg{Components::Tag::UI};
	ecs.addComponent(t, tg);
	EntityFactory::create_player();
	//=============================
}