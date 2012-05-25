#pragma once

#include <vector>

#include "GameStateManager.h"
#include "GridSystem.h"

class FallGameStateManager : public GameStateManager {
	public:

	FallGameStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	void setAnimSpeed();

	private:
		// datas
		Entity eFall;
		std::vector<CellFall> falling;
};
