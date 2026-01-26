#pragma once

#ifndef RS
#define RS

#include "Mesh_factory.h"
#include <vector>


namespace RenderSystem
{

	class RenderObject
	{
		public:
		AEGfxTexture* texture = nullptr;
		RENDER_MODE mode = COLOR;
		MESH_MODE m_mode = MESH_CENTER;
		virtual void render() = 0;
		int z = 0;
	};

	class RenderManager
	{
		private:
		std::vector<RenderObject*> buffer;

		public:
		void RM_add(RenderObject* object, MESH_MODE m_mode, int z = 0);
		void RM_add(RenderObject* object, MESH_MODE m_mode, AEGfxTexture* pTex, int z = 0);
		RenderObject* RM_get(int index);
		void RM_render();

	};
};


#endif // !RS