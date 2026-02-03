#include "Components.h"

Components::Input::Input(u8 type, bool hover,
	  std::function<void()> onClick,
	  std::function<void()> onHover,
	  std::function<void()> offHover)
	: type(type), hover(hover), onClick(onClick),
	onHover(onHover), offHover(offHover), on(true){

}