#pragma once

#include <base/EntityManager.h>

#include "Game.h"
#include "GameStateManager.h"

class AdsStateManager : public GameStateManager {
	public:

	AdsStateManager(ScoreStorage* str) : storage(str) { };
	~AdsStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	private:
		Entity eAds;
		ScoreStorage* storage;
		int gameb4Ads;
};
