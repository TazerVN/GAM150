//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
//				Zejin Kendreich Dayap Chen
//				Tio Chun Yi
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
//				chen.z@digipen.edu
//				tio.c@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "pch.h"
#include "../UI/cardInteraction.h"
#include "CombatLevel.h"
#include "../util/LevelManager.h"
#include "../UI/UI.h"

Scene scene;
UI::UIManager UIM;

void LevelStateCombat_load()
{
	if (gameData.new_Start)
	{
		gameData.new_Start = false;

		gameData.scoringSystem.reset();
		ecs.getComponent<Components::Card_Storage>(playerID)->reset();

		gameData.seed = rand();
		std::srand(gameData.seed);


		//perfect deck (optimal deck)
		size_t perfection = (card_system.start_decks.size() - 2);
		JSON_DECK deck = card_system.start_decks[perfection];

		//for teesting decks
		//JSON_DECK deck = card_system.start_decks[3];

		for (std::string& card_name : deck.cards)
		{
			EntityFactory::add_card_player_deck(playerID, card_system.generate_card_from_bible(card_name));
		}

		Components::HP* playerHP = ecs.getComponent<Components::HP>(playerID);
		playerHP->max_value = PLAYER_MAX_HEALTH;
		playerHP->c_value = playerHP->max_value;

	}
	
}
void LevelStateCombat_init()
{

	//===========Game===============
	//load in player deck
	scene.set_tutorial_active(false);
	scene.init(CS, UIM);
	UIM.combat_init(scene);

	ecs.getComponent<Components::Card_Storage>(playerID)->init();

	PS.particleDataStream();
	PS.particleReverseStream();
	AS.init();
	PUT.init(&ecs, UIM.getCardHand().getID());

	auto camera_position = ecs.getComponent<Components::Transform>(CS.id());
	auto player_position = ecs.getComponent<Components::Transform>(playerID);
	camera_position->pos = player_position->pos;

	camera_position->pos.x = AEClamp(camera_position->pos.x, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
	camera_position->pos.y = AEClamp(camera_position->pos.y, AEGfxGetWinMinY(), AEGfxGetWinMaxY());

}
void LevelStateCombat_update()
{
	if (AEInputCheckTriggered(AEVK_LBUTTON)) {

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX,&mouseY);

		f32 worldX = f32(mouseX) - (f32(AEGfxGetWindowWidth()) * 0.5f);
		f32 worldY = (f32(AEGfxGetWindowHeight()) * 0.5f) - f32(mouseY);
		PS.particleClick(worldX, worldY);
	}

	if (!player_died && !pause)
	{
		VS.update();
		scene.update();
		scene.getBattleGrid().update();	//gameboard update
		//RENDER
		CS.update();
		AS.update(scene.getBattleGrid(),scene.getGBS(), scene.getCombatSystem());
		UIM.update(scene);
	}
	else
	{
		UIM.pause.update();
	}


	if (AEInputCheckTriggered(AEVK_F5)) gLevelStateNext = LevelStates::LS_RESTART;
	PUT.update();
}
void LevelStateCombat_free()
{
	scene.scene_free();
	UIM.free();
	PS.particle_system_free();
	PUT.free();
	IT.free();
}
void LevelStateCombat_unload()
{
}