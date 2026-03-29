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
	Components::Tag tag{ Components::Tag::UI };

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

    //if (triggered)
    //{
    //    triggered = false;

    //    // clear old danger highlights
    //    hlptr->unhighlight_cells();

    //    size_t index = this->ptr_enemyDirector->index();
    //    const std::vector<EnemyDirector::Tokens>& timeline_ = this->ptr_enemyDirector->get_timeline();

    //    // track simulated positions per enemy after their MOVE
    //    // so ATTACK knows where they'll actually be
    //    std::unordered_map<Entity, std::pair<s32, s32>> simulatedPos;

    //    for (size_t i = 0; i < timeline_.size(); i++)
    //    {
    //        if (index >= timeline_.size()) index = 0;
    //        if (timeline_[index][0] == "STOP") break;

    //        const EnemyDirector::Tokens& tok = timeline_[index];
    //        if (tok.size() < 2) { index++; continue; }

    //        // get entity for this line
    //        std::string key = tok[0];
    //        std::unordered_map<std::string, Entity>& map = ptr_enemyDirector->get_map();
    //        if (map.find(key) == map.end()) { index++; continue; }

    //        Entity ent = map[key];
    //        std::string intent = tok[1];

    //        if (intent == "MOVE")
    //        {
    //            // update intention icon
    //            for (auto it : intentionDisplay_list)
    //            {
    //                if (ent == it.first)
    //                {
    //                    Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
    //                    intentionDisp->texture = TF.getTextureOthers(4);
    //                    break;
    //                }
    //            }

    //            // simulate where they end up after the move
    //            // tok[4] is speed if present e.g "E1 MOVE IN_RANGE PLAYER 5"
    //            // tok[3] is speed for FRONT    e.g "E0 MOVE FRONT 10"
    //            int speed = 5; // default
    //            if (tok[2] == "FRONT" && tok.size() >= 4)
    //            {
    //                try { speed = std::stoi(tok[3]); }
    //                catch (...) {}
    //            }
    //            else if (tok[2] == "IN_RANGE" && tok.size() >= 5)
    //            {
    //                try { speed = std::stoi(tok[4]); }
    //                catch (...) {}
    //            }

    //            // get current or already-simulated position
    //            s32 ax, ay;
    //            if (simulatedPos.count(ent))
    //            {
    //                ax = simulatedPos[ent].first;
    //                ay = simulatedPos[ent].second;
    //            }
    //            else
    //            {
    //                if (!gbptr->findEntityCell(ent, ax, ay)) { index++; continue; }
    //            }

    //            // get player position
    //            Entity playerID = scene.getTBS().get_participant()[0];
    //            s32 px, py;
    //            if (!gbptr->findEntityCell(playerID, px, py)) { index++; continue; }

    //            // simulate movement — walk toward player capped by speed
    //            // simple manhattan step simulation
    //            int steps = speed;
    //            int cx = ax, cy = ay;
    //            auto& posGrid = gbptr->get_pos();

    //            for (int s = 0; s < steps; s++)
    //            {
    //                int dx = 0, dy = 0;
    //                int distX = std::abs(px - cx);
    //                int distY = std::abs(py - cy);

    //                // pick axis to move on
    //                if (distX >= distY)
    //                    dx = (px > cx) ? 1 : (px < cx ? -1 : 0);
    //                else
    //                    dy = (py > cy) ? 1 : (py < cy ? -1 : 0);

    //                int nx = cx + dx;
    //                int ny = cy + dy;

    //                // bounds check
    //                if (nx < 0 || nx >= MAX_I || ny < 0 || ny >= MAX_J) break;

    //                // blocked by something other than self
    //                if (posGrid[nx][ny] != Components::NULL_INDEX &&
    //                    posGrid[nx][ny] != ent) break;

    //                cx = nx;
    //                cy = ny;

    //                // stop adjacent to player
    //                if (std::abs(cx - px) + std::abs(cy - py) <= 1) break;
    //            }

    //            simulatedPos[ent] = { cx, cy };
    //            std::cout << "[ID] Simulated " << key << " will be at " << cx << "," << cy << "\n";
    //        }
    //        else if (intent == "ATTACK")
    //        {
    //            // update intention icon
    //            for (auto it : intentionDisplay_list)
    //            {
    //                if (ent == it.first)
    //                {
    //                    Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
    //                    intentionDisp->texture = TF.getTextureOthers(3);
    //                    break;
    //                }
    //            }

    //            // get simulated or current position
    //            s32 ax, ay;
    //            if (simulatedPos.count(ent))
    //            {
    //                ax = simulatedPos[ent].first;
    //                ay = simulatedPos[ent].second;
    //            }
    //            else
    //            {
    //                if (!gbptr->findEntityCell(ent, ax, ay)) { index++; continue; }
    //            }

    //            Entity playerID = scene.getTBS().get_participant()[0];
    //            s32 px, py;
    //            if (!gbptr->findEntityCell(playerID, px, py)) { index++; continue; }

    //            bool isRanged = ptr_enemyDirector->isRangedActor(key);

    //            // collect danger cells from simulated position
    //            std::vector<AEVec2> dangerCells;

    //            if (isRanged)
    //            {
    //                const int rangedRange = 5;
    //                bool sameRow = (ay == py);
    //                bool sameCol = (ax == px);

    //                if (sameRow || sameCol)
    //                {
    //                    int dx = (px > ax) ? 1 : (px < ax ? -1 : 0);
    //                    int dy = (py > ay) ? 1 : (py < ay ? -1 : 0);
    //                    int cx = ax + dx;
    //                    int cy = ay + dy;
    //                    int dist = 0;

    //                    while ((cx != px || cy != py) && dist < rangedRange)
    //                    {
    //                        dangerCells.push_back({ (f32)cx, (f32)cy });
    //                        cx += dx;
    //                        cy += dy;
    //                        dist++;
    //                    }
    //                    dangerCells.push_back({ (f32)px, (f32)py }); // player cell
    //                }
    //            }
    //            else
    //            {
    //                // melee — all cells within range 2 manhattan
    //                const int meleeRange = 2;
    //                for (int ix = ax - meleeRange; ix <= ax + meleeRange; ix++)
    //                {
    //                    for (int iy = ay - meleeRange; iy <= ay + meleeRange; iy++)
    //                    {
    //                        if (ix < 0 || ix >= MAX_I || iy < 0 || iy >= MAX_J) continue;
    //                        int dist = std::abs(ix - ax) + std::abs(iy - ay);
    //                        if (dist <= meleeRange)
    //                            dangerCells.push_back({ (f32)ix, (f32)iy });
    //                    }
    //                }
    //            }

    //            // store in highlight system
    //            hlptr->enemy_attack_highlighted_cells[ent] = dangerCells;
    //            for (auto& cell : dangerCells)
    //            {
    //                int ix = (int)cell.x;
    //                int iy = (int)cell.y;
    //                if (ix >= 0 && ix < MAX_I && iy >= 0 && iy < MAX_J)
    //                {
    //                    hlptr->enemy_attack_highlight_activate[ix][iy] = true;
    //                }
    //            }

    //            std::cout << "[ID] " << key << " danger cells: " << dangerCells.size() << "\n";
    //        }

    //        index++;
    //    }
    //}


	if (triggered)
	{
		triggered = false;
        hlptr->unhighlight_cells();
        enemyRange.clear();
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

                            bool isRanger = ecs.getComponent<Components::Name>(ent)->value == "Ranger";
                            Components::gridData* gd = ecs.getComponent<Components::gridData>(ent);
                            int atkRange = (isRanger) ? rangedRange : meleeRange;
                            int movRange = enemyRange[ent];
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

                            for (Components::GridCell& cell : hlptr->enemy_mov_highlighted_cells[ent])
                            {
                               
                                /*if (isOutermost(cell.x, cell.y, gd->x, gd->y, movRange))
                                {
                                    
                                }*/

                                if (isRanger)
                                {
                                    for (int itr = 0; itr <= atkRange; ++itr)
                                    {
                                        if (cell.x + itr < MAX_I)
                                        {
                                            hlptr->enemy_atk_highlight_activate[cell.x + itr][cell.y] = true;
                                            hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x + itr,cell.y });
                                        }
                                        if (cell.x - itr >= 0)
                                        {
                                            hlptr->enemy_atk_highlight_activate[cell.x - itr][cell.y] = true;
                                            hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x - itr,cell.y });
                                        }
                                        if (cell.y + itr < MAX_J)
                                        {
                                            hlptr->enemy_atk_highlight_activate[cell.x][cell.y + itr] = true;
                                            hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x,cell.y + itr });
                                        }
                                        if (cell.y - itr >= 0)
                                        {
                                            hlptr->enemy_atk_highlight_activate[cell.x][cell.y - itr] = true;
                                            hlptr->enemy_atk_highlighted_cells[ent].push_back({ cell.x,cell.y - itr });
                                        }
                                    }
                                }
                                else
                                {

                                }
                            }
							break;
						}
					}
				}
				else if (intent == "MOVE")
				{
                    //determine if its normal or in range
                    int range = 0;
                    if (timeline_[index][2] == "IN_RANGE")
                        range = std::stoi(timeline_[index][4]);
                    else
                        range = std::stoi(timeline_[index][3]);

                    enemyRange[ent] = range;

					for (auto it : intentionDisplay_list)
					{

						if (ent == it.first)
						{
							Components::Texture* intentionDisp = ecs.getComponent<Components::Texture>(it.second);
							intentionDisp->texture = TF.getTextureOthers(4);
                            
                            //display the movement range on grid
                            Components::gridData* gd = ecs.getComponent<Components::gridData>(ent);
                            Components::GridCell pos = Components::GridCell{ gd->x,gd->y };
                            
                            for (int i = -range; i <= range; ++i)
                            {
                                int j_range = range - std::abs(i);
                                for (int j = -j_range; j <= j_range; ++j)
                                {
                                    int tx = pos.x + i;
                                    int ty = pos.y + j;

                                    if (tx < 0 || tx >= MAX_I || ty < 0 || ty >= MAX_J)
                                        continue;

                                    Components::GridCell s{ s32(pos.x), s32(pos.y) };
                                    Components::GridCell g{ tx, ty };
                                    Components::AStarResult astar = AStar_FindPath_Grid4(MAX_I, MAX_J, gbptr->walkable, s, g);


                                    if (!astar.path.empty() && (int)astar.path.size() - 1 <= range)
                                    {
                                        hlptr->enemy_mov_highlight_activate[tx][ty] = true;
                                        hlptr->enemy_mov_highlighted_cells[ent].push_back({ tx,ty });
                                    }
                                }
                            }
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