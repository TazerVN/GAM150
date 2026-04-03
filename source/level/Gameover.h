#pragma once

struct GameStats
{
	size_t levelscleared = 0;
};

extern GameStats g_GameStats;



void GameState_GameOver_load();
void GameState_GameOver_init();
void GameState_GameOver_update();
void GameState_GameOver_free();
void GameState_GameOver_unload();