#pragma once
#include <vector>
#include "../ECS/ECSystem.h"
#include "../ECS/Scene.h"
#include "../ECS/Components.h"
#include "../factory/EntityFactory.h"
#include "../util/Event.h"
#include "AEEngine.h"
namespace TBS 
{
	class TurnBasedSystem
	{
	private:
		size_t index{ 0 };
		bool is_active{ false };
		size_t cur_round{0};

		EventPool* evsptr = nullptr;

		// Actors on Borad
		std::vector<Entity> participants;
		std::vector<size_t> participant_hand{0};	//hand index not the entity of the card
		std::vector<bool> selected_card{false};
		enum class GM : uint8_t { Player = 0, Enemy = 1 };

		// Game Master Turn and Yield States
		GM current_gm{ GM::Player };
		bool gm_yielded[2]{ false, false };

		// First GM to yield gets first turn next round
		bool first_yield_set{ false };
		GM first_yielder{ GM::Player };
		size_t gm_turn_count{ 0 };

		// Helpers
		static constexpr int gm_index(GM gm) { return (gm == GM::Player) ? 0 : 1; }
		static const char* gm_name(GM gm) { return (gm == GM::Player) ? "Player" : "Enemy"; }

		bool everyone_yielded() const;
		void round_start(ECS::Registry& ecs);
	public:
		//===========Set Ups============================
		TurnBasedSystem(EventPool& evs);
		void add_participant(ECS::Registry& ecs,Entity parti);
		void remove_participant(ECS::Registry& ecs, Entity parti);
		void start(ECS::Registry& ecs);

		Entity current();
		Entity get_selected_cardhand_index();
		bool is_current_selected_card();
		void set_selected_card(bool bol);
		void next(ECS::Registry& ecs);		// advance turn after successful action OR yield
		void end();

		bool active();
		std::vector<size_t>& hand();
		size_t round();
		////TEMPORARY HELPER WILL MOVE AFTER FEW DAYS (jsut to implement and test the turn based FOR NOW)
		////TEMPORARY HELPER WILL MOVE AFTER FEW DAYS (jsut to implement and test the turn based FOR NOW)
		//static Entity FindEntityByName(ECS::Registry& ecs, const char* name)
		//{
		//	ECS::ComponentStorage<Components::Name>* names = ecs.getComponentStorage<Components::Name>();
		//	ECS::ComponentTypeID nID = ECS::getComponentTypeID<Components::Name>();

		//	for (int i = 0; i < names->getCount(); ++i)
		//	{
		//		if (!ecs.getBitMask()[i].test(nID)) continue;
		//		auto* nm = ecs.getComponent<Components::Name>(i);
		//		if (nm && nm->value == name)
		//			return (Entity)i;
		//	}

		//	return (Entity)NULL_INDEX;
		//}
		//============Yield / Turn Control=============
		void yield_current();				// current participant yields
		void force_start_if_ready(ECS::Registry& ecs);  // starts automatically when participants >=2
		void debug_print(ECS::Registry& ecs) const;

		//============Combat=======================
		Entity draw_card(ECS::Registry& ecs, Entity player, size_t chIndex);
		void play_card(ECS::Registry& ecs,Entity target, Entity cardID);
		bool Call_AttackSystem(ECS::Registry& ecs, Entity cardID, Entity target);
		//===============Update=====================
		void update(ECS::Registry& ecs,std::vector<Entity>& entities);

	};
}