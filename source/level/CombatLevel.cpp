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
	if (new_Start)
	{
		
		new_Start = false;

		if(playerID == -1)
			EntityFactory::create_player();
		unsigned int seed = rand()*323123;
		std::srand(seed);
		if (parse_date_to_file(seed, "../../Assets/levels/cur_seed.json") != JSON_RET::OK)
		{
			std::cout << " put data fail";
		}

		int upper_bound = card_system.start_decks.size() - 1; int lower_bound = 0;
		int index = std::rand() % (upper_bound - lower_bound + 1) + lower_bound;

		JSON_DECK deck = card_system.start_decks[index];

		for (std::string& card_name : deck.cards)
		{
			EntityFactory::add_card_player_deck(ecs, playerID, card_system.generate_card_from_bible(card_name));
		}

		Components::HP* playerHP = ecs.getComponent<Components::HP>(playerID);
		playerHP->max_value = PLAYER_MAX_HEALTH;
		playerHP->c_value = playerHP->max_value;

		std::cout << "New start! Reset Player" << std::endl;
	}
	AF.bgm.play(0);
}
void LevelStateCombat_init()
{

	//===========Game===============
	//load in player deck
	ecs.getComponent<Components::Card_Storage>(playerID)->init();


	scene.init(CS, UIM);
	/*card = CardInteraction::CardHand(ecs, mf, TF, -0.1f * w_width, -w_height / 2, w_width / 2, 264, scene.getTBS(), scene.getBattleGrid()
		, scene.getGBS(), UIM);*/
	UIM.init(scene);

	PS.particleDataStream(ecs, mf);
	PS.particleReverseStream(ecs, mf);
	AS.init(ecs);
	PUT.init(&ecs, UIM.getCardHand().getID());
	AF.bgm.play(0);
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

	if (!player_died && !UIM.getPauseMenu().isOn())
	{
		UIM.getPauseMenu().free();
		VS.update(ecs);
		scene.update();
		PS.update(0.2);
		scene.getBattleGrid().update(ecs, CS.id());	//gameboard update
		//RENDER
		CS.update();
		UIM.update(scene, dt);
		AS.update(ecs, scene.getBattleGrid(),scene.getGBS(), scene.getCombatSystem());
	}
	else if(!player_died && !UIM.getPauseMenu().isCreated() && UIM.getPauseMenu().isOn())
	{
		PauseMenu& p = UIM.getPauseMenu();
		p = PauseMenu(50);
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
	AF.bgm.stop();
	EntityFactory::free_Player();
}
void LevelStateCombat_unload()
{
}