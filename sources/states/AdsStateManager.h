#pragma once

#include "Game.h"
#include "GameStateManager.h"
#include "SuccessManager.h"

class AdsStateManager : public GameStateManager {
	public:

	AdsStateManager(AdAPI* ad, StorageAPI* str, SuccessManager* smgr) : adAPI(ad), storage(str), successMgr(smgr) { };
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();
	void LateExit();

	private:
		Entity eAds;
        AdAPI* adAPI;
		StorageAPI* storage;
		int gameb4Ads;
		SuccessManager* successMgr;
};
