#pragma once

#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "Game.h"
#include "GameStateManager.h"
#include "GridSystem.h"

class FallGameStateManager : public GameStateManager {
	public:

	FallGameStateManager();
	~FallGameStateManager();
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
