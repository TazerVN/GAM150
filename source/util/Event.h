#pragma once
#include "../ECS/ECSystem.h"

enum EVENT_TYPE
{
	HIGHLIGHT_EVENT = 0,
	UNHIGHLIGHT_EVENT = 1,
	GBPHASE_EVENT = 2,
	PLAY_CARD_EVENT = 3,
};

struct Event
{
	bool triggered = false;
	int x{ -1 }; int y{-1};
	Entity returned_value = NULL_INDEX;
};

class EventPool
{
public:
	std::vector<Event>pool;
};