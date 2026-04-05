#include "pch.h"

#include "CombatSystem.h"
#include "TurnBasedSystem.h"
#include "../ECS/ECSystem.h"
#include "../system/PhaseSystem.h"
#include "../UI/cardInteraction.h"
#include "GridSystem.h"
#include "CardResolver.h"
#include "HightlightSystem.h"
#include <set>
#include <utility>
#include "../global.h"
#include "UI/IntentionDisplay.h"
#include "system/EnemyDirector.h"

//==================Helpers=================
std::vector<AEVec2>& CombatNameSpace::CombatSystem::get_selected_cell()
{
	return selected_cells;
}

std::vector<AEVec2>& CombatNameSpace::CombatSystem::get_aoe_selected_cell()
{
	return aoe_selected_cells;
}
namespace { //so no need to declare in .h file
	bool get_line_direction(const AEVec2& start, const AEVec2& end, int& dx, int& dy, int& distance)
	{
		int sx = static_cast<int>(start.x);
		int sy = static_cast<int>(start.y);
		int ex = static_cast<int>(end.x);
		int ey = static_cast<int>(end.y);

		int diffX = ex - sx;
		int diffY = ey - sy;

		// Cardinal directions only
		if (diffX != 0 && diffY != 0)
			return false;

		if (diffX == 0 && diffY == 0)
			return false;

		if (diffX != 0)
		{
			dx = (diffX > 0) ? 1 : -1;
			dy = 0;
			distance = std::abs(diffX);
		}
		else
		{
			dx = 0;
			dy = (diffY > 0) ? 1 : -1;
			distance = std::abs(diffY);
		}

		return true;
	}
}

bool is_valid_grid_pos(int x, int y)
{
	return x >= 0 && x < MAX_I && y >= 0 && y < MAX_J;
}

//-============END OF HELPERS=============
void CombatNameSpace::CombatSystem::init(PhaseSystem::GameBoardState& gbs, Grid::GameBoard& gb, TBS::TurnBasedSystem& tbs,
										CardInteraction::CardHand& cardhand, EventPool<highlight_tag>& eventSystem,
										HighlightSystem& hl,IntentionDisplaySystem& intent, EnemyDirector& enemyDirector)
{
	gbsptr = &gbs;
	gbptr = &gb;
	tbsptr = &tbs;
	cardHandptr = &cardhand;
	evsptr = &eventSystem;
	mfptr = &mf;
	tfptr = &TF;
	hlptr = &hl;
	intentptr = &intent;
	edptr = &enemyDirector;

	this->total_attack_cards_played = 0;
	this->total_def_cards_played = 0;
	this->total_item_cards_played = 0;
	this->total_world_interaction_card_played = 0;
}


bool CombatNameSpace::CombatSystem::check_within_range(s32 const& x, s32 const& y)
{
	for (AEVec2 ite : selected_cells)
	{
		if (x == s32(ite.x) && y == s32(ite.y))
		{
			return true;
		}
	}
	return false;
}

