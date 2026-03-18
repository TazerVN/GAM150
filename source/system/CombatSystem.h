#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"
#include "../util/Event.h"
#include <utility>

namespace TBS { class TurnBasedSystem; }
namespace PhaseSystem {class GameBoardState;}
namespace CardInteraction { class CardHand; }
namespace Grid { class GameBoard; };

namespace CombatNameSpace
{
	class CombatSystem
	{
	private:
		Entity horde;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		Grid::GameBoard* gbptr = nullptr;
		TBS::TurnBasedSystem* tbsptr = nullptr;
		CardInteraction::CardHand* cardHandptr = nullptr;
		EventPool<highlight_tag>* evsptr = nullptr;

		Entity targetted_entity = -1;
		int targetted_x{ -1 }, targetted_y{ -1 };
		bool play_card_triggered{ false };

		std::vector<AEVec2> highlighted_cells;
		std::vector<AEVec2> aoe_selected_cells;
		
		std::vector<std::pair<AEVec2, Entity>> graveyard;
	public:
		void init(EntityComponent::Registry& ecs, PhaseSystem::GameBoardState& gbs, Grid::GameBoard& gb ,TBS::TurnBasedSystem& tbs,
					 CardInteraction::CardHand& cardhand,EventPool<highlight_tag>& eventSystem);

		std::vector<AEVec2>& get_highlighted_cell();
		std::vector<AEVec2>& get_aoe_selected_cell();
		bool check_within_range(s32 const& x, s32 const& y);

		void play_attack_card(EntityComponent::Registry& ecs, Entity cardID, Entity target, AEVec2 pos);
		void handle_graveyard();


		void set_targetted_ent(Entity ent);
		void set_targetted_xy(int x, int y);
		void set_play_card_triggered(bool flag);


		void update();
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

COMBAT_SYSTEM_RETURN_TAG Call_AttackSystem(EntityComponent::Registry& ecs, Entity target, f32 damage);