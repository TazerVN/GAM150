#include "pch.h"
#include "../UI/cardInteraction.h"
#include "TutorialLevel.h"
#include "../util/LevelManager.h"
#include "../UI/UI.h"

extern Scene scene;
static UI::UIManager UIM;

void LevelStateTutorial_load()
{
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
	ecs.getComponent<Components::Card_Storage>(playerID)->init();

	scene.set_tutorial_active(true);
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