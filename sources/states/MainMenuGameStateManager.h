#pragma once

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

#include "Game.h"
#include "GameStateManager.h"

class MainMenuGameStateManager : public GameStateManager {
	public:
		MainMenuGameStateManager();
		~MainMenuGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
	private:
		Entity start, score, eScore, eStart, bStart, bScore;
		float elapsedTime;
};
