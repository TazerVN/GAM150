
#ifndef RS
#define RS

#include "../factory/MeshFactory.h"
#include <vector>
#include <utility>
#include "../ECS/ECSystem.h"
#include "../util/BST.h"


namespace RenderSystem
{
	
	class RenderSystem
	{
		private:
		BinaryTree<s8, Entity> bst;
		std::vector<Entity> e_buffer{0};
		std::vector<s8> z_buffer{0};

		std::list<std::pair<s8, Entity>> buffer;
		public:
		RenderSystem();
		void RenderSystem_init(ECS::Registry& ecs);
		void RM_render(ECS::Registry& ecs);

	};
};




#endif // !RS