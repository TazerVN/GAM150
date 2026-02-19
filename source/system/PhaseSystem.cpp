#include "PhaseSystem.h"

namespace PhaseSystem
{
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
		if(this->gb_curr == GBPhase::ENEMY_PHASE){
			this->gb_curr = GBPhase::START_PHASE;
		}
		else{
			int temp = static_cast<long int>(gb_curr); 
			++temp;
			this->gb_curr = static_cast<GBPhase>(temp);
		}
	}
	void GameBoardState::nextPlayerPhase(){
		if (this->player_curr == PlayerPhase::CARD_SELECT || this->player_curr == PlayerPhase::GRID_SELECT)
		{
			this->player_curr = PlayerPhase::WAITING;
		}
		else
		{
			int temp = static_cast<long int>(player_curr);
			++temp;
			this->player_curr = static_cast<PlayerPhase>(temp);
		}
	}
}