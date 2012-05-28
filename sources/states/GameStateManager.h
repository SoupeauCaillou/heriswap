#pragma once

#include "Game.h"
#include "GameState.h"

class GameModeManager;


class GameStateManager {
	public:
		virtual ~GameStateManager() {}

		/* Do some initial setup if needed */
		virtual void Setup() = 0;
		/* Prepare to become the active manager */
		virtual void Enter() = 0;
		/* Update gamestate, and returns the new GameState */
		virtual GameState Update(float dt) = 0;
		/* Always called */
		virtual void BackgroundUpdate(float dt __attribute__((unused))) {};
		/* Prepare to leave as the active manager */
		virtual void Exit() = 0;
        virtual void LateExit() {}

		GameModeManager* modeMgr;
		float stateActiveDuration;
};
