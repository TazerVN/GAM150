#include "pch.h"
#include "HightlightSystem.h"

#include "GridSystem.h"
#include "TurnBasedSystem.h"
#include "../util/Pathfinding.h"


//enum class highlight_tag
//{
//	UNHIGHLIGHTED = 0,
//	ATTACK_HIGHLIGHT = 1,
//	MOVE_HIGHLIGHT = 2
//};

Components::RGBA HIGHLIGHT_PLAYER_ATTACK{ +0.5f,-0.3f,-0.3f,0.f };
Components::RGBA HIGHLIGHT_MOV{ -0.2f,-0.2f,+0.4f,0.f };
Components::RGBA HIGHTLIGHT_UNHIGHLIGHT{ 0.f,0.f,0.f,0.f };

std::array<Components::RGBA, static_cast<size_t>(highlight_tag::COUNT)> HIGHT_LIGHT_VALUES;

HighlightSystem::HighlightSystem(EventPool<highlight_tag>& eventPool,
	Grid::GameBoard& gb,
	CombatNameSpace::CombatSystem& cbs,
	TBS::TurnBasedSystem& tbs)
{
	HIGHT_LIGHT_VALUES.at(static_cast<size_t>(highlight_tag::UNHIGHLIGHTED)) = HIGHTLIGHT_UNHIGHLIGHT;
	HIGHT_LIGHT_VALUES.at(static_cast<size_t>(highlight_tag::ATTACK_HIGHLIGHT)) = HIGHLIGHT_PLAYER_ATTACK;
	HIGHT_LIGHT_VALUES.at(static_cast<size_t>(highlight_tag::MOVE_HIGHLIGHT)) = HIGHLIGHT_MOV;

	evntptr = &eventPool;
	gbptr = &gb;
	cbsptr = &cbs;
	tbsptr = &tbs;
}

void HighlightSystem::highlight_cells(Targetting targetting, int range, highlight_tag type)
{
	//=========================Highlight_cells=================================
	AEVec2 cur_part_pos = gbptr->Get_CurPart_gridPos();

	switch (type)
	{
	case highlight_tag::UNHIGHLIGHTED:
		break;
	case highlight_tag::ATTACK_HIGHLIGHT:
		{
			if (targetting == Targetting::LINE)
			{
				for (int itr = 0; itr <= range; ++itr)
				{
					if (cur_part_pos.x + itr < MAX_I)
					{
						cbsptr->get_selected_cell().push_back({ cur_part_pos.x + itr, cur_part_pos.y });
						atk_highlighted_cells.push_back({ int(cur_part_pos.x) + itr, int(cur_part_pos.y) });
						this->highlight_activate[(int)cur_part_pos.x + itr][(int)cur_part_pos.y] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
					}
					if (cur_part_pos.x - itr >= 0)
					{
						cbsptr->get_selected_cell().push_back({ cur_part_pos.x - itr, cur_part_pos.y });
						atk_highlighted_cells.push_back({ int(cur_part_pos.x) - itr, int(cur_part_pos.y) });
						this->highlight_activate[(int)cur_part_pos.x - itr][(int)cur_part_pos.y] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
					}
					if (cur_part_pos.y + itr < MAX_J)
					{
						cbsptr->get_selected_cell().push_back({ cur_part_pos.x, cur_part_pos.y + itr });
						atk_highlighted_cells.push_back({ int(cur_part_pos.x), int(cur_part_pos.y) + itr });
						this->highlight_activate[(int)cur_part_pos.x][(int)cur_part_pos.y + itr] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
					}
					if (cur_part_pos.y - itr >= 0)
					{
						cbsptr->get_selected_cell().push_back({ cur_part_pos.x, cur_part_pos.y - itr });
						atk_highlighted_cells.push_back({ int(cur_part_pos.x), int(cur_part_pos.y) - itr });
						this->highlight_activate[(int)cur_part_pos.x][(int)cur_part_pos.y - itr] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
					}
				}
			}
			else
			{
				auto try_highlight = [&](int x, int y)
					{
						if (x < 0 || x >= MAX_I || y < 0 || y >= MAX_J)
							return;
						Components::RGBA& cell = this->highlight_activate[x][y];
						cell = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
						cbsptr->get_selected_cell().push_back({ f32(x), f32(y) });
						atk_highlighted_cells.push_back({ x, y });
					};

				for (int i = -range; i <= range; ++i)
				{
					int j_range = range - std::abs(i);
					for (int j = -j_range; j <= j_range; ++j)
					{
						try_highlight((int)cur_part_pos.x + i, (int)cur_part_pos.y + j);
					}
				}
			}
			break;
		}
	case highlight_tag::MOVE_HIGHLIGHT:
		{
			for (int i = -range; i <= range; ++i)
			{
				int j_range = range - std::abs(i);
				for (int j = -j_range; j <= j_range; ++j)
				{
					int tx = (int)cur_part_pos.x + i;
					int ty = (int)cur_part_pos.y + j;

					if (tx < 0 || tx >= MAX_I || ty < 0 || ty >= MAX_J)
						continue;

					//// Run A* from player to (tx, ty)
					//std::vector<AEVec2> path = astar(cur_part_pos, { tx, ty }, gbptr);
					Components::GridCell s{ s32(cur_part_pos.x), s32(cur_part_pos.y)};
					Components::GridCell g{ tx, ty };
					Components::AStarResult astar = AStar_FindPath_Grid4(MAX_I, MAX_J, gbptr->walkable, s, g);
					

					if (!astar.path.empty() && (int)astar.path.size() - 1 <= range)
					{
						this->highlight_activate[tx][ty] = HIGHLIGHT_MOV;
						cbsptr->get_selected_cell().push_back(AEVec2{ (f32)tx, (f32)ty });
						mov_highlighted_cells.push_back(Components::GridCell{ tx, ty });
					}
				}
			}		

			break;
		}
	case highlight_tag::COUNT:
		break;
	default:
		break;
	}
	//=========================================================================
}
void HighlightSystem::highlight_enemy_cells(Entity target)
{
	// wipe the activate arrays entirely first
	for (int i = 0; i < MAX_I; ++i)
		for (int j = 0; j < MAX_J; ++j)
		{
			enemy_mov_highlight_activate[i][j] = false;
			enemy_atk_highlight_activate[i][j] = false;
		}

	// then only set this target's cells
	if (enemy_mov_highlighted_cells.find(target) != enemy_mov_highlighted_cells.end())
		for (Components::GridCell& cell : enemy_mov_highlighted_cells.at(target))
			enemy_mov_highlight_activate[int(cell.x)][int(cell.y)] = true;

	if (enemy_atk_highlighted_cells.find(target) != enemy_atk_highlighted_cells.end())
		for (Components::GridCell& cell : enemy_atk_highlighted_cells.at(target))
			enemy_atk_highlight_activate[int(cell.x)][int(cell.y)] = true;
}

