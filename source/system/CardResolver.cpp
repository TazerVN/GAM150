#include "pch.h"
#include "CardResolver.h"

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

		if (cardID == Components::NULL_INDEX)
			return PC_RETURN_TAG::INVALID;

		CardTag* tag = ecs.getComponent<CardTag>(cardID);
		Components::Card_Value* value = ecs.getComponent<Components::Card_Value>(cardID);

		if (!tag || !value)
			return PC_RETURN_TAG::INVALID;

		switch (*tag)
		{
		case CardTag::ATTACK:
		{
			combatSystem.play_attack_card(ecs, cardID, target, targetPos);
			return PC_RETURN_TAG::VALID;
		}

		// CHANGE THIS ENUM NAME to whatever your real defensive tag is
		// case CardTag::DEF:
		// case CardTag::DEFENSE:
		// case CardTag::DEFEND:
		// {
		//     Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
		//     if (!stats) return PC_RETURN_TAG::INVALID;
		//
		//     switch (value->type)
		//     {
		//     case CardType::SHIELDING:
		//         stats->shields += static_cast<int>(value->value);
		//         return PC_RETURN_TAG::VALID;
		//     default:
		//         return PC_RETURN_TAG::INVALID;
		//     }
		// }

		case CardTag::ITEM:
		{
			switch (value->type)
			{
			case CardType::HEALING:
			{
				Components::HP* hp = ecs.getComponent<Components::HP>(caster);
				if (!hp)
					return PC_RETURN_TAG::INVALID;

				hp->c_value += value->value;
				return PC_RETURN_TAG::VALID;
			}

			case CardType::PP_BUFF:
			{
				Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
				if (!stats)
					return PC_RETURN_TAG::INVALID;

				stats->points += static_cast<int>(value->value);
				return PC_RETURN_TAG::VALID;
			}

			case CardType::MOV_BUFF:
			{
				Components::TurnBasedStats* stats = ecs.getComponent<Components::TurnBasedStats>(caster);
				if (!stats)
					return PC_RETURN_TAG::INVALID;

				stats->cur_movSpd += value->value;
				return PC_RETURN_TAG::VALID;
			}

			case CardType::DRAWING_CARD:
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

		case CardTag::EVENT:
		{
			switch (value->type)
			{
			case CardType::FORCED_MOVEMENT:
				std::cout << "[CardResolver] Forced movement triggered\n";
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