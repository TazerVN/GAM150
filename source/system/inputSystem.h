
#ifndef InputSystem_H
#define InputSystem_H

#include "AEEngine.h"
#include "../ECS/ECSystem.h"
#include "../util/util.h"
#include <vector>

//forward declarion
namespace PhaseSystem { class GameBoardState; }

namespace InputSystem
{


	class InputManager
	{
		public:
		int cur_in = -1;
		s32 mousex, mousey;
		InputManager();
		void update(ECS::Registry& ecs, PhaseSystem::GameBoardState& gbs);
	};



#endif // !InputSystem_H

}