#include "pch.h"
#include "../UI/cardInteraction.h"
#include "TutorialLevel.h"
#include "../util/LevelManager.h"
#include "../UI/UI.h"

Scene tutorialscene;
UI::UIManager tut_UIM;
TutorialFlowStage gTutorialStage = TutorialFlowStage::BASICS;
int gTutorialSubstep = 0;


// gettign step helper
static int GetTutorialSubstepCount(TutorialFlowStage stage)
{
	switch (stage)
	{
	case TutorialFlowStage::BASICS:      return 3;
	case TutorialFlowStage::MOVEMENT:    return 3;
	case TutorialFlowStage::ATTACK_CARD: return 5;
	case TutorialFlowStage::DEFENSE_CARD:return 8;
	case TutorialFlowStage::ITEM_CARD:   return 5;
	case TutorialFlowStage::EVENT_CARD:  return 6;
	case TutorialFlowStage::WIN_TRANSITION: return 17;
	case TutorialFlowStage::DONE:           return 1;
	default:                             return 1;
	}
}

void LevelStateTutorial_load()
{
	if (old_card_buffer.empty())
	{
		Components::Card_Storage* storage = ecs.getComponent<Components::Card_Storage>(playerID);
		for (Entity card : storage->original_draw_pile)
		{
			old_card_buffer.push_back(card);
		}
		storage->original_draw_pile.clear();
	}

	gTutorialStage = TutorialFlowStage::BASICS;
	gTutorialSubstep = 0;

	tutorialscene.set_tutorial_active(true);

	if (playerID == -1)
		EntityFactory::create_player();

	Components::HP* playerHP = ecs.getComponent<Components::HP>(playerID);
	if (playerHP)
	{
		playerHP->max_value = PLAYER_MAX_HEALTH;
		playerHP->c_value = playerHP->max_value;
	}

}

void LevelStateTutorial_init()
{
	TutorialText.z = 1250;
	TutorialText.set(300.f, 0.4f);
	auto* storage = ecs.getComponent<Components::Card_Storage>(playerID);
	if (storage)
	{
		storage->reset();
		storage->free();
		storage->init();
	}

	tutorialscene.set_tutorial_active(true);
	tutorialscene.set_tutorial_stage(static_cast<int>(gTutorialStage));

	tutorialscene.set_tutorial_substep(gTutorialSubstep);

	tutorialscene.init(CS, tut_UIM);
	tut_UIM.combat_init(tutorialscene);

	PS.particleDataStream();
	PS.particleReverseStream();
	AS.init();
	PUT.init(&ecs, tut_UIM.getCardHand().getID());
	ecs.remove_empty_groups();
}

void LevelStateTutorial_update()
{
	f32 dt = (f32)AEFrameRateControllerGetFrameTime();

	// Always sync from Scene first so Scene is the real source of truth
	gTutorialStage = static_cast<TutorialFlowStage>(tutorialscene.get_tutorial_stage());
	gTutorialSubstep = tutorialscene.get_tutorial_substep();

	if (AEInputCheckTriggered(AEVK_SPACE))
	{
		// block manual skipping while the game is waiting for an action
		if (gTutorialStage == TutorialFlowStage::WIN_TRANSITION)
		{
			if (gTutorialSubstep == 7) // wait for horde to die
				return;
		}

		int maxSubsteps = GetTutorialSubstepCount(gTutorialStage);

		if (gTutorialSubstep < maxSubsteps - 1)
		{
			++gTutorialSubstep;
			tutorialscene.set_tutorial_stage(static_cast<int>(gTutorialStage));
			tutorialscene.set_tutorial_substep(gTutorialSubstep);
			tutorialscene.refresh_tutorial_text_only();
			return;
		}

		if (gTutorialStage != TutorialFlowStage::DONE)
		{
			gTutorialStage = static_cast<TutorialFlowStage>(static_cast<int>(gTutorialStage) + 1);
			gTutorialSubstep = 0;

			tutorialscene.set_tutorial_stage(static_cast<int>(gTutorialStage));
			tutorialscene.set_tutorial_substep(gTutorialSubstep);

			gLevelStateNext = LevelStates::LS_RESTART;
			return;
		}
	}

	if (AEInputCheckTriggered(AEVK_E))
	{
		if (gTutorialStage != TutorialFlowStage::BASICS)
		{
			gTutorialStage = static_cast<TutorialFlowStage>(static_cast<int>(gTutorialStage) - 1);
			gTutorialSubstep = 0;

			tutorialscene.set_tutorial_stage(static_cast<int>(gTutorialStage));
			tutorialscene.set_tutorial_substep(gTutorialSubstep);

			gLevelStateNext = LevelStates::LS_RESTART;
			return;
		}
	}

	if (AEInputCheckTriggered(AEVK_Q))
	{
		gLevelStateNext = LevelStates::LS_RESTART;
		return;
	}

	if (AEInputCheckTriggered(AEVK_LBUTTON))
	{

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX, &mouseY);

		f32 worldX = f32(mouseX) - (f32(AEGfxGetWindowWidth()) * 0.5f);
		f32 worldY = (f32(AEGfxGetWindowHeight()) * 0.5f) - f32(mouseY);
		PS.particleClick(worldX, worldY);
	}

	if (!player_died && !pause)
	{
		//UIM.getPauseMenu().free();

		VS.update();
		tutorialscene.update();
		tutorialscene.getBattleGrid().update( CS.id());
		CS.update();
		AS.update(tutorialscene.getBattleGrid(), tutorialscene.getGBS(), tutorialscene.getCombatSystem());
		tut_UIM.update(tutorialscene, dt);
	}
	else
	{
		tut_UIM.pause.update();
	}


	
	/*else if (!player_died && !UIM.getPauseMenu().isCreated() && UIM.getPauseMenu().isOn())
	else if (!player_died && !tut_UIM.getPauseMenu().isCreated() && tut_UIM.getPauseMenu().isOn())
	{
		PauseMenu& p = tut_UIM.getPauseMenu();
		p = PauseMenu(1300);
	}*/

	if (AEInputCheckTriggered(AEVK_F5))
		gLevelStateNext = LevelStates::LS_TUTORIAL;

	PUT.update();
}

void LevelStateTutorial_free()
{
	TutorialText.free();
	//gLevelStateNext = LevelStates::LS_QUIT;
	tutorialscene.set_tutorial_active(false);

 	tutorialscene.scene_free();
	tut_UIM.free();
	PS.particle_system_free();
	PUT.free();
	AF.bgm.stop();
	IT.free();
}

void LevelStateTutorial_unload()
{
	if (!old_card_buffer.empty())
	{
		Components::Card_Storage* stor = ecs.getComponent<Components::Card_Storage>(playerID);
		stor->reset();
		stor->original_draw_pile = old_card_buffer;
		old_card_buffer.clear();
	}
}