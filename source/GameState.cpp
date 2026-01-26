#include "GameState.h"

struct GameState
{
	bool game_running = false;
};

static GameState* current_gs = nullptr;

void SetGameState(void (*init)(), void (*update)(), void (*exit)())
{
	GameState gs;
	current_gs = &gs;
	init();
	gs.game_running = true;
	while (gs.game_running)
	{
		update();
	}
	exit();
	current_gs = nullptr;
}

void leaveGameState()
{
	if (current_gs != nullptr)
	{
		current_gs->game_running = false;
	}
}