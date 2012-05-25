#pragma once

#include "GameStateManager.h"
#include "SuccessManager.h"
#include "GridSystem.h"

class SpawnGameStateManager : public GameStateManager {
	public:
		SpawnGameStateManager(SuccessManager* smgr);
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
