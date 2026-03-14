#include "pch.h"

#include "CombatSystem.h"
#include "../system/TurnBasedSystem.h"
#include "../ECS/ECSystem.h"
#include "../system/PhaseSystem.h"
#include "../UI/cardInteraction.h"
#include "../system/GridSystem.h"
#include "../global.h"

std::vector<AEVec2>& CombatNameSpace::CombatSystem::get_highlighted_cell()
{
	return highlighted_cells;
}

std::vector<AEVec2>& CombatNameSpace::CombatSystem::get_aoe_highlighted_cell()
{
	return aoe_highlighted_cells;
}


void CombatNameSpace::CombatSystem::init(EntityComponent::Registry& ecs, PhaseSystem::GameBoardState& gbs, Grid::GameBoard& gb, TBS::TurnBasedSystem& tbs,
	CardInteraction::CardHand& cardhand, EventPool<highlight_tag>& eventSystem)
{
	ecsptr = &ecs;
	gbsptr = &gbs;
	gbptr = &gb;
	tbsptr = &tbs;
	cardHandptr = &cardhand;
	evsptr = &eventSystem;
}


bool CombatNameSpace::CombatSystem::check_within_range(Entity id, s32 const& x, s32 const& y)
{
	for (AEVec2 ite : highlighted_cells)
	{
		if (x == s32(ite.x) && y == s32(ite.y))
		{
			return true;
		}
	}
	return false;
}

void CombatNameSpace::CombatSystem::play_attack_card(EntityComponent::Registry& ecs, Entity cardID, Entity target, AEVec2 pos)
{
	EntityComponent::ComponentTypeID card_value_ID = EntityComponent::getComponentTypeID<Components::Card_Value>();
	if (!ecs.getBitMask()[cardID].test(card_value_ID))
	{
		std::cout << "Selected card doesn't have card_data component";
		return;
	}
	f32 card_damage = ecs.getComponent<Components::Card_Value>(cardID)->value;

	switch (ecs.getComponent<Components::Targetting_Component>(cardID)->targetting_type)
	{
	case Targetting::SINGLE_TARGET:
	{

		if (Call_AttackSystem(ecs, target, card_damage) != COMBAT_SYSTEM_RETURN_TAG::VALID)
		{
			std::cout << "Cnnot damage the entity" << std::endl;
		}

		//if died push to graveyard;
		f32 targetHp = ecs.getComponent<Components::HP>(target)->c_value;
		if (targetHp <= 0.f)
		{
			graveyard.push_back(target);
		}
		break;
	}
	case Targetting::AOE:
	{
		int i = 0;
		for (AEVec2 pos : aoe_highlighted_cells)
		{
			Entity& ent = gbptr->get_pos()[pos.x][pos.y];
			if (ent != -1 && ent != tbsptr->current())
			{
				std::cout << "Hit Entity :" << ent << std::endl;
				if (Call_AttackSystem(ecs, ent, card_damage) != COMBAT_SYSTEM_RETURN_TAG::VALID)
				{
					std::cout << "Cnnot damage the entity" << std::endl;
				}
				//if died push to graveyard;
				f32 targetHp = ecs.getComponent<Components::HP>(ent)->c_value;
				if (targetHp <= 0.f)
				{
					graveyard.push_back(ent);
				}
			}
		}	
	}
	default:
		break;
	}
}

void CombatNameSpace::CombatSystem::handle_graveyard()
{
	//must reset the position on the grid to be null or there will be bugs
	/*if (targetted_x != -1 && targetted_y != -1) gameBoardptr->get_pos()[targetted_x][targetted_y] = -1;
	this->remove_participant(*ecsptr, targetted_entity);*/
}

COMBAT_SYSTEM_RETURN_TAG Call_AttackSystem(EntityComponent::Registry& ecs, Entity target, f32 damage)
{
	EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();
	//test if card have attack id
	if (!(ecs.getBitMask()[target].test(hpID))) return COMBAT_SYSTEM_RETURN_TAG::INVALID;

	//if the have components then reduce the HP amount
	ecs.getComponent<Components::HP>(target)->c_value -= damage;


	return COMBAT_SYSTEM_RETURN_TAG::VALID;
}

void CombatNameSpace::CombatSystem::set_targetted_ent(Entity ent)
{
	targetted_entity = ent;
}

void CombatNameSpace::CombatSystem::set_targetted_xy(int x, int y)
{
	targetted_x = x; targetted_y = y;
}

void CombatNameSpace::CombatSystem::set_play_card_triggered(bool flag)
{
	play_card_triggered = flag;
}

