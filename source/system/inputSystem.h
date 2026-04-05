//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#pragma once

#include "../types.h"
#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../util/util.h"
#include <vector>
#include <utility>


//forward declarion
namespace PhaseSystem { class GameBoardState; }

namespace InputSystem
{
	class InputManager
	{
		private:
		std::list<std::pair<s32, Entity>> buffer;
		public:
		Entity cur_drag{};
		Entity cur_hover{};
		int cur_in = -1;
		s32 mousex, mousey;
		InputManager();
		void update(Entity id);
	};
}
