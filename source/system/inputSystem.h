
#ifndef InputSystem_H
#define InputSystem_H

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
		std::list<std::pair<s8, Entity>> buffer;
		public:
		int cur_in = -1;
		s32 mousex, mousey;
		InputManager();
		void update(ECS::Registry& ecs, PhaseSystem::GameBoardState& gbs, Entity id);
	};



#endif // !InputSystem_H

}