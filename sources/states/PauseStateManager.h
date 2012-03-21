#pragma once

#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"

#include "GameStateManager.h"
#include "Game.h"

class PauseStateManager : public GameStateManager {
	public:

	PauseStateManager();
	~PauseStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt, GameModeManager* modeMng);
	void Exit();

	private:
		Entity eRestart;

};
