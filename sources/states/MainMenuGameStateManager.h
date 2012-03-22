#pragma once

#include <sstream>

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "Game.h"
#include "DepthLayer.h"
#include "states/GameStateManager.h"
#include "modes/GameModeManager.h"

class MainMenuGameStateManager : public GameStateManager {
	public:
		MainMenuGameStateManager();
		~MainMenuGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
	private:
		Entity start[3], score, eScore, eStart[3], bStart[3], bScore;
		float elapsedTime;

	public:
		GameMode choosenGameMode;
};
