
#include "../ECS/Components.h"
#include "../ECS/ECSystem.h"
#include "../global.h"

namespace Animation{

	class AnimationSystem
	{
		//private:
		//size_t current;
		public:
		AnimationSystem();
		void init(ECS::Registry& ecs);
		void update(ECS::Registry& ecs);
	};


}