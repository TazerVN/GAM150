#include "renderSystem.h"
#include "vector"
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../system/transformSystem.h"
#include "Grid.h"



namespace RenderSystem
{
	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform);
	//void bubbleSort(std::vector<RenderComponent*>& array, int size);

	void RenderSystem::RM_render(ECS::Registry& ecs)
	{

		AEGfxSetBackgroundColor(0.125f, 0.125f, 0.125f);

		ECS::ComponentStorage<Components::Transform>* transform_com = ecs.getComponentStorage<Components::Transform>();
		ECS::ComponentStorage<Components::Mesh>* mesh_com = ecs.getComponentStorage<Components::Mesh>();
		ECS::ComponentStorage<Components::Texture>* texture_com = ecs.getComponentStorage<Components::Texture>();



		for (int i = 0; i < mesh_com->getCount(); i++)
		{

			ECS::ComponentTypeID meshID = ECS::getComponentTypeID<Components::Mesh>();
			ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();

			if (!ecs.getBitMask()[i].test(meshID)) continue;
			if (!ecs.getBitMask()[i].test(transID)) continue;


			Components::Transform* transform = ecs.getComponent<Components::Transform>(i);
			Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(i);
			Components::Texture* texture = ecs.getComponent<Components::Texture>(i);


			if (mesh->r_mode == TEXTURE)
			{
				AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
				AEGfxSetBlendMode(AE_GFX_BM_BLEND);
				AEGfxTextureSet(texture->texture, 0, 0);
				AEGfxSetTransparency(1.0f);
				AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
				AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
				render_mesh(mesh->mesh, transform->pos_onscreen, transform->size, &transform->mtx);
			}
			else
			{
				AEGfxSetRenderMode(AE_GFX_RM_COLOR);
				render_mesh(mesh->mesh, transform->pos_onscreen, transform->size, &transform->mtx);
			}

		}
	}



/*void bubbleSort(std::vector<RenderComponent*>& array, int size)
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
}*/




	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform)
	{
		AEMtx33 translate;
		AEMtx33Trans(&translate, pos.x, pos.y);
		AEMtx33 scale;
		AEMtx33Scale(&scale, size.x, size.y);
		AEMtx33Concat(transform, &translate, &scale);
		AEGfxSetTransform(transform->m);
		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
	}

	//void render_grid(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform, Grid::GridComponent& gc)
	//{
	//	for (s8 i = 0; i < gc.max_i; ++i)
	//	{
	//		for (s8 j = 0; j < gc.max_j; ++j)
	//		{

	//			AEVec2 display_pos;
	//			AEVec2 display_size;
	//			AEVec2Set(&display_pos, pos.x + (i - j) * CELL_WIDTH / 2, pos.y - (i + j) * CELL_HEIGHT / 4);
	//			render_mesh(mesh, display_pos, size, transform);
	//			/*cells_color[i][j] = 0xFFFFFF;
	//			cells_alpha[i][j] = 1.0f;*/
	//		}
	//	}
	//}
}