void CombatNameSpace::CombatSystem::update_GBPhasetriggered()
{
	//
	int index = static_cast<int>(gbsptr->getGBPhase());
	int prev_index = index - 1;
	if (prev_index < 0) prev_index = 0;
	bool& triggered = gbsptr->GBPTriggered()[index];

	//check for gameboard phase and react accordingly
	if (triggered)
	{
		switch (gbsptr->getGBPhase())
		{
		case PhaseSystem::GBPhase::START_PHASE:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
			gbsptr->GBPTriggered()[index] = false;

			//=============rest shit for player in start phase===============

			Components::TurnBasedStats* stats = ecsptr->getComponent<Components::TurnBasedStats>(playerID);
			stats->cur_movSpd = stats->max_movSpd;
			stats->points = stats->maxPoints;
			stats->shields = 0.f;

			//===============================================================

			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::STANDBY_PHASE:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
			gbsptr->GBPTriggered()[index] = false;
			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::DRAW_PHASE:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
			gbsptr->GBPTriggered()[index] = false;
			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::MAIN_PHASE:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
			gbsptr->GBPTriggered()[index] = false;

			tbsptr->debug_print(*ecsptr);

			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::PLAYER_RESOLUTION:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << std::endl;
			gbsptr->GBPTriggered()[index] = false;
			gbsptr->GBPActive()[prev_index] = false;
			//===================play card==============================

			gbsptr->GBPActive()[index] = true;
			//=========================================================
			break;
		}
		default:
		{
			std::cout << "triggered something else";
			gbsptr->GBPTriggered()[index] = false;
			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			break;
		}
		}
		triggered = false;
	}
}

void CombatNameSpace::CombatSystem::update_GBPhaseUpdate()
{
	size_t index = static_cast<size_t>(gbsptr->getGBPhase());
	bool& active = gbsptr->GBPActive()[index];

	if (active)
	{
		switch (gbsptr->getGBPhase())
		{
		case PhaseSystem::GBPhase::START_PHASE:
		{
			//std::cout << "Starting" << std::endl;
			gbsptr->nextGBPhase();
			index = static_cast<size_t>(gbsptr->getGBPhase());
			gbsptr->GBPTriggered()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::STANDBY_PHASE:
		{
			//std::cout << "Standing By"<< std::endl;
			gbsptr->nextGBPhase();
			index = static_cast<size_t>(gbsptr->getGBPhase());
			gbsptr->GBPTriggered()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::DRAW_PHASE:
		{
			//draw until max_hand
			for (int i = 0; i < Components::DRAW_COUNT; ++i)
			{
				tbsptr->DrawPhase_add_card(*ecsptr);
			}
			cardHandptr->reset_hand();

			gbsptr->nextGBPhase();
			index = static_cast<size_t>(gbsptr->getGBPhase());
			gbsptr->GBPTriggered()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::MAIN_PHASE:
		{
			if (tbsptr->current() == playerID)
			{
				switch (gbsptr->getPlayerPhase())
				{
				case PhaseSystem::PlayerPhase::PLAYER_EXPLORE:
				{
	
				}
				default:
					break;
				}
			}
			//this is break for the GPhase
			break;
		}
		case PhaseSystem::GBPhase::PLAYER_RESOLUTION:
		{
			//example sample case where animation player can work
			/*if (update_animation())
			{
				play_card_triggered = true;
			}*/

			//will remove below code once the animation machine is done

			if (gbsptr->getPrevPlayerPhase() == PhaseSystem::PlayerPhase::PLAYER_EXPLORE)
			{
				std::cout << "Movement Animating" << std::endl;
				if (AEInputCheckTriggered(AEVK_P))
				{
					end_player_resolution();
				}
				//end_player_resolution();
			}
			else //else the previous player phase is grid select or aoe grid select
			{
				if (!play_card_triggered)
				{
					std::cout << "Card Animating" << std::endl;
					if (AEInputCheckTriggered(AEVK_P))
					{
						play_card_triggered = true;
					}
					//play_card_triggered = true;
				}
				if (play_card_triggered)
				{
					play_card_triggered = false;

					PC_RETURN_TAG tag = tbsptr->play_card(*ecsptr, tbsptr->current(), targetted_entity, { f32(targetted_x),f32(targetted_y) }
					, tbsptr->get_selected_cardhand_index());

					////remove the card that just played inside tbs
					//if (tag == PC_RETURN_TAG::DIED)
					//{
					//	//must reset the position on the grid to be null or there will be bugs
					//	if (targetted_x != -1 && targetted_y != -1) gameBoardptr->get_pos()[targetted_x][targetted_y] = -1;
					//	this->remove_participant(*ecsptr, targetted_entity);
					//}

					if (tag != PC_RETURN_TAG::INVALID)
					{
						tbsptr->set_selected_card(false);
					}

					end_player_resolution();
				}
			}
			break;
		}
		default:
		{
			std::cout << "Updating error";
			break;
		}
		}
	}
}

void CombatNameSpace::CombatSystem::end_player_resolution()
{
	gbsptr->GBPActive()[static_cast<int>(PhaseSystem::GBPhase::PLAYER_RESOLUTION)] = false;
	//remove if u want to make the the playerphase not reset when u click on invalid target
	gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
	//gbsptr->debug_print();
	evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;

	gbsptr->set_GBPhase(PhaseSystem::GBPhase::MAIN_PHASE);
	int i = static_cast<int>(gbsptr->getGBPhase());
	gbsptr->GBPTriggered()[i] = true;
}