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
#include "SuccessManager.h"

class SpawnGameStateManager : public GameStateManager {
	public:
		SpawnGameStateManager(SuccessManager* smgr);
		~SpawnGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		GameState NextState(bool marker);
		void Exit();
		void setAnimSpeed();
		
	private:
		// datas
		Entity eSpawn, eGrid;
		std::vector<Feuille> spawning;
		
		SuccessManager* successMgr;
};
