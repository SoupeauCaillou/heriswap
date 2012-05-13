#pragma once

#include <base/EntityManager.h>

#include "GameStateManager.h"

class AdsStateManager : public GameStateManager {
	public:

	AdsStateManager() { };
	~AdsStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	private:
		Entity eAds;
};
