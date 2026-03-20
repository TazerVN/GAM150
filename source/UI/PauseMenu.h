#include "types.h"


class PauseMenu
{
	private:
	Entity dim;
	Entity continue_button;
	Entity restart_button;
	Entity leave_button;
	bool on{false};
	bool created{false};

	public:
	PauseMenu() = default;
	PauseMenu(EntityComponent::Registry& ecs, MeshFactory& mf, s32 z);
	PauseMenu& operator=(const PauseMenu& rhs);
	void update(EntityComponent::Registry& ecs);
	void free(EntityComponent::Registry& ecs);
	bool isOn();
	bool isCreated();
	void setStateOn(bool flag);
	void setStateCreate(bool flag);

};

