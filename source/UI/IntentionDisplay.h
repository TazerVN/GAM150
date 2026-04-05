//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once
#include "pch.h"

class HighlightSystem;

class EnemyDirector;
class Scene;
Entity intenton_icon(AEGfxTexture* pTex, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z);
namespace Grid { class GameBoard; }

class IntentionDisplaySystem
{
private:
	HighlightSystem* hlptr;
	
	Grid::GameBoard* gbptr = nullptr;
	EnemyDirector* ptr_enemyDirector = nullptr;
	bool triggered{false};
	std::vector<std::pair<Entity, Entity>> intentionDisplay_list;
	std::unordered_map<Entity, int> enemyRange;
public:
	void init(EnemyDirector&,HighlightSystem&,Grid::GameBoard&);
	void update(Scene& scene);
	void trigger();
	void intentionSystem_free();
};