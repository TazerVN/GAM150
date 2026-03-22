#pragma once

#include "AEEngine.h"
#include "../types.h"
#include "../ECS/ECSystem.h"
#include <iostream>
#include <string>
#include <vector>

namespace Text
{

	class PopUpText
	{
		private:

		struct Color
		{
			f32 r,g,b,a;
			Color(f32 r, f32 g, f32 b, f32 a) : r{r}, g{g}, b{b}, a{a}{}
		};

		Color c_color{1.f, 1.f, 1.f, 1.f};

		Entity pos{ static_cast<Entity>(-1)};
		EntityComponent::Registry* ecs{nullptr};
		std::vector<Entity> current;
		Entity text_create(Entity e, const char* t, f32 r, f32 g, f32 b, f32 a);
		Entity text_create_attached(Entity e, const char* t, f32 r, f32 g, f32 b, f32 a);

		public:
		PopUpText() = default;
		void init(EntityComponent::Registry* ecs, Entity pos);
		void update();
		void free();
		Entity get() const;
		void set(Entity id);

		friend PopUpText& operator<<(PopUpText&, const char* text);
		friend PopUpText& operator<<(PopUpText&, s32 value);
		void display(Entity pos, const char* text);
	};

	class NameTag
	{
	private:
		std::vector<std::pair<Entity, Entity>> nametag;
	public:
		void create_static_nametag(Entity parent, const char* a);
		void update();
		void name_tag_free();
	};
}