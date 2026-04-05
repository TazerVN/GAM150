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

#include <AEEngine.h>
#include <array>

namespace PhaseSystem{
	enum class GBPhase{START_PHASE, STANDBY_PHASE, DRAW_PHASE, MAIN_PHASE, PLAYER_RESOLUTION, ENEMY_PHASE, ENEMY_RESOLUTION , UNIN};
	enum class PlayerPhase{PLAYER_EXPLORE, GRID_SELECT, AOE_GRID_SELECT, PLAYER_ANIMATION, UNIN};
	enum class EnemyPhase {ENEMY_ANIMATION, ENEMY_IDLE};

	constexpr size_t GBPhaseCount = static_cast<size_t>(GBPhase::UNIN);
	constexpr size_t PlayerPhaseCount = static_cast<size_t>(PlayerPhase::UNIN);

	extern const char* GBPhaseNames[];
	extern const char* PlayerPhaseNames[];

	class GameBoardState
	{
		private:	
		GBPhase gb_curr{GBPhase::START_PHASE};
		PlayerPhase player_curr{PlayerPhase::PLAYER_EXPLORE};
		EnemyPhase enemy_curr{ EnemyPhase::ENEMY_IDLE};

		static std::array<bool, GBPhaseCount> GBPhaseTriggered;
		static std::array<bool, GBPhaseCount> GBPhaseActive;

		GBPhase prev_GBPhase{ GBPhase::START_PHASE };
		PlayerPhase prev_PlayerPhase{ PlayerPhase::PLAYER_EXPLORE };

		public:
		GameBoardState();
		GameBoardState(GBPhase gbp, PlayerPhase pp, EnemyPhase ep);

		GBPhase getGBPhase() const;
		PlayerPhase getPlayerPhase() const;
		EnemyPhase getEnemyPhase() const;
		GBPhase getPrevGBPhase() const;
		PlayerPhase getPrevPlayerPhase() const;
		void set_GBPhase(GBPhase gbp);
		void set_PlayerPhase(PlayerPhase pp);
		void set_EnemyPhase(EnemyPhase ep);
		void nextGBPhase();
		void nextPlayerPhase();
		void prevGBPhase();
		void prevPlayerPhase();
		void resetGPhase();
		void resetPlayerPhase();

		std::array<bool, GBPhaseCount>& GBPTriggered();
		std::array<bool, GBPhaseCount>& GBPActive();

		void debug_print();
		void gbs_free();
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


