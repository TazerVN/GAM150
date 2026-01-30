#include "Mesh_factory.h"
#include <vector>
#include "../ECS/ECSystem.h"

#ifndef RS
#define RS



namespace RenderSystem
{
	
	class RenderSystem
	{
		private:
		//std::vector<RenderComponent*> buffer;

		public:
		void RM_render(ECS::Registry& ecs);

	};
};




#endif // !RS