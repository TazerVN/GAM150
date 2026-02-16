#pragma once
#include "../ECS/ECSystem.h"

enum EVENT_TYPE
{
	HIGHLIGHT_EVENT = 0,
	ATTACK_EVENT = 1,
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