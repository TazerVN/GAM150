#pragma once

namespace PhaseSystem{
	enum class GBPhase{
		START_PHASE, STANDBY_PHASE, DRAW_PHASE, MAIN_PHASE, RESOLUTION, ENEMY_PHASE
	};

	enum class PlayerPhase{
		PLAYER_EXPLORE, CARD_SELECT, GRID_SELECT, WAITING
	};


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
		void updateGBPhase(GBPhase gbp);
		void updatePlayerPhase(PlayerPhase pp);
		void nextGBPhase();
		void nextPlayerPhase();
	};
}


