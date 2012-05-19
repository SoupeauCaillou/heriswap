#pragma once

#include "Game.h"
#include "GameStateManager.h"
#include "SuccessManager.h"

class AdsStateManager : public GameStateManager {
	public:

	AdsStateManager(ScoreStorage* str, SuccessManager* smgr) : storage(str), successMgr(smgr) { };
	~AdsStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();
	void LateExit();

	private:
		Entity eAds;
		ScoreStorage* storage;
		int gameb4Ads;
		SuccessManager* successMgr;
};
