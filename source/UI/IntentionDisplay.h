#pragma once
#include "pch.h"

class EnemyDirector;
Entity intenton_icon(AEGfxTexture* pTex, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
{
	Entity id = ecs.createEntity();
	//default player values
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},{}, rotation };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), TEXTURE , MESH_RECTANGLE_CORNER, z };
	Components::Color color{ 0.0f, 0.8f, 1.0f ,1.0f };
	Components::Texture texture{ pTex };
	Components::Tag tag{ Components::Tag::UI };

	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, texture);

	return id;
}

class IntentionDisplaySystem
{
private:
	EnemyDirector* ptr_enemyDirector = nullptr;
public:
	void init(EnemyDirector&);
	void update();
};