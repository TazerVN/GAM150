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
		Entity pos{ static_cast<Entity>(-1)};
		EntityComponent::Registry* ecs{nullptr};
		std::vector<Entity> current;
		Entity text_create(const char*);

		public:
		PopUpText() = default;
		void init(EntityComponent::Registry* ecs, Entity pos);
		void update();
		void free();
		Entity get() const;
		void set(Entity id);

		friend PopUpText& operator<<(PopUpText&, const char* text);
	};
}