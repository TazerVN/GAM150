#include "pch.h"

Entity create_ECS_enemy(JSON_ENEMY const& json_enemy)
{
	Entity enemy_id = ecs.createEntity();
	Components::Name nm{ json_enemy.name };
	Components::HP hp{ json_enemy.hp };
	Components::Attack atk{ json_enemy.value };
	Components::TurnBasedStats tbstats
	{ 5,	//max points
	  0,	//cur_points
	  0,	//shields
	  7 };	//movement spd
	Components::Targetting_Component targetting{ Targetting::SINGLE_TARGET,json_enemy.range,0 };
	Components::image_location eimg{ json_enemy.png };
	Components::Animation_Actor aa{ Components::AnimationType::IDLE };
	Components::AStarResult ar{};

	ecs.addComponent(enemy_id, nm);
	ecs.addComponent(enemy_id, hp);
	ecs.addComponent(enemy_id, atk);
	ecs.addComponent(enemy_id, tbstats);
	ecs.addComponent(enemy_id, targetting);
	ecs.addComponent(enemy_id, eimg);
	ecs.addComponent(enemy_id, ar);
	ecs.addComponent(enemy_id, aa);
	ecs.addComponent(enemy_id, eimg);

	return enemy_id;
}

void Beastiary::init_data()
{
	std::vector<JSON_ENEMY> vec;
	if (parse_enemy_data(vec, "Assets/misc/enemies.json") != JSON_RET::OK)
		return;

	for (JSON_ENEMY enemy : vec)
	{
		std::cout << "Name : " << enemy.name.c_str() << '\n';
		std::cout << "HP : " << enemy.hp << '\n';
		std::cout << "Value : " << enemy.value << '\n';
		std::cout << "Enemy Image : " << enemy.png << '\n' << '\n';
		Entity enemyECSID = create_ECS_enemy(enemy);
		enemies_map[enemy.name] = enemyECSID;
		enemies_vec.push_back(enemyECSID);
	}
}
Entity Beastiary::generate_enemy_from_beastiary(std::string key, AEVec2 spawnpos, AEVec2 size, Components::AnimationType at)
{
	if (enemies_map.find(key) == enemies_map.end())
		return static_cast<Entity>(-1);

	Entity id = ecs.createEntity();

	Entity beastiaryID = enemies_map[key];
	Components::Name nm{ ecs.getComponent<Components::Name>(beastiaryID)->value };
	Components::HP hp{ *ecs.getComponent<Components::HP>(beastiaryID) };
	Components::Attack atk{ *ecs.getComponent<Components::Attack>(beastiaryID) };
	Components::TurnBasedStats tbstats{ *ecs.getComponent<Components::TurnBasedStats>(beastiaryID) };
	Components::Targetting_Component targetting{ *ecs.getComponent<Components::Targetting_Component>(beastiaryID) };
	Components::image_location eimg{ *ecs.getComponent<Components::image_location>(beastiaryID)};

	AEGfxTexture* pTex = TF.getTextureFromEnemyMap(ecs.getComponent<Components::image_location>(beastiaryID)->location);
	Components::Texture tex{pTex , 0.0f, 0.0f };

	Components::Transform trans{ spawnpos, spawnpos , size , size,0.f };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
	Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
	Components::Timer timer{ 1.f, 0.5f, true, true };
	Components::Animation_Actor aa{ at };
	Components::AStarResult ar{};
	Components::Tag tag{ Components::Tag::ACTOR };
	Components::gridData gd{};


	ecs.addComponent(id, nm);
	ecs.addComponent(id, hp);
	ecs.addComponent(id, atk);
	ecs.addComponent(id, tbstats);
	ecs.addComponent(id, targetting);
	ecs.addComponent(id, eimg);

	ecs.addComponent(id, tex);
	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, timer);
	ecs.addComponent(id, aa);
	ecs.addComponent(id, ar);
	ecs.addComponent(id, tag);
	ecs.addComponent(id, gd);
	return id;
}
Entity Beastiary::generate_enemy_from_beastiary(Entity beastiaryID, AEVec2 spawnpos, AEVec2 size, Components::AnimationType at)
{
	Entity id = ecs.createEntity();

	Components::Name nm{ ecs.getComponent<Components::Name>(beastiaryID)->value };
	Components::HP hp{ *ecs.getComponent<Components::HP>(beastiaryID) };
	Components::Attack atk{ *ecs.getComponent<Components::Attack>(beastiaryID) };
	Components::TurnBasedStats tbstats{ *ecs.getComponent<Components::TurnBasedStats>(beastiaryID) };
	Components::Targetting_Component targetting{ *ecs.getComponent<Components::Targetting_Component>(beastiaryID) };
	Components::image_location eimg{ *ecs.getComponent<Components::image_location>(beastiaryID) };

	AEGfxTexture* pTex = TF.getTextureFromEnemyMap(ecs.getComponent<Components::image_location>(beastiaryID)->location);
	Components::Texture tex{ pTex , 0.0f, 0.0f };

	Components::Transform trans{ spawnpos, spawnpos , size , size,0.f };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, 1 };
	Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
	Components::Timer timer{ 1.f, 0.5f, true, true };
	Components::Animation_Actor aa{ at };
	Components::AStarResult ar{};
	Components::Tag tag{ Components::Tag::ACTOR };
	Components::gridData gd{};

	ecs.addComponent(id, nm);
	ecs.addComponent(id, hp);
	ecs.addComponent(id, atk);
	ecs.addComponent(id, tbstats);
	ecs.addComponent(id, targetting);
	ecs.addComponent(id, eimg);

	ecs.addComponent(id, tex);
	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, timer);
	ecs.addComponent(id, aa);
	ecs.addComponent(id, ar);
	ecs.addComponent(id, tag);
	ecs.addComponent(id, gd);
	return id;
}
Entity Beastiary::get_beastiary_id(std::string key)
{

	if (enemies_map.find(key) == enemies_map.end())
		return static_cast<Entity>(-1);

	return enemies_map[key];

}
size_t Beastiary::size() const
{
	return enemies_vec.size();
}