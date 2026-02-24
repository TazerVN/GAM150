#pragma once
#include "../ECS/ECSystem.h"

enum EVENT_TYPE
{
	HIGHLIGHT_EVENT = 0,
	UNHIGHLIGHT_EVENT = 1,
	ENTITY_DIE_EVENT = 2,
	PLAY_CARD_EVENT = 3,
};

struct Event
{
	bool triggered = false;
	Entity returned_value = NULL_INDEX;
};

class EventPool
{
public:
	std::vector<Event>pool;
};