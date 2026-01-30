#pragma once

void SetGameState(void (*init)(), void (*update)(), void (*exit)());
void leaveGameState();