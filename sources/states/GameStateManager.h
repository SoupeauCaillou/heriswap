#pragma once

#include <map>
#include "modes/GameModeManager.h"

enum GameState {
	Spawn,
	UserInput,
	Delete,
	Fall,
	LevelChanged,
	Pause,
	MainMenu,
	ScoreBoard,
	EndMenu,
	Background,
	BlackToLogoState,
	LogoToBlackState,
	BlackToMainMenu,
	MainMenuToBlackState,
	BlackToSpawn
};


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
		virtual void BackgroundUpdate(float dt) {};
		/* Prepare to leave as the active manager */
		virtual void Exit() = 0;

		GameMode mode;
		GameModeManager* modeMng;
};
