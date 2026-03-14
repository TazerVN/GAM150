#include "game.h"
#include "../main.h"
#include "CombatLevel.h"
#include "../UI/cardInteraction.h"
#include "../ECS/Scene.h"
#include "../UI/UI.h"
#include "../system/particleSystem.h"

Scene scene;
CardInteraction::CardHand card{};
UI::UIManager UIM;
Particle::ParticleSystem PS;

void LevelStateCombat_load()
{

}
void LevelStateCombat_init()
{
	//==========(Init)======================
	s32 w_width = AEGfxGetWindowWidth();
	s32 w_height = AEGfxGetWindowHeight();

	//===========Game===============
	scene.init(ecs, mf, card_system, TF, CS, card);
	card = CardInteraction::CardHand(ecs, mf, TF, -1 * w_width / 8, -w_height / 2, w_width / 2, 264, scene.getTBS(), scene.getBattleGrid()
		, scene.getGBS());
	UIM.init(scene, mf, TF);
	ecs.remove_empty_groups();
	PS.particleClick(ecs, mf);
	PS.particleDataStream(ecs, mf);
}
void LevelStateCombat_update()
{
	f32 dt = AEFrameRateControllerGetFrameTime();
	//==========(Object updates)===========
	if (AEInputCheckTriggered(AEVK_LBUTTON)) {
		PS.particleClick(ecs, mf);
	}
	scene.update();
	card.update_logic(ecs, scene.getTBS(), mf, TF, dt);
	UIM.update(scene);
	PS.update(ecs, 0.2);
	scene.getBattleGrid().update(ecs, CS.id());	//gameboard update
}
void LevelStateCombat_free()
{

}
void LevelStateCombat_unload()
{

}