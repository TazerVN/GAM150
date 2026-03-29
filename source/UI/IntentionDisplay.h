#pragma once
#include "pch.h"

class HighlightSystem;

class EnemyDirector;
class Scene;
Entity intenton_icon(AEGfxTexture* pTex, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z);

class IntentionDisplaySystem
{
private:
	HighlightSystem* hlptr;

	EnemyDirector* ptr_enemyDirector = nullptr;
	bool triggered{false};
	std::vector<std::pair<Entity, Entity>> intentionDisplay_list;
public:
	void init(EnemyDirector&,HighlightSystem&);
	void update(Scene& scene);
	void trigger();
	void intentionSystem_free();
};