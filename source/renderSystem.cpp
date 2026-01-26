#include "renderSystem.h"
#include "vector"

namespace RenderSystem
{
	void bubbleSort(std::vector<RenderObject*> &array, int size);
	void RenderManager::RM_render()
	{
		bubbleSort(this->buffer, int(this->buffer.size()));
		while (!this->buffer.empty())
		{
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			this->buffer.front()->render();
			this->buffer.erase(this->buffer.begin());
		}
	}

	void RenderManager::RM_add(RenderObject* object, MESH_MODE m_mode, int z)
	{
		object->mode = COLOR;
		object->m_mode = m_mode;
		object->z = z;
		this->buffer.push_back(object);
	}

	void RenderManager::RM_add(RenderObject* object, MESH_MODE m_mode, AEGfxTexture* pTex, int z)
	{
		object->mode = TEXTURE;
		object->m_mode = m_mode;
		object->texture = pTex;
		object->z = z;
		this->buffer.push_back(object);
	}

	RenderObject* RenderManager::RM_get(int index)
	{
		return this->buffer[index];
	}

	void bubbleSort(std::vector<RenderObject*> &array, int size)
	{

		for (int step = 0; step < (size - 1); ++step)
		{

			int swapped = 0;

			for (int i = 0; i < (size - step - 1); ++i)
			{

				if (array[i]->z > array[i + 1]->z)
				{

					RenderObject* temp = array[i];
					array[i] = array[i + 1];
					array[i + 1] = temp;

					swapped = 1;
				}
			}

			if (swapped == 0)
				break;
		}
	}

}