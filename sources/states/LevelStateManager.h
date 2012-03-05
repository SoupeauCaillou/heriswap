#pragma once

#include "base/EntityManager.h"

#include "Game.h"
#include "GameStateManager.h"
#include "PlayerSystem.h"

class LevelStateManager : public GameStateManager {
	public:

	LevelStateManager();
	~LevelStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	private:

};
