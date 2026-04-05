//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
// 
//	EMAIL:		w.phyooo@digipen.edu, 
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"
#include "../factory/MeshFactory.h"
#include "../factory/TextureFactory.h"
#include "../util/Event.h"
#include <utility>

namespace TBS { class TurnBasedSystem; }
namespace PhaseSystem {class GameBoardState;}
namespace CardInteraction { class CardHand; }
namespace Grid { class GameBoard; };
class IntentionDisplaySystem;
class HighlightSystem;
class EnemyDirector;

namespace CombatNameSpace
{
	class CombatSystem
	{
	private:
		Entity horde{Entity(-1)};
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		Grid::GameBoard* gbptr = nullptr;
		TBS::TurnBasedSystem* tbsptr = nullptr;
		CardInteraction::CardHand* cardHandptr = nullptr;
		EventPool<highlight_tag>* evsptr = nullptr;
		HighlightSystem* hlptr = nullptr;
		IntentionDisplaySystem* intentptr = nullptr;
		EnemyDirector* edptr = nullptr;

		Entity targetted_entity = 0;
		int targetted_x{ -1 }, targetted_y{ -1 };
		bool play_card_triggered{ false };

		std::vector<AEVec2> selected_cells;
		std::vector<AEVec2> aoe_selected_cells;
		
		std::vector<std::pair<AEVec2, Entity>> graveyard;

	public:
		void init(PhaseSystem::GameBoardState& gbs, Grid::GameBoard& gb ,TBS::TurnBasedSystem& tbs,
				  CardInteraction::CardHand& cardhand,EventPool<highlight_tag>& eventSystem, 
				  HighlightSystem& hl, IntentionDisplaySystem& intnent, EnemyDirector& enemyDirector);

		int total_attack_cards_played = 0;
		int total_def_cards_played = 0;
		int total_item_cards_played = 0;
		int total_world_interaction_card_played = 0;

		MeshFactory* mfptr = nullptr;
		TextureFactory::TextureFactory* tfptr = nullptr;

		std::vector<AEVec2>& get_selected_cell();
		std::vector<AEVec2>& get_aoe_selected_cell();
		bool check_within_range(s32 const& x, s32 const& y);

		void play_attack_card(Entity caster, Entity cardID, Entity target, AEVec2 pos);
		void handle_graveyard();
		// Ranged attacks
		bool resolve_line_attack_first_hit( Entity caster, const AEVec2& startPos, const AEVec2& targetPos, int maxRange, f32 damage);
		bool resolve_line_attack_pierce(Entity caster, const AEVec2& startPos, const AEVec2& targetPos, int maxRange, f32 damage);


		void set_targetted_ent(Entity ent);
		void set_targetted_xy(int x, int y);
		void set_play_card_triggered(bool flag);

		//draw the card of the player
		PC_RETURN_TAG play_card(Entity player, Entity target, AEVec2 targetted_pos, int index);
		void remove_card(Entity user, int index);

		void update();
		void update_GBPhasetriggered();
		void update_GBPhaseUpdate();
		void end_player_resolution();
		void combatSystem_free();
		std::vector<std::pair<AEVec2, Entity>>& get_graveyard();

		//return the cardID inside the hand
		Entity draw_card(Entity player, size_t chIndex);
	};
}

enum class COMBAT_SYSTEM_RETURN_TAG
{
	INVALID = -1,
	VALID = 0
};

COMBAT_SYSTEM_RETURN_TAG Call_AttackSystem(Entity target, f32 damage,Grid::GameBoard& gb);