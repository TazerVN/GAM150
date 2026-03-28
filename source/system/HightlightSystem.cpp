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
						this->highlight_activate[cur_part_pos.x + itr][cur_part_pos.y] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
					}
					if (cur_part_pos.x - itr >= 0)
					{
						cbsptr->get_selected_cell().push_back({ cur_part_pos.x - itr, cur_part_pos.y });
						this->highlight_activate[cur_part_pos.x - itr][cur_part_pos.y] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
					}
					if (cur_part_pos.y + itr < MAX_J)
					{
						cbsptr->get_selected_cell().push_back({ cur_part_pos.x, cur_part_pos.y + itr });
						this->highlight_activate[cur_part_pos.x][cur_part_pos.y + itr] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
					}
					if (cur_part_pos.y - itr >= 0)
					{
						cbsptr->get_selected_cell().push_back({ cur_part_pos.x, cur_part_pos.y - itr });
						this->highlight_activate[cur_part_pos.x][cur_part_pos.y - itr] = HIGHT_LIGHT_VALUES[static_cast<size_t>(type)];
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
					};

				for (int i = -range; i <= range; ++i)
				{
					int j_range = range - std::abs(i);
					for (int j = -j_range; j <= j_range; ++j)
					{
						try_highlight(cur_part_pos.x + i, cur_part_pos.y + j);
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
					int tx = cur_part_pos.x + i;
					int ty = cur_part_pos.y + j;

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

void HighlightSystem::unhighlight_cells()
{
	if (cbsptr->get_selected_cell().empty()) return;
	for (AEVec2 a : cbsptr->get_selected_cell())
	{
		this->highlight_activate[int(a.x)][int(a.y)] = HIGHTLIGHT_UNHIGHLIGHT;
	}
	cbsptr->get_selected_cell().clear();
}

void HighlightSystem::update()
{
	if (evntptr->template_pool[UNHIGHLIGHT_EVENT].triggered)
	{
		unhighlight_cells();
		evntptr->template_pool[UNHIGHLIGHT_EVENT].triggered = false;
	}

	if (evntptr->template_pool[HIGHLIGHT_EVENT].triggered)
	{
		highlight_tag highlight_type = evntptr->template_pool[HIGHLIGHT_EVENT].returned_value;

		switch (highlight_type)
		{
		case highlight_tag::ATTACK_HIGHLIGHT:
		{
			Entity card_ID = cbsptr->draw_card(playerID, tbsptr->get_selected_cardhand_index());
			f32& card_range = ecs.getComponent<Components::Targetting_Component>(card_ID)->range;

			Targetting targetting = ecs.getComponent<Components::Targetting_Component>(card_ID)->targetting_type;
			highlight_cells(targetting, card_range, highlight_type);

			break;
		}
		case highlight_tag::MOVE_HIGHLIGHT:
		{
			f32& range = ecs.getComponent<Components::TurnBasedStats>(playerID)->cur_movSpd;
			highlight_cells(Targetting::SINGLE_TARGET, range, highlight_type);
		}
		break;
		default:
			break;
		}

		evntptr->template_pool[HIGHLIGHT_EVENT].triggered = false;
	}
}