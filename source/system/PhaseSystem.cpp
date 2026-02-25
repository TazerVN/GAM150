#include "PhaseSystem.h"

namespace PhaseSystem
{
	GBPhase& operator++(GBPhase& gbp)
	{
		GBPhase firstPhase = static_cast<GBPhase>(0);
		GBPhase lastPhase = static_cast<GBPhase>(static_cast<int>(GBPhase::COUNT) - 1);
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
		GBPhase lastPhase = static_cast<GBPhase>(static_cast<int>(GBPhase::COUNT) - 1);
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
		PlayerPhase lastPhase = static_cast<PlayerPhase>(static_cast<int>(PlayerPhase::COUNT) - 1);
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
		PlayerPhase lastPhase = static_cast<PlayerPhase>(static_cast<int>(PlayerPhase::COUNT) - 1);
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
	void GameBoardState::updateGBPhase(GBPhase gbp) {
		this->gb_curr = gbp;
	}
	void GameBoardState::updatePlayerPhase(PlayerPhase pp) {
		this->player_curr = pp;
	}

	void GameBoardState::nextGBPhase(){
		/*GBPhase lastPhase = static_cast<GBPhase>(static_cast<int>(GBPhase::COUNT) - 1);
		if(this->gb_curr == lastPhase){
			this->gb_curr = GBPhase::START_PHASE;
		}
		else{
			int temp = static_cast<long int>(gb_curr); 
			++temp;
			this->gb_curr = static_cast<GBPhase>(temp);
		}*/
		this->gb_curr++;
	}
	void GameBoardState::nextPlayerPhase(){
		/*if (this->player_curr == PlayerPhase::CARD_SELECT || this->player_curr == PlayerPhase::GRID_SELECT)
		{
			this->player_curr = PlayerPhase::WAITING;
		}
		else
		{
			int temp = static_cast<long int>(player_curr);
			++temp;
			this->player_curr = static_cast<PlayerPhase>(temp);
		}*/
		this->player_curr++;
	}
}