void CombatNameSpace::CombatSystem::play_attack_card(Entity caster, Entity cardID, Entity target, AEVec2 pos)
{
	EntityComponent::ComponentTypeID card_value_ID = EntityComponent::getComponentTypeID<Components::Card_Value>();
	if (!ecs.getBitMask()[cardID].test(card_value_ID))
	{
		std::cout << "Selected card doesn't have card_data component";
		return;
	}

	Components::Card_ID* cid = ecs.getComponent<Components::Card_ID>(cardID);
	if (!cid)
		return;

	Components::Targetting_Component* tgtComp = ecs.getComponent<Components::Targetting_Component>(cardID);
	if (!tgtComp)
		return;

	int serialID = cid->value;
	int family = (serialID / 100) % 10;

	f32 card_damage = ecs.getComponent<Components::Card_Value>(cardID)->value;

	// Turn bash special damage
	if (serialID == 1210)
	{
		card_damage = static_cast<f32>(tbsptr->round());
		std::cout << "[CombatSystem] Turn bash damage = " << card_damage << '\n';
	}

	f32 final_damage = card_damage;
	bool attackResolved = false;

	Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
	if (stats)
	{
		final_damage *= stats->atkMultiplier;
	}

	switch (tgtComp->targetting_type)
	{
	case Targetting::SINGLE_TARGET:
	{

		if (Call_AttackSystem(target, final_damage,*gbptr) != COMBAT_SYSTEM_RETURN_TAG::VALID)
		{
			std::cout << "Cannot damage the entity" << '\n';
		}
		else
		{
			attackResolved = true;
		}

		//if died push to graveyard;
		f32 targetHp = ecs.getComponent<Components::HP>(target)->c_value;
		if (targetHp <= 0.f)
		{
			graveyard.push_back({ pos, target });
		}
		break;
	}
	case Targetting::AOE:
	{
		for (AEVec2 pos : aoe_selected_cells)
		{
			Entity& ent = gbptr->get_pos()[(size_t)pos.x][(size_t)pos.y];
			if (ent != -1 && ent != tbsptr->current())
			{
				Components::Tag* tag = ecs.getComponent<Components::Tag>(ent);
				if (tag && *tag == Components::Tag::OTHERS)
				{
					continue; // skip rocks / mana wall / map objects
				}

				std::cout << "Hit Entity :" << ent << '\n';
				if (Call_AttackSystem(ent, final_damage, *gbptr) != COMBAT_SYSTEM_RETURN_TAG::VALID)
				{
					std::cout << "Cannot damage the entity" << '\n';
				}
				else
				{
					attackResolved = true;
				}
				//if died push to graveyard;
				f32 targetHp = ecs.getComponent<Components::HP>(ent)->c_value;
				if (targetHp <= 0.f)
				{
					graveyard.push_back({ pos,ent });
				}
			}
		}
		for (AEVec2 a : aoe_selected_cells)
		{
			hlptr->aoe_highlight_activate[int(a.x)][int(a.y)] = 0;
		}
		aoe_selected_cells.clear();
		break;
	}
	case Targetting::LINE:
	{
		s32 ax, ay;
		if (!gbptr->findEntityCell(caster, ax, ay))
		{
			std::cout << "Could not find caster on board.\n";
			break;
		}

		AEVec2 casterPos{ (f32)ax, (f32)ay };

		int dx = 0;
		int dy = 0;
		int distance = 0;

		if (!get_line_direction(casterPos, pos, dx, dy, distance))
		{
			std::cout << "LINE attack requires same row or column.\n";
			break;
		}

		int maxRange = static_cast<int>(tgtComp->range);

		if (distance > maxRange)
		{
			std::cout << "LINE attack out of range.\n";
			break;
		}

		// Shoot / Shoot+ : piercing
		if (family == 1)
		{
			AF.sfx.resetAudio();
			AF.sfx.play(7);
			for (int step = 1; step <= maxRange; ++step)
			{
				int cx = ax + dx * step;
				int cy = ay + dy * step;

				if (cx < 0 || cx >= MAX_I || cy < 0 || cy >= MAX_J)
					break;

				Entity& ent = gbptr->get_pos()[cx][cy];
				if (ent != -1 && ent != caster)
				{
					if (Call_AttackSystem(ent, final_damage,*gbptr) != COMBAT_SYSTEM_RETURN_TAG::VALID)
					{
						std::cout << "Cannot damage the entity" << '\n';
					}
					else
					{
						attackResolved = true;
					}

					f32 targetHp = ecs.getComponent<Components::HP>(ent)->c_value;
					if (targetHp <= 0.f)
					{
						graveyard.push_back({ AEVec2{ (f32)cx, (f32)cy }, ent });
					}
				}
			}
		}
		// Snipe : first enemy only
		else if (family == 2)
		{
			AF.sfx.resetAudio();
			AF.sfx.play(8); // sniper.wav
			for (int step = 1; step <= maxRange; ++step)
			{
				int cx = ax + dx * step;
				int cy = ay + dy * step;

				if (cx < 0 || cx >= MAX_I || cy < 0 || cy >= MAX_J)
					break;

				Entity& ent = gbptr->get_pos()[cx][cy];
				if (ent != -1 && ent != caster)
				{
					if (Call_AttackSystem(ent, final_damage,*gbptr) != COMBAT_SYSTEM_RETURN_TAG::VALID)
					{
						std::cout << "Cannot damage the entity" << '\n';
					}
					else
					{
						attackResolved = true;
					}

					f32 targetHp = ecs.getComponent<Components::HP>(ent)->c_value;
					if (targetHp <= 0.f)
					{
						graveyard.push_back({ AEVec2{ (f32)cx, (f32)cy }, ent });
					}
					break; // frontmost enemy blocks the shot
				}
			}
		}

		break;
	}
	default:
		break;
	}

	// STR DISK LOGIC (ATK BUFF)
	if (attackResolved && stats && stats->atkBuffHitsLeft > 0)
	{
		--stats->atkBuffHitsLeft;

		if (stats->atkBuffHitsLeft <= 0)
		{
			stats->atkBuffHitsLeft = 0;
			stats->atkBuffStacks = 0;
			stats->atkMultiplier = 1.0f;
		}

		std::cout << "[CombatSystem] ATK buff consumed. Multiplier = "
			<< stats->atkMultiplier
			<< " | Hits Left = "
			<< stats->atkBuffHitsLeft
			<< " | Stacks = "
			<< stats->atkBuffStacks << '\n';
	}
}

