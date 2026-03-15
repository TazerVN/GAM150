#include "pch.h"
#include "CombatLevel.h"

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
	//PS.particleClick(ecs, mf, x, y);
	PS.particleDataStream(ecs, mf);
	PS.particleReverseStream(ecs, mf);
}
void LevelStateCombat_update()
{
	f32 dt = AEFrameRateControllerGetFrameTime();
	//==========(Object updates)===========
	if (AEInputCheckTriggered(AEVK_LBUTTON)) {

		s32 mouseX, mouseY;
		AEInputGetCursorPosition(&mouseX,&mouseY);

		f32 worldX = f32(mouseX) - (f32(AEGfxGetWindowWidth()) * 0.5f);
		f32 worldY = (f32(AEGfxGetWindowHeight()) * 0.5f) - f32(mouseY);
		PS.particleClick(ecs, mf, worldX, worldY);
	}
	scene.update();
	card.update_logic(ecs, scene.getTBS(), mf, TF, dt);
	UIM.update(scene);
	PS.update(ecs, 0.2);
	scene.getBattleGrid().update(ecs, CS.id());	//gameboard update
	AS.update(ecs, scene.getBattleGrid(), scene.getCombatSystem());
}
void LevelStateCombat_free()
{

}
void LevelStateCombat_unload()
{

}