void HighlightSystem::unhighlight_cells()
{
	if (!cbsptr->get_selected_cell().empty())
	{
		for (AEVec2 a : cbsptr->get_selected_cell())
		{
			this->highlight_activate[int(a.x)][int(a.y)] = HIGHTLIGHT_UNHIGHLIGHT;
		}
	}
	cbsptr->get_selected_cell().clear();
	if(!atk_highlighted_cells.empty())
		this->atk_highlighted_cells.clear();
	if (!mov_highlighted_cells.empty())
		this->mov_highlighted_cells.clear();
	unhighlight_enemy_cells();

	unhighlight_aoe_cells();
}

void HighlightSystem::clear_enemy_cells()
{
	if (!enemy_mov_highlighted_cells.empty())
	{
		for (auto itr : enemy_mov_highlighted_cells)
		{
			for (Components::GridCell& cell : itr.second)
			{
				if(cell.x >= 0 && cell.y >= 0)
					enemy_mov_highlight_activate[int(cell.x)][int(cell.y)] = false;
			}
		}
		enemy_mov_highlighted_cells.clear();
	}
	if (!enemy_atk_highlighted_cells.empty())
	{
		for (auto itr : enemy_atk_highlighted_cells)
		{
			for (Components::GridCell& cell : itr.second)
			{
				if (cell.x >= 0 && cell.y >= 0)
					enemy_atk_highlight_activate[int(cell.x)][int(cell.y)] = false;
			}
		}
		enemy_atk_highlighted_cells.clear();
	}
}

void HighlightSystem::unhighlight_aoe_cells()
{
	if (aoe_highlighted_cells.empty())
		return;
	for (Components::GridCell& a : aoe_highlighted_cells)
	{
		aoe_highlight_activate[int(a.x)][int(a.y)] =  false;
	}
	if (!aoe_highlighted_cells.empty()) aoe_highlighted_cells.clear();
}

void HighlightSystem::unhighlight_atk_cells()
{
	if (enemy_atk_highlighted_cells.empty())
		return;
	for (Components::GridCell& a : atk_highlighted_cells)
	{
		Components::RGBA& rgba = this->highlight_activate[int(a.x)][int(a.y)];

		rgba.r = 0.f;
		rgba.g = 0.f;
		rgba.b = 0.f;
	}
	if(!cbsptr->get_selected_cell().empty())cbsptr->get_selected_cell().clear();
	atk_highlighted_cells.clear();
}
void HighlightSystem::unhighlight_mov_cells()
{
	for (Components::GridCell& a : mov_highlighted_cells)
	{
		Components::RGBA& rgba = this->highlight_activate[int(a.x)][int(a.y)];

		rgba.r = 0.f;
		rgba.g = 0.f;
		rgba.b = 0.f;
	}
	if (!cbsptr->get_selected_cell().empty())cbsptr->get_selected_cell().clear();
	mov_highlighted_cells.clear();
}

