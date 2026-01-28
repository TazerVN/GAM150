#include "Mesh_factory.h"
#include <vector>
#include "ecs.h"

#ifndef RS
#define RS



namespace RenderSystem
{
	class TextureComponent : public ECSystem::Component{
		AEGfxTexture* texture = nullptr;
	};


	class RenderComponent : public ECSystem::Component
	{
		public:
		RenderComponent(int z);
		int z = 0;
	};

	class RenderManager
	{
		private:
		std::vector<RenderComponent*> buffer;

		public:
		void RM_render(ECSystem::ECS& ecs);

	};
};




#endif // !RS