void CombatNameSpace::CombatSystem::handle_graveyard()
{
	//must reset the position on the grid to be null or there will be bugs
	/*if (targetted_x != -1 && targetted_y != -1) gameBoardptr->get_pos()[targetted_x][targetted_y] = -1;
	this->remove_participant(*ecsptr, targetted_entity);*/
	if (!graveyard.empty())
	{
		Components::Horde_Tag* goons = ecs.getComponent<Components::Horde_Tag>(tbsptr->get_participant()[1]);
		int i = 0;
		for (std::pair<AEVec2, Entity>& gra : graveyard)
		{
			AEVec2& targetPos = gra.first;

			Components::Animation_Actor* anim = ecs.getComponent<Components::Animation_Actor>(gra.second);

			if (!anim)
			{
				// Already destroyed somehow, just remove from graveyard
				graveyard[i] = graveyard.back();
				graveyard.pop_back();
				edptr->removeEnemy(gra.second);
				hlptr->unhighlight_enemy_cells(gra.second);
				continue;
			}
			
			if (targetPos.x != -1 && targetPos.y != -1) gbptr->get_pos()[(int)targetPos.x][(int)targetPos.y] = -1;
			gbptr->walkable[int(targetPos.y) * MAX_I + int(targetPos.x)] = 1;

			if (goons->alive() && anim->prev_type != Components::AnimationType::DEATH && anim->anim_type != Components::AnimationType::DEATH)
			{
				anim->setType(Components::AnimationType::DEATH);
				goons->remove_goon(gra.second);
			}

			if(anim->prev_type == Components::AnimationType::DEATH){
				gbptr->walkable[int(targetPos.y) * MAX_I + int(targetPos.x)] = 1;
				ecs.destroyEntity(gra.second);
				if(graveyard.size() > 0){

					std::pair<AEVec2, Entity> temp = graveyard[graveyard.size() - 1];
					graveyard[graveyard.size() - 1] = gra;
					graveyard[i] = temp;
					graveyard.pop_back();
					hlptr->unhighlight_enemy_cells(gra.second);
					edptr->removeEnemy(gra.second);
					}
				else
				{
					graveyard.pop_back();
				}
			}
			else
			{
				i++;
			}
		}
	}
}

