//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================
#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../System/TurnBasedSystem.h"
#include "../system/GridSystem.h"
#include "../system/PhaseSystem.h"
#include "../UI/cardInformation.h"


#include <iostream>
#include "AEEngine.h"


#include "AEEngine.h"
#include "vector"
#include <utility>

#define MAX_CARDS_HAND 6


//forward declaration for gameboard state
namespace PhaseSystem { class GameBoardState; }
namespace TBS { class TurnBasedSystem; }
namespace Grid { class GameBoard; }
namespace CombatNameSpace{class CombatSystem;}
class HighlightSystem;

namespace CardInteraction
{

	void card_onHover(CardInformation::CardDisplay& cd, std::pair<Entity, Entity> id, Entity card_data, s32 z);
	void card_offHover(std::pair<Entity, Entity> id, s32 z);
	void card_onClick(std::pair<Entity, Entity> id);
	void card_offClick(std::pair<Entity, Entity> id);

	class CardHand
	{
	private:
		std::vector<std::pair<Entity, Entity>> curr_hand_display{};
		std::vector<Entity> curr_card_id{};
		std::vector<bool> activate;
		Entity id{0};
		bool reset{false};
		s32 z{ 1000 };

		TBS::TurnBasedSystem* tbsptr = nullptr;
		PhaseSystem::GameBoardState* gbsptr = nullptr;
		MeshFactory* mfptr = nullptr;
		Grid::GameBoard* gbptr = nullptr;
		TextureFactory::TextureFactory* tfptr = nullptr;
		CombatNameSpace::CombatSystem* cbsptr = nullptr;
		CardInformation::CardDisplay* cdptr = nullptr;
		HighlightSystem* hlptr = nullptr;

	public:
		CardHand(f32 x, f32 y, f32 width, f32 height);
		CardHand(f32 x, f32 y, f32 width, f32 height, TBS::TurnBasedSystem& tbs);
		CardHand(f32 x, f32 y, f32 width, f32 height, 
			TBS::TurnBasedSystem& tbs, 
			Grid::GameBoard& gb, 
			PhaseSystem::GameBoardState& gbs, 
			CardInformation::CardDisplay& cd,
			CombatNameSpace::CombatSystem& cbs,
			HighlightSystem& hl);

		CardHand();

		void generateCards();
		void update_logic();
		void update_pos();
		void activate_card(Entity e);
		void remove_card(int);
		void reset_hand();
		Entity getID() const;

		void card_interaction_free();
	};

	std::pair<Entity, Entity> selectableCard_create(Entity id,
													f32 x, f32 y, f32 width, f32 height, 
													f32 rotation, s32 z, AEGfxTexture* pTex, std::function<void()> fp, 
													s32 cost, CardInformation::CardDisplay& cd, Entity card_data);
}
