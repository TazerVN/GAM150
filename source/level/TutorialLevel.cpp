#include "pch.h"
#include "../UI/cardInteraction.h"
#include "TutorialLevel.h"
#include "../util/LevelManager.h"
#include "../UI/UI.h"

extern Scene scene;
static UI::UIManager UIM;
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
	case TutorialFlowStage::WIN_TRANSITION: return 16;
	case TutorialFlowStage::DONE:           return 1;
	default:                             return 1;
	}
}

void LevelStateTutorial_load()
{
	gTutorialStage = TutorialFlowStage::BASICS;
	gTutorialSubstep = 0;

	scene.set_tutorial_active(true);

	if (playerID == -1)
		EntityFactory::create_player();

	Components::HP* playerHP = ecs.getComponent<Components::HP>(playerID);
	if (playerHP)
	{
		playerHP->max_value = PLAYER_MAX_HEALTH;
		playerHP->c_value = playerHP->max_value;
	}

	std::cout << "Tutorial load!\n";
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

	scene.set_tutorial_active(true);
	scene.set_tutorial_stage(static_cast<int>(gTutorialStage));
	std::cout << "[TutorialLevel_init] set tutorial_active = "
		<< scene.is_tutorial_active() << '\n';
	scene.set_tutorial_substep(gTutorialSubstep);

	scene.init(CS, UIM);
	UIM.combat_init(scene);

	PS.particleDataStream(ecs, mf);
	PS.particleReverseStream(ecs, mf);
	AS.init(ecs);
	PUT.init(&ecs, UIM.getCardHand().getID());
	ecs.remove_empty_groups();
}

void LevelStateTutorial_update()
{
	f32 dt = AEFrameRateControllerGetFrameTime();

	// Always sync from Scene first so Scene is the real source of truth
	gTutorialStage = static_cast<TutorialFlowStage>(scene.get_tutorial_stage());
	gTutorialSubstep = scene.get_tutorial_substep();

	if (AEInputCheckTriggered(AEVK_SPACE))
	{
		// block manual skipping while the game is waiting for an action
		if (gTutorialStage == TutorialFlowStage::WIN_TRANSITION)
		{
			if (gTutorialSubstep == 7) // wait for horde to die
				return;

			// lock the whole victory-card section until selection is done
			if (gTutorialSubstep >= 8 && gTutorialSubstep <= 12)
				return;
		}

		int maxSubsteps = GetTutorialSubstepCount(gTutorialStage);

		if (gTutorialSubstep < maxSubsteps - 1)
		{
			++gTutorialSubstep;

			scene.set_tutorial_stage(static_cast<int>(gTutorialStage));
			scene.set_tutorial_substep(gTutorialSubstep);
			scene.refresh_tutorial_text_only();
			return;
		}

		if (gTutorialStage != TutorialFlowStage::DONE)
		{
			gTutorialStage = static_cast<TutorialFlowStage>(static_cast<int>(gTutorialStage) + 1);
			gTutorialSubstep = 0;

			scene.set_tutorial_stage(static_cast<int>(gTutorialStage));
			scene.set_tutorial_substep(gTutorialSubstep);

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

			scene.set_tutorial_stage(static_cast<int>(gTutorialStage));
			scene.set_tutorial_substep(gTutorialSubstep);

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
		PS.particleClick(ecs, mf, worldX, worldY);
	}

	if (!player_died && !UIM.getPauseMenu().isOn())
	{
		UIM.getPauseMenu().free();
		VS.update(ecs);
		scene.update();
		scene.getBattleGrid().update(ecs, CS.id());
		CS.update();
		UIM.update(scene, dt);
		AS.update(ecs, scene.getBattleGrid(), scene.getGBS(), scene.getCombatSystem());
	}
	else if (!player_died && !UIM.getPauseMenu().isCreated() && UIM.getPauseMenu().isOn())
	{
		PauseMenu& p = UIM.getPauseMenu();
		p = PauseMenu(1300);
	}

	if (AEInputCheckTriggered(AEVK_F5))
		gLevelStateNext = LevelStates::LS_TUTORIAL;

	PUT.update();
}

void LevelStateTutorial_free()
{
	IT.free();
	TutorialText.free();
	scene.set_tutorial_active(false);
	//gLevelStateNext = LevelStates::LS_QUIT;
	scene.scene_free();
	UIM.free();
	PS.particle_system_free();
	PUT.free();
	AF.bgm.stop();
}

void LevelStateTutorial_unload()
{
	ecs.remove_empty_groups();
}