//===============================================================
//	AUTHOR:		Wai Phyoo Ooo, 
//				Pham Minh Tuan
// 
//	EMAIL:		w.phyooo@digipen.edu, 
//				minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//===============================================================

#include "global.h"
#include "PhaseSystem.h"
#include <iostream>

namespace PhaseSystem
{
	const char* GBPhaseNames[] = { "START_PHASE", "STANDBY_PHASE", "DRAW_PHASE", "MAIN_PHASE", "PLAYER_RESOLUTION", "ENEMY_PHASE", "ENEMY_RESOLUTION", "UNIN" };
	const char* PlayerPhaseNames[] = { "PLAYER_EXPLORE", "GRID_SELECT", "AOE_GRID_SELECT", "PLAYER_ANIMATION" ,"UNIN"};

	std::array<bool, GBPhaseCount> GameBoardState::GBPhaseTriggered{};
	std::array<bool, GBPhaseCount> GameBoardState::GBPhaseActive{};

	GBPhase& operator++(GBPhase& gbp)
	{
		GBPhase firstPhase = static_cast<GBPhase>(0);
		GBPhase lastPhase = static_cast<GBPhase>(static_cast<int>(GBPhase::UNIN) - 1);
		gbp = (gbp == lastPhase) ? firstPhase : static_cast<GBPhase>(static_cast<int>(gbp) + 1);
		return gbp;
	}

	GBPhase operator++(GBPhase& gbp, int)
	{
		GBPhase ret{ gbp };
		++gbp;
		return ret;
	}

	GBPhase& operator--(GBPhase& gbp)
	{
		GBPhase firstPhase = static_cast<GBPhase>(0);
		GBPhase lastPhase = static_cast<GBPhase>(static_cast<int>(GBPhase::UNIN) - 1);
		gbp = (gbp == firstPhase) ? lastPhase : static_cast<GBPhase>(static_cast<int>(gbp) - 1);
		return gbp;
	}

	GBPhase operator--(GBPhase& gbp, int)
	{
		GBPhase ret{ gbp };
		--gbp;
		return ret;
	}

	PlayerPhase& operator++(PlayerPhase& pp)
	{
		PlayerPhase firstPhase = static_cast<PlayerPhase>(0);
		PlayerPhase lastPhase = static_cast<PlayerPhase>(static_cast<int>(PlayerPhase::UNIN) - 1);
		pp = (pp == lastPhase) ? firstPhase : static_cast<PlayerPhase>(static_cast<int>(pp) + 1);
		return pp;
	}

	PlayerPhase operator++(PlayerPhase& pp, int)
	{
		PlayerPhase ret{ pp };
		++pp;
		return ret;
	}

	PlayerPhase& operator--(PlayerPhase& pp)
	{
		PlayerPhase firstPhase = static_cast<PlayerPhase>(0);
		PlayerPhase lastPhase = static_cast<PlayerPhase>(static_cast<int>(PlayerPhase::UNIN) - 1);
		pp = (pp == firstPhase) ? lastPhase : static_cast<PlayerPhase>(static_cast<int>(pp) - 1);
		return pp;
	}

	PlayerPhase operator--(PlayerPhase& pp, int)
	{
		PlayerPhase ret{ pp };
		--pp;
		return ret;
	}

	GameBoardState::GameBoardState() : gb_curr{ GBPhase::START_PHASE }, player_curr{ PlayerPhase::PLAYER_EXPLORE }, enemy_curr{ EnemyPhase::ENEMY_IDLE}{}
	GameBoardState::GameBoardState(GBPhase gbp, PlayerPhase pp, EnemyPhase ep) : gb_curr{ gbp }, player_curr{ pp }, enemy_curr{ ep } {}

	GBPhase GameBoardState::getGBPhase() const
	{
		return this->gb_curr;
	}
	PlayerPhase GameBoardState::getPlayerPhase() const {
		return this->player_curr;
	}
	EnemyPhase GameBoardState::getEnemyPhase() const {
		return this->enemy_curr;
	}
	GBPhase GameBoardState::getPrevGBPhase() const
	{
		return this->prev_GBPhase;
	}
	PlayerPhase GameBoardState::getPrevPlayerPhase() const {
		return this->prev_PlayerPhase;
	}
	void GameBoardState::set_GBPhase(GBPhase gbp) {
		prev_GBPhase = gb_curr;
		this->gb_curr = gbp;
	}
	void GameBoardState::set_PlayerPhase(PlayerPhase pp) {
		prev_PlayerPhase = player_curr;
	
		this->player_curr = pp;
	}
	void GameBoardState::set_EnemyPhase(EnemyPhase ep)
	{
		this->enemy_curr = ep;
	}

	void GameBoardState::nextGBPhase(){
		prev_GBPhase = gb_curr;
		this->gb_curr++;
	}
	void GameBoardState::nextPlayerPhase(){
		prev_PlayerPhase = player_curr;
		this->player_curr++;
	}

	void GameBoardState::prevGBPhase()
	{
		prev_GBPhase = gb_curr;
		this->gb_curr--;
	}
	void GameBoardState::prevPlayerPhase()
	{
		prev_PlayerPhase = player_curr;
		this->player_curr--;
	}

	void GameBoardState::resetGPhase()
	{
		prev_GBPhase = gb_curr;
		GBPhase firstPhase = static_cast<GBPhase>(0);
		this->gb_curr = firstPhase;
	}
	void GameBoardState::resetPlayerPhase()
	{
		prev_PlayerPhase = player_curr;
		PlayerPhase firstPhase = static_cast<PlayerPhase>(0);
		this->player_curr = firstPhase;
	}

	std::array<bool, GBPhaseCount>& GameBoardState::GBPTriggered()
	{
		return this->GBPhaseTriggered;
	}
	std::array<bool, GBPhaseCount>& GameBoardState::GBPActive()
	{
		return this->GBPhaseActive;
	}

	void GameBoardState::debug_print()
	{
		std::cout << "\n==================Gameboard State==================" << '\n';
		std::cout << "GB Phase     : " << GBPhaseNames[static_cast<size_t>(gb_curr)] << '\n';
		std::cout << "Player Phase : " << PlayerPhaseNames[static_cast<size_t>(player_curr)] << '\n';
		std::cout << "===================================================" << '\n';
	}

	void GameBoardState::gbs_free()
	{
		resetPlayerPhase();
		resetGPhase();
		enemy_curr = EnemyPhase::ENEMY_IDLE;
	}
}