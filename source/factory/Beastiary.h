//=========================================
//	AUTHOR:		Wai Phyoo Ooo	
// 
//	EMAIL:		w.phyooo@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#pragma once
class Beastiary
{
	std::unordered_map<std::string, Entity> enemies_map;
public:
	std::vector<Entity>enemies_vec;
	void init_data();
	Entity generate_enemy_from_beastiary(std::string key, AEVec2 spawnpos, AEVec2 size, Components::AnimationType at);
	Entity generate_enemy_from_beastiary(Entity beastiaryID, AEVec2 spawnpos, AEVec2 size, Components::AnimationType at);
	Entity get_beastiary_id(std::string key);
	size_t size() const;
};