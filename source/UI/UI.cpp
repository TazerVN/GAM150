#include "../ECS/gameObject.h"
#include "../system/transformSystem.h"
#include "../ECS/ECSystem.h"
#include "../UI/UI.h"

namespace UI
{
	Entity hp_bar(ECS::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, f32 rotation, s8 z)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ { 0.5f, 1.0f, 0.5f ,1.0f }, { 0.5f, 1.0f, 0.5f ,1.0f } };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

}