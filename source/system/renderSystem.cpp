#include "pch.h"

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
	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, f32 rotation, AEMtx33* transform);

	bool renderCMP(const std::pair<s32, Entity>& a, const std::pair<s32, Entity>& b)
	{
		if(a.first < b.first) return true;
		return false;
	}

	RenderSystem::RenderSystem(){}

	void RenderSystem::RenderSystem_init()
	{

		EntityComponent::ComponentTypeID meshID = EntityComponent::getComponentTypeID<Components::Mesh>();
		EntityComponent::ComponentTypeID textureID = EntityComponent::getComponentTypeID<Components::Texture>();
		EntityComponent::ComponentTypeID textID = EntityComponent::getComponentTypeID<Components::Text>();

		EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
		EntityComponent::ComponentTypeID colorID = EntityComponent::getComponentTypeID<Components::Color>();
		//create bitsets
		EntityComponent::ComponentBitMask objMask;
		objMask.set(transID); objMask.set(meshID); objMask.set(colorID);

		EntityComponent::ComponentBitMask textMask;
		textMask.set(transID); textMask.set(textID); textMask.set(colorID);

		for (auto it = ecs.groups().begin(); it != ecs.groups().end(); ++it)
		{
			if ((it->first & objMask) == objMask)
			{
				for (Entity ent : it->second)
				{
					Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(ent);
					std::pair<s32, Entity> a{mesh->z, ent};
					buffer.push_back(a);
				}
			}
			else if ((it->first & textMask) == textMask)
			{
				for (Entity ent : it->second)
				{
					Components::Text* text = ecs.getComponent<Components::Text>(ent);
					std::pair<s32, Entity> a{ text->z, ent };
					buffer.push_back(a);
				}
			}
		}

	}

	void RenderSystem::RM_render(Entity camera)
	{

		AEGfxSetBackgroundColor(0.f, 0.f, 0.f);


		RenderSystem_init();

		buffer.sort(renderCMP);
		Components::Transform* cam_t = ecs.getComponent<Components::Transform>(camera);

		while (!this->buffer.empty())
		{
			Entity current_e = this->buffer.front().second;

			this->buffer.pop_front();

			EntityComponent::ComponentTypeID meshID = EntityComponent::getComponentTypeID<Components::Mesh>();
			EntityComponent::ComponentTypeID transID = EntityComponent::getComponentTypeID<Components::Transform>();
			EntityComponent::ComponentTypeID textID = EntityComponent::getComponentTypeID<Components::Text>();
			EntityComponent::ComponentTypeID colorID = EntityComponent::getComponentTypeID<Components::Color>();
			EntityComponent::ComponentTypeID tagID = EntityComponent::getComponentTypeID<Components::TagClass>();

			if (!ecs.getBitMask()[current_e].test(transID)) continue;
			if (!ecs.getBitMask()[current_e].test(colorID)) continue;


			f32 camera_x = cam_t->pos.x;
			f32 camera_y = cam_t->pos.y;
			f32 zoom = cam_t->size.x;

			if (ecs.getBitMask()[current_e].test(tagID))
			{
				Components::TagClass* tag = ecs.getComponent<Components::TagClass>(current_e);
				if(tag->value == Components::Tag::CARDS || tag->value == Components::Tag::UI
				   || tag->value == Components::Tag::BACKGROUND)
				{
					camera_x = 0;
					camera_y = 0;
					zoom = 1.f;
				}
			}

			Components::Transform* transform = ecs.getComponent<Components::Transform>(current_e);
			Components::Color* color = ecs.getComponent<Components::Color>(current_e);

			AEVec2 new_pos;
			AEVec2Set(&new_pos, (transform->pos_onscreen.x - camera_x) * zoom, (transform->pos_onscreen.y - camera_y) * zoom);
			AEVec2 new_size;
			AEVec2Set(&new_size, transform->size.x * zoom, transform->size.y * zoom);


			if (ecs.getBitMask()[current_e].test(meshID))
			{
				Components::Mesh* mesh = ecs.getComponent<Components::Mesh>(current_e);
				if (mesh->on == false) continue;


				if (mesh->r_mode == TEXTURE)
				{
					Components::Texture* texture = ecs.getComponent<Components::Texture>(current_e);
					AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
					AEGfxSetBlendMode(AE_GFX_BM_BLEND);
					AEGfxTextureSet(texture->texture, texture->offset_x, texture->offset_y);
					AEGfxSetTransparency(1.0f);
					AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
					AEGfxSetColorToMultiply(color->d_color.r, color->d_color.g, color->d_color.b, color->d_color.a);
					render_mesh(mesh->mesh, new_pos, new_size, transform->rotation ,&transform->mtx);
				}
				else
				{
					AEGfxSetRenderMode(AE_GFX_RM_COLOR);
					AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
					AEGfxSetColorToMultiply(color->d_color.r, color->d_color.g, color->d_color.b, color->d_color.a);
					render_mesh(mesh->mesh, new_pos, new_size, transform->rotation, &transform->mtx);
				}
			}
			else if (ecs.getBitMask()[current_e].test(textID))
			{
				Components::Text* text = ecs.getComponent<Components::Text>(current_e);

				AEGfxPrint(text->fontID, text->text.c_str(), new_pos.x/AEGfxGetWinMaxX(), new_pos.y/AEGfxGetWinMaxY(), new_size.x, color->d_color.r, color->d_color.g, color->d_color.b, color->d_color.a);
			}

		}
	}




	void render_mesh(AEGfxVertexList* mesh, AEVec2& pos, AEVec2& size, f32 rotation , AEMtx33* transform)
	{
		AEMtx33 translate;
		AEMtx33Trans(&translate, pos.x, pos.y);
		AEMtx33 scale;
		AEMtx33Scale(&scale, size.x, size.y);
		AEMtx33 rot;
		AEMtx33RotDeg(&rot, rotation);

		AEMtx33Concat(transform, &rot, &scale);
		AEMtx33Concat(transform, &translate, transform);
		AEGfxSetTransform(transform->m);
		AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
	}

}
