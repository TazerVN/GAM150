//=========================================
//	AUTHOR:		PHAM MINH TUAN
// 
//	EMAIL:		minhtuan.pham@digipen.edu
// 
//	DATE:		5-4-2026
//=========================================
#pragma once

#include "AEEngine.h"
#include "../types.h"
#include "../ECS/ECSystem.h"
#include "../UI/UIObject.h"
#include <iostream>
#include <string>
#include <vector>

namespace Text
{

	class PopUpText
	{
		private:
		Components::RGBA c_color{1.f, 1.f, 1.f, 1.f};

		Entity pos{ static_cast<Entity>(-1)};
		EntityComponent::Registry* ecs{nullptr};
		std::vector<Entity> current;
		Entity text_create(Entity e, const char* t, f32 r, f32 g, f32 b, f32 a);
		Entity text_create_attached(Entity e, const char* t, f32 r, f32 g, f32 b, f32 a);
		s32 z{1000};
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

	class InstructionText
	{
		public:
		Components::RGBA rgba{1.f, 1.f, 1.f, 1.f};
		s32 z{1100};
		f32 y{400.f};
		f32 size{0.7f};

		std::vector<UIO::TextShadow*> text;

		void set(f32 y, f32 size);
		void set(Components::RGBA color);
		friend InstructionText& operator<<(InstructionText&, const char* text);
		InstructionText& operator+=(const char* text);
		void free();
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