#include "renderSystem.h"
#include <vector>
#include <utility>
#include "../ECS/ECSystem.h"
#include "../ECS/Components.h"
#include "../system/transformSystem.h"
#include "GridSystem.h"
#include <algorithm>

constexpr int BUFFER_CAPACITY = 100;

namespace RenderSystem
{
	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, AEMtx33* transform);

	bool renderCMP(const std::pair<s8, Entity>& a, const std::pair<s8, Entity>& b)
	{
		if(a.first < b.first) return true;
		return false;
	}

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
		ECS::ComponentTypeID textureID = ECS::getComponentTypeID<Components::Texture>();
		ECS::ComponentTypeID textID = ECS::getComponentTypeID<Components::Text>();

		ECS::ComponentTypeID transID = ECS::getComponentTypeID<Components::Transform>();
		ECS::ComponentTypeID colorID = ECS::getComponentTypeID<Components::Color>();
		//create bitsets
		ECS::ComponentBitMask objMask;
		objMask.set(transID); objMask.set(meshID); objMask.set(colorID);

		ECS::ComponentBitMask textMask;
		textMask.set(transID); textMask.set(textID); textMask.set(colorID);


		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(ent);
					std::pair<s8, Entity> a{mesh->z, ent};
					buffer.push_back(a);
				}
			}
			else if ((it->first & textMask) == textMask)
			{
				for (Entity ent : it->second)
				{
					Components::Text* text = ecs.getComponent<Components::Text>(ent);
					std::pair<s8, Entity> a{ text->z, ent };
					buffer.push_back(a);
				}
			}
		}

	}

	void RenderSystem::RM_render(ECS::Registry& ecs)
	{

		AEGfxSetBackgroundColor(0.125f, 0.125f, 0.125f);


		RenderSystem_init(ecs);
		//twan's old code

		buffer.sort(renderCMP);

		while (!this->buffer.empty())
		{


			Entity current_e = this->buffer.front().second;
			this->buffer.pop_front();
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
					AEGfxSetColorToMultiply(color->d_color.r, color->d_color.g, color->d_color.b, color->d_color.a);
					render_mesh(mesh->mesh, transform->pos_onscreen, transform->size, &transform->mtx);
				}
				else
				{
					AEGfxSetRenderMode(AE_GFX_RM_COLOR);
					AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
					AEGfxSetColorToMultiply(color->d_color.r, color->d_color.g, color->d_color.b, color->d_color.a);
					render_mesh(mesh->mesh, transform->pos_onscreen, transform->size, &transform->mtx);
				}
			}
			else if (ecs.getBitMask()[current_e].test(textID))
			{
				Components::Text* text = ecs.getComponent<Components::Text>(current_e);

				AEGfxPrint(text->fontID, text->text, transform->pos_onscreen.x, transform->pos_onscreen.y, transform->size.x, color->d_color.r, color->d_color.g, color->d_color.b, color->d_color.a);
			}

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

}
