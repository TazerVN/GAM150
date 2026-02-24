#include "../ECS/gameObject.h"
#include "../ECS/ECSystem.h"
#include "../ECS/Scene.h"
#include "../factory/TextureFactory.h"
#include "../UI/UI.h"
#include <functional>

namespace UI
{

	void UIManager::init(Scene& scene, MeshFactory& mf, TextureFactory::TextureFactory& tf){

		this->list.push_back(ui_button(scene.getECS(), mf, 0.8F * AEGfxGetWinMaxX(), -0.65F * AEGfxGetWinMaxY(), 200, 0.3 * 200, 0, 2, [&scene] { scene.getTBS().next(scene.getECS()); }));
		this->list.push_back(ui_text(scene.getECS(), mf, tf ,0.72F, -0.67F, 0.3F, 0, 0, 5, "End Turn"));
	}

	Entity UIManager::ui_hp_bar(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.5f, 1.0f, 0.5f ,1.0f };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity UIManager::ui_button(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, std::function<void()> func)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 0.5, 0.5f, 0.5f ,1.0f };
		Components::Input in(AEVK_LBUTTON, true, func , nullptr, nullptr);	//add input system for grid
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, in);

		return id;
	}

	Entity UIManager::ui_text(ECS::Registry& ecs, MeshFactory& mf, TextureFactory::TextureFactory& tf,f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{a, tf.getFontID(), z};
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);

		return id;
	}


}