#include "renderSystem.h"
#include "vector"
#include "ecs.h"
#include "Grid.h"



namespace RenderSystem
{
	RenderComponent::RenderComponent(int z)
	{
		this->z = z;
	}

	TextureComponent::TextureComponent(AEGfxTexture* pTex){
		this->texture = pTex;
	}
	void render_grid(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform, Grid::GridComponent& gc);
	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform);
	void bubbleSort(std::vector<RenderComponent*>& array, int size);

	void RenderManager::RM_render(ECSystem::ECS& ecs)
	{

		AEGfxSetBackgroundColor(0.125f, 0.125f, 0.125f);

		ECSystem::ComponentPool* render_com = ecs.ECS_get_pool(ECSystem::COMPONENT_RENDERABLE);
		ECSystem::ComponentPool* transform_com = ecs.ECS_get_pool(ECSystem::COMPONENT_TRANSFORM);
		ECSystem::ComponentPool* mesh_com = ecs.ECS_get_pool(ECSystem::COMPONENT_MESH);
		ECSystem::ComponentPool* texture_com = ecs.ECS_get_pool(ECSystem::COMPONENT_TEXTURE);
		ECSystem::ComponentPool* grid_com = ecs.ECS_get_pool(ECSystem::COMPONENT_GRID);

		for (int i = 0; i < MAX_ENTITY; i++)
		{
			if (render_com->has[i] == false || transform_com->has[i] == false) continue;
			if (mesh_com->has[i] == true)
			{
				ECSystem::TransformComponent* tc = static_cast<ECSystem::TransformComponent*>(transform_com->data[i]);
				RenderSystem::RenderComponent* rc = static_cast<RenderSystem::RenderComponent*>(render_com->data[i]);
				RenderSystem::TextureComponent* tec = static_cast<RenderSystem::TextureComponent*>(texture_com->data[i]);
				MeshComponent* mc = static_cast<MeshComponent*>(mesh_com->data[i]);

				if (grid_com->has[i] == true)
				{
					Grid::GridComponent* gc = static_cast<Grid::GridComponent*>(grid_com->data[i]);
					AEVec2 tile_size;
					AEVec2Set(&tile_size, gc->cell_width, gc->cell_height);
					if (mc->r_mode == TEXTURE)
					{
						AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
						AEGfxSetBlendMode(AE_GFX_BM_BLEND);
						AEGfxTextureSet(tec->texture, 0, 0);
						AEGfxSetTransparency(1.0f);
						AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
						AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

						

						render_grid(mc->MeshGet(), tc->pos, tile_size, &tc->transform, *gc);
					}
					else
					{
						AEGfxSetRenderMode(AE_GFX_RM_COLOR);
						render_grid(mc->MeshGet(), tc->pos, tile_size, &tc->transform, *gc);

					}
				}
				else
				{

					if (mc->r_mode == TEXTURE)
					{
						AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
						AEGfxSetBlendMode(AE_GFX_BM_BLEND);
						AEGfxTextureSet(tec->texture, 0, 0);
						AEGfxSetTransparency(1.0f);
						AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
						AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
						render_mesh(mc->MeshGet(), tc->pos, tc->size, &tc->transform);
					}
					else
					{
						AEGfxSetRenderMode(AE_GFX_RM_COLOR);
						render_mesh(mc->MeshGet(), tc->pos, tc->size, &tc->transform);
					}
				}
			}
		}
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
		AEMtx33 translate;
		AEMtx33Trans(&translate, pos.x, pos.y);
		AEMtx33 scale;
		AEMtx33Scale(&scale, size.x, size.y);
		AEMtx33Concat(transform, &translate, &scale);
		AEGfxSetTransform(transform->m);
		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
	}

	void render_grid(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform, Grid::GridComponent& gc)
	{
		for (s8 i = 0; i < gc.max_i; ++i)
		{
			for (s8 j = 0; j < gc.max_j; ++j)
			{

				AEVec2 display_pos;
				AEVec2 display_size;
				AEVec2Set(&display_pos, pos.x + (i - j) * CELL_WIDTH / 2, pos.y - (i + j) * CELL_HEIGHT / 4);
				render_mesh(mesh, display_pos, size, transform);
				/*cells_color[i][j] = 0xFFFFFF;
				cells_alpha[i][j] = 1.0f;*/
			}
		}
	}
}