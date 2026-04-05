#include "pch.h"
#include "../UI/cardInteraction.h"
#include "CombatLevel.h"
#include "../util/LevelManager.h"
#include "../UI/UI.h"

Scene scene;
UI::UIManager UIM;
//Particle::ParticleSystem PS;

void LevelStateCombat_load()
{
	if (gameData.new_Start)
	{

		gameData.new_Start = false;

		gameData.scoringSystem.reset();
		ecs.getComponent<Components::Card_Storage>(playerID)->reset();

		gameData.seed = rand();
		std::srand(gameData.seed);

		int upper_bound = static_cast<int>(card_system.start_decks.size()) - 1;
		int lower_bound = 0;
		int index = std::rand() % (upper_bound - lower_bound + 1) + lower_bound;

		//JSON_DECK deck = card_system.start_decks[index];

		//perfect deck (optimal deck)
		int perfection = (card_system.start_decks.size() - 2);
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
	AF.bgm.play(0);
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
}
void LevelStateCombat_update()
{
	f32 dt = (f32)AEFrameRateControllerGetFrameTime();
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
		scene.getBattleGrid().update(CS.id());	//gameboard update
		//RENDER
		CS.update();
		AS.update(scene.getBattleGrid(),scene.getGBS(), scene.getCombatSystem());
		UIM.update(scene, dt);
	}
	else
	{
		UIM.pause.update();
	}
	/*else if(!player_died && !UIM.getPauseMenu().isCreated() && UIM.getPauseMenu().isOn())
	{
		PauseMenu& p = UIM.getPauseMenu();
		p = PauseMenu(1300);
	}*/

	if (AEInputCheckTriggered(AEVK_F5)) gLevelStateNext = LevelStates::LS_RESTART;
	PUT.update();
}
void LevelStateCombat_free()
{
	scene.scene_free();
	UIM.free();
	PS.particle_system_free();
	PUT.free();
	AF.bgm.stop();
	IT.free();
}
void LevelStateCombat_unload()
{
}