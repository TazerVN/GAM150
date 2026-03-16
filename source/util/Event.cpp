#include "pch.h"
#include "Event.h"

template<typename T>
void EventPool<T>::free()
{
	template_pool.clear();
}