// non piercing long ranged atk
bool CombatNameSpace::CombatSystem::resolve_line_attack_first_hit(
	Entity caster,
	const AEVec2& startPos,
	const AEVec2& targetPos,
	int maxRange,
	f32 damage)
{
	int dx = 0, dy = 0, distance = 0;
	if (!get_line_direction(startPos, targetPos, dx, dy, distance))
		return false;

	if (distance > maxRange)
		return false;

	for (int step = 1; step <= maxRange; ++step)
	{
		int cx = static_cast<int>(startPos.x) + dx * step;
		int cy = static_cast<int>(startPos.y) + dy * step;

		if (!is_valid_grid_pos(cx, cy))
			break;

		Entity ent = gbptr->get_pos()[cx][cy];
		if (ent != Components::NULL_INDEX && ent != -1 && ent != caster)
		{
			if (Call_AttackSystem(ent, damage,*gbptr) == COMBAT_SYSTEM_RETURN_TAG::VALID)
			{
				f32 targetHp = ecs.getComponent<Components::HP>(ent)->c_value;
				if (targetHp <= 0.f)
				{
					graveyard.push_back({ AEVec2{ static_cast<f32>(cx), static_cast<f32>(cy) }, ent });
				}
				return true;
			}
			return false;
		}
	}

	return false;
}

//piercing long ranged atk
bool CombatNameSpace::CombatSystem::resolve_line_attack_pierce(
	Entity caster,
	const AEVec2& startPos,
	const AEVec2& targetPos,
	int maxRange,
	f32 damage)
{
	int dx = 0, dy = 0, distance = 0;
	if (!get_line_direction(startPos, targetPos, dx, dy, distance))
		return false;

	if (distance > maxRange)
		return false;

	bool hitAnything = false;

	for (int step = 1; step <= maxRange; ++step)
	{
		int cx = static_cast<int>(startPos.x) + dx * step;
		int cy = static_cast<int>(startPos.y) + dy * step;

		if (!is_valid_grid_pos(cx, cy))
			break;

		Entity ent = gbptr->get_pos()[cx][cy];
		if (ent != Components::NULL_INDEX && ent != -1 && ent != caster)
		{
			if (Call_AttackSystem(ent, damage,*gbptr) == COMBAT_SYSTEM_RETURN_TAG::VALID)
			{
				hitAnything = true;

				f32 targetHp = ecs.getComponent<Components::HP>(ent)->c_value;
				if (targetHp <= 0.f)
				{
					graveyard.push_back({ AEVec2{ static_cast<f32>(cx), static_cast<f32>(cy) }, ent });
				}
			}
		}
	}

	return hitAnything;
}

