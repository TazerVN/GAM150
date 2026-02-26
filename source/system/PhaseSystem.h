#pragma once

namespace PhaseSystem{
	enum class GBPhase
	{
		START_PHASE, STANDBY_PHASE, DRAW_PHASE, MAIN_PHASE, RESOLUTION, ENEMY_PHASE, UNIN
	};
	extern const char* GBPhaseNames[];

	enum class PlayerPhase
	{
		PLAYER_EXPLORE, CARD_SELECT, GRID_SELECT, WAITING, UNIN
	};
	extern const char* PlayerPhaseNames[];

	class GameBoardState
	{
		private:	
		GBPhase gb_curr;
		PlayerPhase player_curr;

		public:
		GameBoardState();
		GameBoardState(GBPhase gbp, PlayerPhase pp);

		GBPhase getGBPhase() const;
		PlayerPhase getPlayerPhase() const;
		void set_GBPhase(GBPhase gbp);
		void set_PlayerPhase(PlayerPhase pp);
		void nextGBPhase();
		void nextPlayerPhase();
		void resetGPhase();
		void resetPlayerPhase();

		void debug_print();

		void update();
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


