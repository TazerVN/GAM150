#pragma once
#include "../ECS/ECSystem.h"
#include "../global.h"
#include "../util/Event.h"

namespace TBS { class TurnBasedSystem; }
namespace PhaseSystem {class GameBoardState;}
namespace CardInteraction { class CardHand; }
namespace Grid { class GameBoard; };

namespace CombatNameSpace
{
	class CombatSystem
	{
	private:
		ECS::Registry* ecsptr = nullptr;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		Grid::GameBoard* gbptr = nullptr;
		TBS::TurnBasedSystem* tbsptr = nullptr;
		CardInteraction::CardHand* cardHandptr = nullptr;
		EventPool<highlight_tag>* evsptr = nullptr;

		Entity targetted_entity = -1;
		int targetted_x{ -1 }, targetted_y{ -1 };
		bool play_card_triggered{ false };

		std::vector<AEVec2> highlighted_cells;
		std::vector<AEVec2> aoe_highlighted_cells;
		
		std::vector<Entity> graveyard;
	public:
		void init(ECS::Registry& ecs, PhaseSystem::GameBoardState& gbs, Grid::GameBoard& gb ,TBS::TurnBasedSystem& tbs,
					 CardInteraction::CardHand& cardhand,EventPool<highlight_tag>& eventSystem);

		std::vector<AEVec2>& get_highlighted_cell();
		std::vector<AEVec2>& get_aoe_highlighted_cell();
		bool check_within_range(Entity id, s32 const& x, s32 const& y);

		void play_attack_card(ECS::Registry& ecs, Entity cardID, Entity target, AEVec2 pos);
		void handle_graveyard();


		void set_targetted_ent(Entity ent);
		void set_targetted_xy(int x, int y);

		void update_GBPhasetriggered();
		void update_GBPhaseUpdate();
		void end_player_resolution();
	};
}

enum class COMBAT_SYSTEM_RETURN_TAG
{
	INVALID = -1,
	VALID = 0
};

COMBAT_SYSTEM_RETURN_TAG Call_AttackSystem(ECS::Registry& ecs, Entity target, f32 damage);