// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "util/GameStateManager.h"
#include "main.h"
// ---------------------------------------------------------------------------


//=====================One time declaration of extern values============================= 
ECS::Registry ecs;
MeshFactory mf;
InputSystem::InputManager IM;
TextureFactory::TextureFactory TF;
Camera::CameraSystem CS;
RenderSystem::RenderSystem RM;
CardSystem card_system;
VelocitySystem::VelocitySystem VS;
Particle::ParticleSystem PS;
TransformSystem::TransformSystem TrS;
TimerSystem::TimerSystem TS;
Animation::AnimationSystem AS;
//=======================================================================================


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
		mf.MeshFree();
		GameStateFree();

		if (gGameStateNext != GameStates::GS_RESTART)
			GameStateUnload();

		gGameStatePrev = gGameStateCurr;
		gGameStateCurr = gGameStateNext;
	}


	// reset the system modules
	AESysReset();
	AESysExit();
}