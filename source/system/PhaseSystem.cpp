#include "PhaseSystem.h"
#include <iostream>

namespace PhaseSystem
{
	const char* GBPhaseNames[] = { "START_PHASE", "STANDBY_PHASE", "DRAW_PHASE", "MAIN_PHASE", "RESOLUTION", "ENEMY_PHASE", "UNIN" };
	const char* PlayerPhaseNames[] = { "PLAYER_EXPLORE", "CARD_SELECT", "GRID_SELECT", "WAITING", "UNIN" };

	std::array<bool, GBPhaseCount> GameBoardState::GBPhaseTriggered{};
	std::array<bool, GBPhaseCount> GameBoardState::GBPhaseActive{};
	std::array<bool, PlayerPhaseCount> GameBoardState::PlayerPhaseTriggered{};

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

	GameBoardState::GameBoardState() : gb_curr{ GBPhase::START_PHASE }, player_curr{ PlayerPhase::PLAYER_EXPLORE }{}
	GameBoardState::GameBoardState(GBPhase gbp, PlayerPhase pp) : gb_curr{gbp}, player_curr{pp}{}

	GBPhase GameBoardState::getGBPhase() const
	{
		return this->gb_curr;
	}
	PlayerPhase GameBoardState::getPlayerPhase() const {
		return this->player_curr;
	}
	void GameBoardState::set_GBPhase(GBPhase gbp) {
		this->gb_curr = gbp;
	}
	void GameBoardState::set_PlayerPhase(PlayerPhase pp) {
		this->player_curr = pp;
	}

	void GameBoardState::nextGBPhase(){
		this->gb_curr++;
	}
	void GameBoardState::nextPlayerPhase(){
		this->player_curr++;
	}

	void GameBoardState::prevGBPhase()
	{
		this->gb_curr--;
	}
	void GameBoardState::prevPlayerPhase()
	{
		this->player_curr--;
	}

	void GameBoardState::resetGPhase()
	{
		GBPhase firstPhase = static_cast<GBPhase>(0);
		this->gb_curr = firstPhase;
	}
	void GameBoardState::resetPlayerPhase()
	{
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
	std::array<bool, PlayerPhaseCount>& GameBoardState::PlayerPTriggered()
	{
		return this->PlayerPhaseTriggered;
	}

	void GameBoardState::debug_print()
	{
		std::cout << "\n==================Gameboard State==================" << std::endl;
		std::cout << "GB Phase     : " << GBPhaseNames[static_cast<size_t>(gb_curr)] << std::endl;
		std::cout << "Player Phase : " << PlayerPhaseNames[static_cast<size_t>(player_curr)] << std::endl;
		std::cout << "===================================================" << std::endl;
	}


}