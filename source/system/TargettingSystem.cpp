//#include "TargettingSystem.h"
//
//namespace TargettingSystem
//{
//	void highlight_cells(ECS::Registry& ecs,TBS::TurnBasedSystem& tbs,Grid::GameBoard& gb)
//	{
//		//=========================Highlight_cells=================================
//		f32& card_range = ecs.getComponent<Components::Attack>(tbs.get_selected_card())->range;
//
//		AEVec2 cur_part_pos = gb.get_CurPart_gridPos(ecs,tbs);
//
//		//iterate over range * 2 
//		for (int ite = 1; ite <= card_range; ++ite)
//		{
//			gb.get_highlighted_cell().push_back({ cur_part_pos.x + ite , cur_part_pos.y });
//			gb.get_highlighted_cell().push_back({ cur_part_pos.x , cur_part_pos.y + ite });
//			gb.get_highlighted_cell().push_back({ cur_part_pos.x - ite , cur_part_pos.y });
//			gb.get_highlighted_cell().push_back({ cur_part_pos.x , cur_part_pos.y - ite });
//			  
//			gb.get_attack_activate()[cur_part_pos.x + ite][cur_part_pos.y] = true;
//			gb.get_attack_activate()[cur_part_pos.x][cur_part_pos.y + ite] = true;
//			gb.get_attack_activate()[cur_part_pos.x - ite][cur_part_pos.y] = true;
//			gb.get_attack_activate()[cur_part_pos.x][cur_part_pos.y - ite] = true;
//		}
//		//=========================================================================
//	}
//
//
//	void unhighlight_cells(Grid::GameBoard& gb)
//	{
//		{
//			//un-highligh cells
//			for (AEVec2 a : gb.get_highlighted_cell())
//			{
//				gb.get_attack_activate()[a.x][a.y] = false;
//				gb.get_highlighted_cell().clear();
//			}
//		}
//	}
//}
