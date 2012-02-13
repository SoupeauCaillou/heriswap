#pragma once

enum GameState {
	Spawn,
	UserInput,
	Delete,
	Fall
};

#include <map>

class GameStateManager {
	public:
		virtual ~GameStateManager() {}

		/* Do some initial setup if needed */
		virtual void Setup() = 0;
		/* Prepare to become the active manager */
		virtual void Enter() = 0;
		/* Update gamestate, and returns the new GameState */
		virtual GameState Update(float dt) = 0;
		/* Prepare to leave as the active manager */
		virtual void Exit() = 0;
};

