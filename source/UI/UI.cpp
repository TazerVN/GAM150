#include "../ECS/gameObject.h"
#include "../ECS/ECSystem.h"
#include "../ECS/Scene.h"
#include "../factory/TextureFactory.h"
#include "../UI/UI.h"
#include <functional>
#include "../global.h"

namespace UI
{

	void UIManager::init(Scene& scene, MeshFactory& mf, TextureFactory::TextureFactory& tf) 
	{

		Entity end_b = ui_button(scene.getECS(), mf, 0.8F * AEGfxGetWinMaxX(), -0.65F * AEGfxGetWinMaxY(), 200, 0.3 * 200, 0, 2, 
			// the lambda function
			[&scene] 
			{ 
				if(scene.getGBS().getGBPhase() != PhaseSystem::GBPhase::MAIN_PHASE) return;
				scene.getTBS().next(scene.getECS()); 
			}
		);
		Entity end_t = ui_text(scene, tf, mf ,0.72F, -0.67F, 0.3F, 0, 0, 5, "End Turn");
		std::pair<Entity, Entity> end_turn{end_b, end_t};



		for(Entity e : scene.entities_store())
		{
			
			Entity health = ui_hp_bar(scene, mf, -50, 100, 100, 10, 0, 5);
			std::pair<Entity, Entity> hp{ e, health };
			this->list.push_back(hp);
		}



		this->list.push_back(end_turn);
	}

	void UIManager::update(Scene& scene)
	{
		ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
		ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();

		/*for(int i = 0; i < this->list.size(); i++)
		{
			for(int j = 0; j <  scene.getTBS().pa; j++){
				if (!scene.getECS().getBitMask()[this->list[i].first].test(hpID)) continue;
				else if(scene.entities_store()[j] == this->list[i].first) continue;
				else{
					this->list[i] = this->list[this->list.size() - 1];
					this->list.pop_back();
				}
			}
		}*/
		this->health_update(scene.getECS());

		for(std::pair<Entity, Entity> p : this->list)
		{

			

			if (!scene.getECS().getBitMask()[p.first].test(transID)) continue;
			if (!scene.getECS().getBitMask()[p.second].test(transID)) continue;

			Components::Transform* parent = scene.getECS().getComponent<Components::Transform>(p.first);
			Components::Transform* child = scene.getECS().getComponent<Components::Transform>(p.second);

			child->pos_onscreen.x = parent->pos_onscreen.x + child->pos.x;
			child->pos_onscreen.y = parent->pos_onscreen.y + child->pos.y;
			
		}
	}

	void UIManager::health_update(ECS::Registry& ecs)
	{

		ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
		ECS::ComponentTypeID hpID = ECS::getComponentTypeID<Components::HP>();
		//create bitsets
		ECS::ComponentBitMask objMask;
		objMask.set(transID); objMask.set(hpID);

		for(std::pair<Entity, Entity> p : this->list)
		{
			if (!ecs.getBitMask()[p.first].test(hpID)) continue;
			if (!ecs.getBitMask()[p.second].test(hpID)) continue;
			Components::HP* hp_parent = ecs.getComponent<Components::HP>(p.first);

			Components::HP* hp_child = ecs.getComponent<Components::HP>(p.second);
			Components::Transform* transform_child = ecs.getComponent<Components::Transform>(p.second);

			hp_child->c_value = hp_parent->c_value/hp_parent->max_value * hp_child->max_value;
			transform_child->size.x = hp_child->c_value/hp_child->max_value * transform_child->size_col.x;
		}

	}

	Entity UIManager::ui_hp_bar(Scene& scene, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)

	{

		Entity id = scene.getECS().createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.5f, 1.0f, 0.5f ,1.0f };
		Components::HP hp{100};
		scene.getECS().addComponent(id, trans);
		scene.getECS().addComponent(id, mesh);
		scene.getECS().addComponent(id, color);
		scene.getECS().addComponent(id, hp);

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

	Entity UIManager::ui_text(Scene& scene, TextureFactory::TextureFactory& tf, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z, const char* a)

	{
		Entity id = scene.getECS().createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{a, tf.getFontID(), z};
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		scene.getECS().addComponent(id, trans);
		scene.getECS().addComponent(id, text);
		scene.getECS().addComponent(id, color);

		return id;
	}


}