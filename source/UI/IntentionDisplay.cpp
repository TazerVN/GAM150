#include "pch.h"

#include "IntentionDisplay.h"
#include "../system/EnemyDirector.h"
#include "ECS/Scene.h"

Entity intenton_icon(AEGfxTexture* pTex, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)
{
	Entity id = ecs.createEntity();
	//default player values
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},{}, rotation };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), TEXTURE , MESH_RECTANGLE_CORNER, z };
	Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
	Components::Texture texture{ pTex };
	Components::Tag tag{ Components::Tag::UI };

	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, texture);

	return id;
}


void IntentionDisplaySystem::init(EnemyDirector& enemyDirector)
{
	this->ptr_enemyDirector = &enemyDirector;

	for (auto it = this->ptr_enemyDirector->get_map().begin(); it != this->ptr_enemyDirector->get_map().end(); ++it)
	{
		Entity enemy = it->second;
		Entity intentionD = intenton_icon(TF.getTextureOthers(0), -50.f, 80.f, 32.f, 32.f, 0, 10);
		this->intentionDisplay_list.push_back({enemy , intentionD});
	}
	triggered = true;
}

void IntentionDisplaySystem::update(Scene& scene)
{
	EntityComponent::ComponentTypeID hordeID = EntityComponent::getComponentTypeID<Components::Horde_Tag>();

	//check if the enemy died if so delete the data
	int i = 0;
	for (; i < this->intentionDisplay_list.size(); i++)
	{
		Entity enemy = this->intentionDisplay_list[i].first;
		bool exist = false;

		Entity hordeEnt = scene.getTBS().get_participant()[1];
		
		if (ecs.getBitMask()[hordeEnt].test(hordeID))
		{
			auto horde = ecs.getComponent<Components::Horde_Tag>(hordeEnt);
			auto find = std::find(horde->goons.begin(), horde->goons.end(), enemy);
			if (find != horde->goons.end()) continue;
		}

		ecs.destroyEntity(this->intentionDisplay_list[i].second);
		this->intentionDisplay_list[i] = this->intentionDisplay_list[this->intentionDisplay_list.size() - 1];
		this->intentionDisplay_list.pop_back();
	}

	if (triggered)
	{
		triggered = false;
		size_t index = this->ptr_enemyDirector->index();
		const std::vector<EnemyDirector::Tokens>& timeline_ = this->ptr_enemyDirector->get_timeline();

		for (size_t i = 0; i < timeline_.size(); i++)
		{
			if (index >= timeline_.size()) index = 0;

			for (std::string a : timeline_[index])
			{
				Entity ent;
				std::string key = timeline_[index][0];
				std::unordered_map<std::string, Entity>& map = ptr_enemyDirector->get_map();
				if (map.find(key) != map.end())
				{
					ent = map[key];
				}
				else ent = -1;

				if (ent == -1) continue;

				std::string intent = timeline_[index][1];
				if (intent == "ATTACK")
				{
					for (auto it : intentionDisplay_list)
					{
						if (ent == it.first)
						{
							Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
							intentionDisp->texture = TF.getTextureOthers(3);
							break;
						}
					}
				}
				else if (intent == "MOVE")
				{
					for (auto it : intentionDisplay_list)
					{
						if (ent == it.first)
						{
							Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
							intentionDisp->texture = TF.getTextureOthers(4);
							break;
						}
					}
				}

				index++;
			}

			if (timeline_[index][0] == "STOP") break;
		}
	}

	if (!intentionDisplay_list.empty())
	{
		for (std::pair<Entity, Entity> p : intentionDisplay_list)
		{
			EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
			if (!ecs.getBitMask()[p.first].test(transID)) continue;
			if (!ecs.getBitMask()[p.second].test(transID)) continue;

			Components::Transform* parent = ecs.getComponent<Components::Transform>(p.first);
			Components::Transform* child = ecs.getComponent<Components::Transform>(p.second);
			Components::Mesh* parent_mesh = ecs.getComponent<Components::Mesh>(p.first);
			Components::Mesh* child_mesh = ecs.getComponent<Components::Mesh>(p.second);

			child->pos_onscreen.x = parent->pos_onscreen.x + child->pos.x;
			child->pos_onscreen.y = parent->pos_onscreen.y + child->pos.y;

			child_mesh->z = parent_mesh->z + 1;
		}
	}
}

void IntentionDisplaySystem::trigger()
{
	this->triggered = true;
}