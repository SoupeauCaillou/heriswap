#pragma once

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"
#include "PlacementHelper.h"
#include "states/GameStateManager.h"
#include "modes/GameModeManager.h"

class MainMenuGameStateManager : public GameStateManager {
	public:
		MainMenuGameStateManager() { };
		~MainMenuGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

		GameMode choosenGameMode;
		Entity eStart[3];
		Entity menufg, menubg;
		AnimatedActor* herisson;

	private:
		Entity bStart[3];
		Entity modeTitleToReset;

};
