#pragma once

#include <sstream>

#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "Game.h"
#include "GameStateManager.h"
#include "GridSystem.h"
#include "DepthLayer.h"

class SpawnGameStateManager : public GameStateManager {
	public:
		SpawnGameStateManager();
		~SpawnGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		GameState NextState(bool marker);
		void Exit();

	private:
		// datas
		Entity eSpawn, eGrid;
		std::vector<Feuille> spawning;
};