void HighlightSystem::unhighlight_enemy_cells()
{
	for (int i = 0; i < MAX_I; ++i)
		for (int j = 0; j < MAX_J; ++j)
		{
			enemy_mov_highlight_activate[i][j] = false;
			enemy_atk_highlight_activate[i][j] = false;
		}
}

void HighlightSystem::unhighlight_enemy_cells(Entity target)
{
	if (enemy_mov_highlighted_cells.find(target) != enemy_mov_highlighted_cells.end())
	{
		for (Components::GridCell& cell : enemy_mov_highlighted_cells.at(target))
		{
			enemy_mov_highlight_activate[int(cell.x)][int(cell.y)] = false;
		}
	}

	if (enemy_atk_highlighted_cells.find(target) != enemy_atk_highlighted_cells.end())
	{
		for (Components::GridCell& cell : enemy_atk_highlighted_cells.at(target))
		{
			enemy_atk_highlight_activate[int(cell.x)][int(cell.y)] = false;
		}
	}
}

void HighlightSystem::refresh_enmey_cells()
{
	for (auto it : enemy_atk_highlighted_cells)
	{
		for (Components::GridCell cell : it.second)
		{
			enemy_atk_highlight_activate[cell.x][cell.y] = true;
		}
	}
	for (auto it : enemy_mov_highlighted_cells)
	{
		for (Components::GridCell cell : it.second)
		{
			enemy_mov_highlight_activate[cell.x][cell.y] = true;
		}
	}
}

void HighlightSystem::refresh_enmey_cell(Entity id)
{
	if (enemy_atk_highlighted_cells.find(id) != enemy_atk_highlighted_cells.end())
	{
		auto it = enemy_atk_highlighted_cells[id];
		for (Components::GridCell cell : it)
		{
			enemy_atk_highlight_activate[cell.x][cell.y] = true;
		}
	}
	if (enemy_mov_highlighted_cells.find(id) != enemy_mov_highlighted_cells.end())
	{
		auto it = enemy_mov_highlighted_cells[id];
		for (Components::GridCell cell : it)
		{
			enemy_mov_highlight_activate[cell.x][cell.y] = true;
		}
	}
}

//void HighlightSystem::highlight_enemy_attack(Entity target,f32 range)
//{
//
//}

void HighlightSystem::update()
{
	if (evntptr->template_pool[UNHIGHLIGHT_EVENT].triggered)
	{
		evntptr->template_pool[UNHIGHLIGHT_EVENT].triggered = false;
		highlight_tag highlight_type = evntptr->template_pool[HIGHLIGHT_EVENT].returned_value;
		switch (highlight_type)
		{

			case highlight_tag::ATTACK_HIGHLIGHT: 
			{
				unhighlight_atk_cells();
				break;
			}
			case highlight_tag::MOVE_HIGHLIGHT :
			{
				unhighlight_mov_cells();
				break;
			}
			default :
			{
				//unhighlight_cells();
				break;
			}
		}
	}

	if (evntptr->template_pool[HIGHLIGHT_EVENT].triggered)
	{
		highlight_tag highlight_type = evntptr->template_pool[HIGHLIGHT_EVENT].returned_value;

		switch (highlight_type)
		{
		case highlight_tag::ATTACK_HIGHLIGHT:
		{
			Entity card_ID = cbsptr->draw_card(playerID, tbsptr->get_selected_cardhand_index());
			int& card_range = ecs.getComponent<Components::Targetting_Component>(card_ID)->range;

			Targetting targetting = ecs.getComponent<Components::Targetting_Component>(card_ID)->targetting_type;
			highlight_cells(targetting, card_range, highlight_type);

			break;
		}
		case highlight_tag::MOVE_HIGHLIGHT:
		{
			int& range = ecs.getComponent<Components::TurnBasedStats>(playerID)->cur_movSpd;
			highlight_cells(Targetting::SINGLE_TARGET, range, highlight_type);
		}
		break;
		default:
			break;
		}

		evntptr->template_pool[HIGHLIGHT_EVENT].triggered = false;
	}
}

void HighlightSystem::free()
{
	if (!cbsptr->get_selected_cell().empty())
	{
		for (AEVec2 a : cbsptr->get_selected_cell())
		{
			this->highlight_activate[int(a.x)][int(a.y)] = HIGHTLIGHT_UNHIGHLIGHT;
		}
	}
	cbsptr->get_selected_cell().clear();
	if (!atk_highlighted_cells.empty())
		this->atk_highlighted_cells.clear();
	if (!mov_highlighted_cells.empty())
		this->mov_highlighted_cells.clear();

	clear_enemy_cells();
	unhighlight_aoe_cells();
}