#pragma once

#include <AEEngine.h>
#include <array>

namespace PhaseSystem{
	enum class GBPhase{START_PHASE, STANDBY_PHASE, DRAW_PHASE, MAIN_PHASE, PLAYER_RESOLUTION, ENEMY_PHASE, ENEMY_RESOLUTION , UNIN};
	enum class PlayerPhase{PLAYER_EXPLORE, GRID_SELECT, AOE_GRID_SELECT, PLAYER_ANIMATION, UNIN};

	constexpr size_t GBPhaseCount = static_cast<size_t>(GBPhase::UNIN);
	constexpr size_t PlayerPhaseCount = static_cast<size_t>(PlayerPhase::UNIN);

	extern const char* GBPhaseNames[];
	extern const char* PlayerPhaseNames[];

	class GameBoardState
	{
		private:	
		GBPhase gb_curr;
		PlayerPhase player_curr;

		static std::array<bool, GBPhaseCount> GBPhaseTriggered;
		static std::array<bool, GBPhaseCount> GBPhaseActive;

		GBPhase prev_GBPhase;
		PlayerPhase prev_PlayerPhase;

		public:
		GameBoardState();
		GameBoardState(GBPhase gbp, PlayerPhase pp);

		GBPhase getGBPhase() const;
		PlayerPhase getPlayerPhase() const;
		GBPhase getPrevGBPhase() const;
		PlayerPhase getPrevPlayerPhase() const;
		void set_GBPhase(GBPhase gbp);
		void set_PlayerPhase(PlayerPhase pp);
		void nextGBPhase();
		void nextPlayerPhase();
		void prevGBPhase();
		void prevPlayerPhase();
		void resetGPhase();
		void resetPlayerPhase();

		std::array<bool, GBPhaseCount>& GBPTriggered();
		std::array<bool, GBPhaseCount>& GBPActive();

		void debug_print();
	};
	

	GBPhase& operator++(GBPhase& gbp);
	GBPhase operator++(GBPhase& gbp, int);
	GBPhase& operator--(GBPhase& gbp);
	GBPhase operator--(GBPhase& gbp, int);

	PlayerPhase& operator++(PlayerPhase& pp);
	PlayerPhase operator++(PlayerPhase& pp, int);
	PlayerPhase& operator--(PlayerPhase& pp);
	PlayerPhase operator--(PlayerPhase& pp, int);
}


