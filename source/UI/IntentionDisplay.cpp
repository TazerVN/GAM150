#include "pch.h"

#include "IntentionDisplay.h"
#include "../system/EnemyDirector.h"
#include "../system/HightlightSystem.h"
#include "ECS/Scene.h"

Entity intenton_icon(AEGfxTexture* pTex, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)
{
	Entity id = ecs.createEntity();
	//default player values
	Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},{}, rotation };
	Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), TEXTURE , MESH_RECTANGLE_CORNER, z };
	Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
	Components::Texture texture{ pTex };

	ecs.addComponent(id, trans);
	ecs.addComponent(id, mesh);
	ecs.addComponent(id, color);
	ecs.addComponent(id, texture);

	return id;
}


void IntentionDisplaySystem::init(EnemyDirector& enemyDirector,HighlightSystem& hl,Grid::GameBoard& gb)
{
	this->ptr_enemyDirector = &enemyDirector;
	this->hlptr = &hl;
	this->gbptr = &gb;

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
		//bool exist = false;

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
        i--;
	}


	if (triggered)
	{
		triggered = false;
        enemyRange.clear();
        hlptr->clear_enemy_cells();
        if (!intentionDisplay_list.empty())
        {
            for (auto it : intentionDisplay_list)
            {
                Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
                intentionDisp->texture = TF.getTextureOthers(11);
            }
        }

		size_t index = this->ptr_enemyDirector->index();
		const std::vector<EnemyDirector::Tokens>& timeline_ = this->ptr_enemyDirector->get_timeline();

		for (size_t itr = 0; itr < timeline_.size(); itr++)
		{
			if (index >= timeline_.size()) index = 0;

            Entity ent;
            std::string key = timeline_[index][0];
            std::unordered_map<std::string, Entity>& map = ptr_enemyDirector->get_map();
            if (map.find(key) != map.end())
            {
                ent = map[key];
            }
            else ent = static_cast<Entity>(-1);

            if (ent == static_cast<Entity>(-1))
            {
                index++;
                continue;
            }

            std::string intent = timeline_[index][1];
            if (intent == "ATTACK")
            {
                hlptr->enemy_atk_highlighted_cells[ent].clear();

                for (auto it : intentionDisplay_list)
                {
                    if (ent == it.first)
                    {
                        Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
                        intentionDisp->texture = TF.getTextureOthers(3);

                        bool isRanger = ecs.getComponent<Components::Name>(ent)->value == "Ranger";
                        Components::gridData* gd = ecs.getComponent<Components::gridData>(ent);
                        int atkRange = (isRanger) ? rangedRange : meleeRange;


                        auto isOutermost = [&](int x, int y, int originX, int originY, int range) -> bool
                            {
                                // check all 4 neighbours
                                const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
                                for (auto& d : dirs)
                                {
                                    int nx = x + d[0];
                                    int ny = y + d[1];
                                    int neighbourDist = std::abs(nx - originX) + std::abs(ny - originY);

                                    // if neighbour is outside the range, this cell is on the edge
                                    if (neighbourDist > range)
                                        return true;
                                }
                                return false;
                            };


                        if (hlptr->enemy_mov_highlighted_cells[ent].empty())
                        {
                            hlptr->enemy_mov_highlighted_cells[ent].push_back({ gd->x,gd->y });
                        }
                        for (Components::GridCell& cell : hlptr->enemy_mov_highlighted_cells[ent])
                        {
                            if (isRanger)
                            {
                                for (int _itr = 0; _itr <= atkRange; ++_itr)
                                {
                                    if (cell.x + _itr < MAX_I)
                                    {
                                        //hlptr->enemy_atk_highlight_activate[cell.x + itr][cell.y] = true;
                                        hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x + _itr,cell.y });
                                    }
                                    if (cell.x - _itr >= 0)
                                    {
                                        //hlptr->enemy_atk_highlight_activate[cell.x - itr][cell.y] = true;
                                        hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x - _itr,cell.y });
                                    }
                                    if (cell.y + _itr < MAX_J)
                                    {
                                        //hlptr->enemy_atk_highlight_activate[cell.x][cell.y + itr] = true;
                                        hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x,cell.y + _itr });
                                    }
                                    if (cell.y - _itr >= 0)
                                    {
                                        //hlptr->enemy_atk_highlight_activate[cell.x][cell.y - itr] = true;
                                        hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x,cell.y - _itr });
                                    }
                                }
                            }
                            else
                            {
                                auto try_highlight = [&](int x, int y)
                                    {
                                        if (x < 0 || x >= MAX_I || y < 0 || y >= MAX_J)
                                            return;

                                        hlptr->enemy_atk_highlighted_cells[ent].push_back({ x, y });
                                    };

                                for (int i_ = -atkRange; i_ <= atkRange; ++i_)
                                {
                                    int j_range = atkRange - std::abs(i_);
                                    for (int j = -j_range; j <= j_range; ++j)
                                    {
                                        try_highlight(cell.x + i_, cell.y + j);
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
            else if (intent == "MOVE")
            {

                if (timeline_.empty())
                {
                    std::cout << "Intention insert failed !" << '\n';
                    return;
                }
                //determine if its normal or in range
                int range = 0;
                if (timeline_[index][2] == "IN_RANGE")
                {
                    if (timeline_[index].size() < 5) return;
                    std::string token = timeline_[index][4];
                    range = std::stoi(token);
                }
                else
                {
                    if (timeline_[index].size() < 4) return;
                    std::string token = timeline_[index][3];
                    range = std::stoi(token);
                }


                if (enemyRange.find(ent) == enemyRange.end())
                {
                    enemyRange.emplace(ent, range);
                }
                else
                {
                    enemyRange.at(ent) = range;
                }

                hlptr->enemy_atk_highlighted_cells[ent].clear();

                for (auto it : intentionDisplay_list)
                {

                    if (ent == it.first)
                    {
                        Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
                        intentionDisp->texture = TF.getTextureOthers(4);

                        //display the movement range on grid
                        Components::gridData* gd = ecs.getComponent<Components::gridData>(ent);
                        Components::GridCell pos = Components::GridCell{ gd->x,gd->y };

                        for (int irange = -range; irange <= range; ++irange)
                        {
                            int j_range = range - std::abs(irange);
                            for (int j = -j_range; j <= j_range; ++j)
                            {
                                int tx = pos.x + irange;
                                int ty = pos.y + j;

                                if (tx < 0 || tx >= MAX_I || ty < 0 || ty >= MAX_J)
                                    continue;

                                Components::GridCell s{ s32(pos.x), s32(pos.y) };
                                Components::GridCell g{ tx, ty };
                                Components::AStarResult astar = AStar_FindPath_Grid4(MAX_I, MAX_J, gbptr->walkable, s, g);


                                if (!astar.path.empty() && (int)astar.path.size() - 1 <= range)
                                {
                                    //hlptr->enemy_mov_highlight_activate[tx][ty] = true;
                                    hlptr->enemy_mov_highlighted_cells[ent].push_back({ tx,ty });
                                }
                            }
                        }
                        break;
                    }

                }
            }
			index++;

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
			Components::Mesh* parent_mesh = ecs.getComponent<Components::Mesh>(p.first);

			Components::Transform* child = ecs.getComponent<Components::Transform>(p.second);
			Components::Mesh* child_mesh = ecs.getComponent<Components::Mesh>(p.second);

			child->pos_onscreen.x = parent->pos_onscreen.x + child->pos.x;
			child->pos_onscreen.y = parent->pos_onscreen.y + child->pos.y;

			if (parent_mesh == nullptr || child_mesh == nullptr) continue;
			child_mesh->z = parent_mesh->z + 1;
		}
	}
}

void IntentionDisplaySystem::trigger()
{
	this->triggered = true;
}

void IntentionDisplaySystem::intentionSystem_free()
{
	if(ptr_enemyDirector != nullptr)ptr_enemyDirector = nullptr;
	if(hlptr != nullptr)hlptr = nullptr;
	if (gbptr != nullptr) gbptr = nullptr;
	triggered = false;

	for (std::pair<Entity, Entity> pair : intentionDisplay_list)
	{
		ecs.destroyEntity(pair.second);
	}
	intentionDisplay_list.clear();
}