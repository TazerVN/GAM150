#pragma once

#include "../ECS/components.h"
#include "../ECS/ECSystem.h"
#include "../System/TurnBasedSystem.h"

namespace CardInformation
{

	class CardDisplay
	{
		private:
		Entity current_card_id;
		Entity previous_card_id;
		Entity info_id;
		struct Info
		{
			s32 z{ 1000 };
			Entity name;
			Entity cost;
			std::vector<Entity> description;
			Info() = default;
			Info(EntityComponent::Registry& ecs, Entity display, Entity CardData, s32 z);
			void free();
		};
		Info text;
		bool on{ false };
		bool created{ false };
		s32 z{ 1000 };

		public:
		CardDisplay() = default;
		CardDisplay(const CardDisplay& rhs);
		CardDisplay& operator=(const CardDisplay& rhs);
		CardDisplay(EntityComponent::Registry& ecs, MeshFactory& mf, f32 x, f32 y, f32 width, f32 height, s32 z);
		void update();

		bool isOn();
		bool isCreated();

		void setStateOn(bool flag);
		void setStateCreated(bool flag);
		void setCurrentCard(Entity card_data);


		void free(EntityComponent::Registry& ecs);

	};


}