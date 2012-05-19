#pragma once

#include "states/HelpStateManager.h"
#include "states/GameStateManager.h"

#include "modes/GameModeManager.h"

#include "Game.h"

class PauseStateManager : public GameStateManager {
	public:

	PauseStateManager(LocalizeAPI* lAPI) : localizeAPI(lAPI) { };
	~PauseStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	GameMode mode;
	HelpStateManager* helpMgr;
	private:
		Entity eRestart, bRestart;
		Entity eAbort, bAbort;
		Entity eHelp, bHelp;
		LocalizeAPI* localizeAPI;
};
