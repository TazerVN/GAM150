#pragma once

#include "../types.h"
#include "../ECS/ECSystem.h"



enum EVENT_TYPE
{
	HIGHLIGHT_EVENT = 0,
	UNHIGHLIGHT_EVENT = 1
};

template <typename T>
struct EventTemplate
{
	bool triggered = false;
	T returned_value{};
};

template <typename T>
class EventPool
{
public:
	std::vector<EventTemplate<T>> template_pool;
	void free();
};