COMBAT_SYSTEM_RETURN_TAG Call_AttackSystem(Entity target, f32 damage,Grid::GameBoard& gb)
{
	EntityComponent::ComponentTypeID hpID = EntityComponent::getComponentTypeID<Components::HP>();

	// Check in enity have hp
	Components::HP* hp = ecs.getComponent<Components::HP>(target);
	if (!hp)
		return COMBAT_SYSTEM_RETURN_TAG::INVALID;

	//test if card have attack id
	Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(target);

	// Aura Farm: ignore all damage
	if (stats)
	{
		damage *= stats->damageTakenMultiplier;
	}

	// Shield absorbs damage first
	if (stats && stats->shields > 0)
	{
		if (stats->shields >= static_cast<int>(damage))
		{
			stats->shields -= static_cast<int>(damage);
			damage = 0.0f;
		}
		else
		{
			damage -= static_cast<f32>(stats->shields);
			stats->shields = 0;
		}
	}

	// Leftover damage hits HP
	hp->c_value -= damage;

	PUT.display(target, std::to_string(static_cast<int>(damage)).c_str());
	auto anim = ecs.getComponent<Components::Animation_Actor>(target);
	if (anim && damage)
	{
		anim->setType(Components::AnimationType::TAKING_DAMAGE);
	}
	std::cout << "[Combat] Target: " << target;
	if (stats)
	{
		std::cout << " | Shield After: " << stats->shields;
	}
	std::cout << " | HP After: " << hp->c_value << '\n';

	s32 x, y;
	gb.findEntityCell(target, x, y);
	f32 particle_x; f32 width = 40.f;
	f32 particle_y; f32 height = 20.f;
	translate_To_Isometric(gb.GetOffsetPos(), height, particle_x, particle_y, x, y);
	PS.particleHeal(particle_x, particle_y, width, height);	//here heal is placeholder



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

void CombatNameSpace::CombatSystem::update()
{
	update_GBPhasetriggered();
	update_GBPhaseUpdate();


	if (play_card_triggered)
	{
		play_card_triggered = false;

		PC_RETURN_TAG tag = this->play_card(tbsptr->current(), targetted_entity, { f32(targetted_x),f32(targetted_y) }
		, tbsptr->get_selected_cardhand_index());

		if (tag != PC_RETURN_TAG::INVALID)
		{
			std::cout << "Played an invalid cardtype" << '\n';
			tbsptr->set_selected_card(false);
		}

		gbsptr->set_PlayerPhase(PhaseSystem::PlayerPhase::PLAYER_EXPLORE);
		evsptr->template_pool[UNHIGHLIGHT_EVENT].triggered = true;
		gbsptr->set_GBPhase(PhaseSystem::GBPhase::MAIN_PHASE);
	}

	handle_graveyard();
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
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << '\n';
			gbsptr->GBPTriggered()[index] = false;

			//=============rest shit for player in start phase===============

			Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(playerID);
			stats->cur_movSpd = stats->max_movSpd;
			stats->points = stats->maxPoints;
			//stats->shields = 0.f;
			stats->damageTakenMultiplier = 1.0f;

			//===============================================================

			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			IT << "STARTING!";
			break;
		}
		case PhaseSystem::GBPhase::STANDBY_PHASE:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << '\n';
			gbsptr->GBPTriggered()[index] = false;
			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::DRAW_PHASE:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << '\n';
			gbsptr->GBPTriggered()[index] = false;
			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			IT << "Draw card...";
			break;
		}
		case PhaseSystem::GBPhase::MAIN_PHASE:
		{
			//std::cout << "triggered " << PhaseSystem::GBPhaseNames[index] << '\n';
			gbsptr->GBPTriggered()[index] = false;

			if(tbsptr->active())
			tbsptr->debug_print();

			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			IT << "YOUR TURN!";
			break;
		}
		default:
		{
			std::cout << "triggered something else";
			gbsptr->GBPTriggered()[index] = false;
			gbsptr->GBPActive()[prev_index] = false;
			gbsptr->GBPActive()[index] = true;
			IT << "ENEMY TURN!";
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
			//std::cout << "Starting" << '\n';
			gbsptr->nextGBPhase();
			index = static_cast<size_t>(gbsptr->getGBPhase());
			gbsptr->GBPTriggered()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::STANDBY_PHASE:
		{
			//std::cout << "Standing By"<< '\n';
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
				tbsptr->DrawPhase_add_card();
			}
			cardHandptr->reset_hand();

			gbsptr->nextGBPhase();
			index = static_cast<size_t>(gbsptr->getGBPhase());
			gbsptr->GBPTriggered()[index] = true;
			break;
		}
		case PhaseSystem::GBPhase::MAIN_PHASE:
		{
			break; 
		}
		/*case PhaseSystem::GBPhase::PLAYER_RESOLUTION:
		{
			if ((gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::PLAYER_ANIMATION && gbsptr->getPrevPlayerPhase() == PhaseSystem::PlayerPhase::GRID_SELECT) ||
				(gbsptr->getPlayerPhase() == PhaseSystem::PlayerPhase::PLAYER_ANIMATION && gbsptr->getPrevPlayerPhase() == PhaseSystem::PlayerPhase::AOE_GRID_SELECT))
			{
				if (!play_card_triggered)
				{
					std::cout << "Card Animating" << '\n';
				}
				if (play_card_triggered)
				{
					play_card_triggered = false;

					PC_RETURN_TAG tag = tbsptr->play_card(tbsptr->current(), targetted_entity, { f32(targetted_x),f32(targetted_y) }
					, tbsptr->get_selected_cardhand_index());

					if (tag != PC_RETURN_TAG::INVALID)
					{
						tbsptr->set_selected_card(false);
					}

					end_player_resolution();
				}
			}
			break;
		}*/
		default:
		{
			std::cout << "Updating error";
			break;
		}
		}
	}
}

