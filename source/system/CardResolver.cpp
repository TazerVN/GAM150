#include "pch.h"
#include "CardResolver.h"

namespace
{
	int get_category(int id)
	{
		return id / 1000;
	}

	int get_family(int id)
	{
		return (id / 100) % 10;
	}

	int get_targetting_code(int id)
	{
		return (id / 10) % 10;
	}

	int get_variant(int id)
	{
		return id % 10;
	}
}

namespace CardResolver
{
	PC_RETURN_TAG resolve(
		EntityComponent::Registry& ecs,
		CombatNameSpace::CombatSystem& combatSystem,
		Grid::GameBoard& board,
		TBS::TurnBasedSystem& tbs,
		Entity caster,
		Entity cardID,
		Entity target,
		AEVec2 targetPos)
	{
		(void)board;
		(void)targetPos;

		if (cardID == Components::NULL_INDEX)
			return PC_RETURN_TAG::INVALID;

		Components::Card_ID* idComp = ecs.getComponent<Components::Card_ID>(cardID);
		Components::Card_Value* value = ecs.getComponent<Components::Card_Value>(cardID);
		Components::Name* name = ecs.getComponent<Components::Name>(cardID);

		if (!idComp || !value || !name)
			return PC_RETURN_TAG::INVALID;

		int serialID = idComp->value;

		int category = get_category(serialID);
		int family = get_family(serialID);
		int targettingCode = get_targetting_code(serialID);
		int variant = get_variant(serialID);

		std::cout << "[CardResolver] Resolving card: " << name->value
			<< " | ID: " << serialID
			<< " | category: " << category
			<< " | family: " << family
			<< " | target: " << targettingCode
			<< " | variant: " << variant << std::endl;

		switch (category)
		{
			//-----------------------------------
			// 1XXX = ATTACK
			//-----------------------------------
		case 1:
		{
			// For now all attack families still go through existing combat attack logic
			combatSystem.play_attack_card(ecs, cardID, target, targetPos);
			return PC_RETURN_TAG::VALID;
		}

		//-----------------------------------
		// 2XXX = DEFENSE
		//-----------------------------------
		case 2:
		{
			Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
			if (!stats)
				return PC_RETURN_TAG::INVALID;

			switch (family)
			{
			case 0: // Shield
				stats->shields += value->value;
				return PC_RETURN_TAG::VALID;

			case 1: // Damage reduction / invincibility family
				// TODO: implement proper reduction/invincibility stat when your team adds it
				std::cout << "[CardResolver] Defense family 1 not fully implemented yet.\n";
				return PC_RETURN_TAG::VALID;

			default:
				return PC_RETURN_TAG::INVALID;
			}
		}

		//-----------------------------------
		// 3XXX = ITEM
		//-----------------------------------
		case 3:
		{
			switch (family)
			{
			case 0: // Healing
			{
				Components::HP* hp = ecs.getComponent<Components::HP>(caster);
				if (!hp)
					return PC_RETURN_TAG::INVALID;

				hp->c_value += value->value;
				return PC_RETURN_TAG::VALID;
			}

			case 1: // PP buff
			{
				Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
				if (!stats)
					return PC_RETURN_TAG::INVALID;

				stats->points += static_cast<int>(value->value);
				return PC_RETURN_TAG::VALID;
			}

			case 2: // Movement buff
			{
				Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
				if (!stats)
					return PC_RETURN_TAG::INVALID;

				stats->cur_movSpd += value->value;
				return PC_RETURN_TAG::VALID;
			}

			case 3: // ATK buff
			{
				// TODO: implement proper attack stat when your team adds it
				std::cout << "[CardResolver] ATK buff not fully implemented yet.\n";
				return PC_RETURN_TAG::VALID;
			}

			case 4: // Draw / generate family
			{
				int count = static_cast<int>(value->value);
				for (int i = 0; i < count; ++i)
				{
					tbs.DrawPhase_add_card(ecs);
				}
				return PC_RETURN_TAG::VALID;
			}

			default:
				return PC_RETURN_TAG::INVALID;
			}
		}

		//-----------------------------------
		// 4XXX = EVENT
		//-----------------------------------
		case 4:
		{
			switch (family)
			{
			case 0: // Absorb / pull
				std::cout << "[CardResolver] Absorb event triggered\n";
				return PC_RETURN_TAG::VALID;

			case 1: // Push
				std::cout << "[CardResolver] Push event triggered\n";
				return PC_RETURN_TAG::VALID;

			default:
				return PC_RETURN_TAG::INVALID;
			}
		}

		default:
			return PC_RETURN_TAG::INVALID;
		}
	}
}