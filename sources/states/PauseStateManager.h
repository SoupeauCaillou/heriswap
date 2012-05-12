#pragma once

#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "modes/GameModeManager.h"
#include "states/GameStateManager.h"
#include "states/HelpStateManager.h"

#include "Game.h"
#include "DepthLayer.h"

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
