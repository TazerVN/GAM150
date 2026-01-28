#include "renderSystem.h"
#include "vector"
#include "ecs.h"

namespace RenderSystem
{
	RenderComponent::RenderComponent(int z)
	{
		this->z = z;
	}

	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform);
	void bubbleSort(std::vector<RenderComponent*>& array, int size);

	void RenderManager::RM_render(ECSystem::ECS& ecs)
	{

		AEGfxSetBackgroundColor(0.125f, 0.125f, 0.125f);

		ECSystem::ComponentPool* render_com = ecs.ECS_get_pool(ECSystem::COMPONENT_RENDERABLE);
		ECSystem::ComponentPool* transform_com = ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM);
		ECSystem::ComponentPool* mesh_com = ecs.ECS_get_pool(ECSystem::COMPONENT_MESH);

		for (int i = 0; i < MAX_ENTITY; i++)
		{
			if (render_com->has[i] == false) continue;
			if (transform_com->has[i] == false) continue;
			if (mesh_com->has[i] == true)
			{
				ECSystem::TransformComponent* tc = static_cast<ECSystem::TransformComponent*>(transform_com->data[i]);
				RenderSystem::RenderComponent* rc = static_cast<RenderSystem::RenderComponent*>(render_com->data[i]);
				MeshComponent* mc = static_cast<MeshComponent*>(mesh_com->data[i]);
				render_mesh(mc->MeshGet(), tc->pos, tc->size, &tc->transform);
			}
		}


		/*bubbleSort(this->buffer, int(this->buffer.size()));
		while (!this->buffer.empty())
		{


			this->buffer.erase(this->buffer.begin());
		}*/
	}

	void bubbleSort(std::vector<RenderComponent*>& array, int size)
	{

		for (int step = 0; step < (size - 1); ++step)
		{

			int swapped = 0;

			for (int i = 0; i < (size - step - 1); ++i)
			{

				if (array[i]->z > array[i + 1]->z)
				{

					RenderComponent* temp = array[i];
					array[i] = array[i + 1];
					array[i + 1] = temp;

					swapped = 1;
				}
			}

			if (swapped == 0)
				break;
		}
	}



	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform)
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetColorToAdd(size.x * 80/size.x * 100, 0.f, 0.f, 1.f);
		AEMtx33 translate;
		AEMtx33Trans(&translate, pos.x, pos.y);
		AEMtx33 scale;
		AEMtx33Scale(&scale, size.x, size.y);
		AEMtx33Concat(transform, &translate, &scale);

		AEGfxSetTransform(transform->m);
		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
	}

}