//returns the status of target being attacked
PC_RETURN_TAG CombatNameSpace::CombatSystem::play_card(Entity player, Entity target, AEVec2 targetted_pos, int index)
{
	PC_RETURN_TAG ret = PC_RETURN_TAG::INVALID;

	Entity cardID = this->draw_card(player, index);
	if (cardID == Components::NULL_INDEX) // Added a guard 
		return PC_RETURN_TAG::INVALID;

	Components::Targetting_Component* targetting = ecs.getComponent<Components::Targetting_Component>(cardID);

	int& aoe_range = ecs.getComponent<Components::Targetting_Component>(cardID)->aoe;

	if (!aoe_selected_cells.empty()) aoe_selected_cells.clear();
	s32 x = gbptr->cur_x; s32 y = gbptr->cur_y;

	std::set<std::pair<int, int>> selected;

	for (int i = -(int)aoe_range; i <= (int)aoe_range; ++i)
	{
		for (int j = -(int)aoe_range; j <= (int)aoe_range; ++j)
		{
			if (std::abs(i) + std::abs(j) > (int)aoe_range) continue;

			int cx = x + i;
			int cy = y + j;

			if (cx >= 0 && cx < MAX_I && cy >= 0 && cy < MAX_J)
				selected.insert({ cx, cy });
		}
	}

	for (auto& [cx, cy] : selected)
		this->aoe_selected_cells.push_back({ f32(cx), f32(cy) });

	//remove the card that just played inside tbs
	int& card_cost = ecs.getComponent<Components::Card_Cost>(cardID)->value;
	int& player_curMana = ecs.getComponent<Components::TurnBasedStats>(player)->points;

	if (card_cost > player_curMana) // Added a not enough mana condition
	{
		std::cout << "Not enough mana!!" << '\n';
		return PC_RETURN_TAG::INVALID;
	}

	ret = CardResolver::resolve(
		*this,
		*gbptr,
		*tbsptr,
		*intentptr,
		player,
		cardID,
		target,
		targetted_pos
	);

	// Made it a guard for you Steven - Zejin
	if (ret == PC_RETURN_TAG::VALID)
	{
		player_curMana -= static_cast<int>(card_cost);
		this->remove_card(player, index);
	}

	return ret;
}

void CombatNameSpace::CombatSystem::remove_card(Entity user, int index)
{
	EntityFactory::remove_card_player(user, index);	//this is to remove data from ecs
	cardHandptr->remove_card(index);		//this is for visual side
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

void CombatNameSpace::CombatSystem::combatSystem_free()
{
	horde = -1;
	gbsptr = nullptr;
	gbptr = nullptr;
	tbsptr = nullptr;
	cardHandptr = nullptr;
	evsptr = nullptr;
	hlptr = nullptr;
	intentptr = nullptr;
	edptr = nullptr;

	targetted_entity = -1;
	targetted_x = -1; targetted_y= -1;
	play_card_triggered = false;

	if(!selected_cells.empty())selected_cells.clear();
	if(!aoe_selected_cells.empty())aoe_selected_cells.clear();
	if(!graveyard.empty())graveyard.clear();
}

std::vector<std::pair<AEVec2, Entity>>& CombatNameSpace::CombatSystem::get_graveyard()
{
	return graveyard;
}

//return the cardID inside the hand
Entity CombatNameSpace::CombatSystem::draw_card(Entity player, size_t chIndex)
{
	Components::Card_Storage* player_storage = ecs.getComponent<Components::Card_Storage>(player);
	if (player_storage->data_card_hand.empty()) return -1;
	return player_storage->data_card_hand[chIndex];
}