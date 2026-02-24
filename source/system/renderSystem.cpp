#include "renderSystem.h"
#include "vector"
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../system/transformSystem.h"
#include "GridSystem.h"

constexpr int BUFFER_CAPACITY = 100;



namespace RenderSystem
{
	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform);
	void bubbleSort(std::vector<Entity>& array, std::vector<s8>& z_buffer);

	RenderSystem::RenderSystem()
	{
		this->e_buffer.clear();
		this->z_buffer.clear();
		this->e_buffer.reserve(BUFFER_CAPACITY);
		this->z_buffer.reserve(BUFFER_CAPACITY);
	}



	void RenderSystem::RenderSystem_init(ECS::Registry& ecs)
	{

		ECS::ComponentTypeID meshID = ECS::getComponentTypeID<Components::Mesh>();
		ECS::ComponentTypeID textID = ECS::getComponentTypeID<Components::Text>();

		for (int i = 0; i < ecs.sizeEntity(); i++)
		{


			if (ecs.getBitMask()[i].test(meshID))
			{
				Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(i);

				this->e_buffer.push_back(i);
				this->z_buffer.push_back(mesh->z);
			}
			else if (ecs.getBitMask()[i].test(textID)){
				Components::Text* text = ecs.getComponent<Components::Text>(i);

				this->e_buffer.push_back(i);
				this->z_buffer.push_back(text->z);
			}
		}

		bubbleSort(this->e_buffer, this->z_buffer);
	}

	void RenderSystem::RM_render(ECS::Registry& ecs)
	{

		AEGfxSetBackgroundColor(0.125f, 0.125f, 0.125f);

		for (int i = 0; i < this->e_buffer.size(); i++)
		{

			int current_e = this->e_buffer[i];
			//int current_e = i;


			ECS::ComponentTypeID meshID = ECS::getComponentTypeID<Components::Mesh>();
			ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
			ECS::ComponentTypeID textID = ECS::getComponentTypeID<Components::Text>();
			ECS::ComponentTypeID colorID = ECS::getComponentTypeID<Components::Color>();

			if (!ecs.getBitMask()[current_e].test(transID)) continue;
			if (!ecs.getBitMask()[current_e].test(colorID)) continue;

			Components::Transform* transform = ecs.getComponent<Components::Transform>(current_e);
			Components::Color* color = ecs.getComponent<Components::Color>(current_e);




			if (ecs.getBitMask()[current_e].test(meshID))
			{
				Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(current_e);
				if (mesh->on == false) continue;



				if (mesh->r_mode == TEXTURE)
				{
					Components::Texture* texture = ecs.getComponent<Components::Texture>(current_e);
					AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
					AEGfxSetBlendMode(AE_GFX_BM_BLEND);
					AEGfxTextureSet(texture->texture, 0, 0);
					AEGfxSetTransparency(1.0f);
					AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
					AEGfxSetColorToMultiply(color->p_color.r, color->p_color.g, color->p_color.b, color->p_color.a);
					render_mesh(mesh->mesh, transform->pos_onscreen, transform->size, &transform->mtx);
				}
				else
				{
					AEGfxSetRenderMode(AE_GFX_RM_COLOR);
					AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
					AEGfxSetColorToMultiply(color->p_color.r, color->p_color.g, color->p_color.b, color->p_color.a);
					render_mesh(mesh->mesh, transform->pos_onscreen, transform->size, &transform->mtx);
				}
			}
			else if (ecs.getBitMask()[current_e].test(textID))
			{
				Components::Text* text = ecs.getComponent<Components::Text>(current_e);

				AEGfxPrint(text->fontID, text->text, transform->pos_onscreen.x, transform->pos_onscreen.y, transform->size.x, color->p_color.r, color->p_color.g, color->p_color.b, color->p_color.a);
			}


		}
	}



	void bubbleSort(std::vector<Entity>& array, std::vector<s8>& z_buffer)
	{

		for (int step = 0; step < (array.size() - 1); ++step)
		{

			int swapped = 0;

			for (int i = 0; i < (array.size() - step - 1); ++i)
			{

				if (z_buffer[i] > z_buffer[i + 1])
				{

					Entity temp = array[i];
					array[i] = array[i + 1];
					array[i + 1] = temp;

					s8 temp2 = z_buffer[i];
					z_buffer[i] = z_buffer[i + 1];
					z_buffer[i + 1] = temp2;


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
