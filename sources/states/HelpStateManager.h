#pragma once

#include "modes/GameModeManager.h"
#include "states/GameStateManager.h"


class HelpStateManager : public GameStateManager {
	public:

	HelpStateManager() { };
	~HelpStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	GameState oldState;
	GameMode mode;
	
	private:
		Entity eHelp;
};
