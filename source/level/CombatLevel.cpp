#include "pch.h"
#include "../UI/cardInteraction.h"
#include "CombatLevel.h"
#include "../util/LevelManager.h"
#include "../UI/UI.h"

Scene scene;
UI::UIManager UIM;
Particle::ParticleSystem PS;

void LevelStateCombat_load()
{
	if (new_Start)
	{
		new_Start = false;
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

		Components::HP* playerHP = ecs.getComponent<Components::HP>(playerID);
		playerHP->max_value = PLAYER_MAX_HEALTH;
		playerHP->c_value = playerHP->max_value;

		std::cout << "New start! Reset Player" << std::endl;
	}
}
void LevelStateCombat_init()
{
	//==========(Init)======================
	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	//===========Game===============
	//load in player deck
	ecs.getComponent<Components::Card_Storage>(playerID)->init();


	scene.init(ecs, mf, card_system, TF, CS, UIM.getCardHand());
	/*card = CardInteraction::CardHand(ecs, mf, TF, -0.1f * w_width, -w_height / 2, w_width / 2, 264, scene.getTBS(), scene.getBattleGrid()
		, scene.getGBS(), UIM);*/
	UIM.init(scene, mf, TF);
	PS.particleDataStream(ecs, mf);
	PS.particleReverseStream(ecs, mf);
	AS.init(ecs);
	PUT.init(&ecs, UIM.getCardHand().getID());
	ecs.remove_empty_groups();
}
void LevelStateCombat_update()
{
	f32 dt = AEFrameRateControllerGetFrameTime();
	if (AEInputCheckTriggered(AEVK_LBUTTON)) {

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX,&mouseY);

		f32 worldX = f32(mouseX) - (f32(AEGfxGetWindowWidth()) * 0.5f);
		f32 worldY = (f32(AEGfxGetWindowHeight()) * 0.5f) - f32(mouseY);
		PS.particleClick(ecs, mf, worldX, worldY);
	}

	if (!scene.getTBS().player_died && !UIM.getPauseMenu().isOn())
	{
		UIM.getPauseMenu().free(ecs);
		if (AEInputCheckTriggered(AEVK_H)) // test particle
		{
			PS.particleHeal(ecs, mf, 0.0f, 0.0f);
		}
		scene.update();
		PS.update(0.2);
		scene.getBattleGrid().update(ecs, CS.id());	//gameboard update
		UIM.update(scene, dt);
		AS.update(ecs, scene.getBattleGrid(),scene.getGBS(), scene.getCombatSystem());
	}
	else if(!scene.getTBS().player_died && !UIM.getPauseMenu().isCreated() && UIM.getPauseMenu().isOn())
	{
		PauseMenu& p = UIM.getPauseMenu();
		p = PauseMenu(ecs, mf, 50);
	}

	if (AEInputCheckTriggered(AEVK_R)) gLevelStateNext = LevelStates::LS_RESTART;
	PUT.update();
}
void LevelStateCombat_free()
{
	scene.scene_free();
	UIM.free(ecs);
	ecs.getComponent<Components::Card_Storage>(playerID)->free();
	PS.particle_system_free();
}
void LevelStateCombat_unload()
{

}