/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "StateManager.h"
#include "SuccessManager.h"

class AdsStateManager : public GameStateManager {
	public:

	AdsStateManager(AdAPI* ad, StorageAPI* str) : adAPI(ad), storageAPI(str) { };
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();
	void LateExit();

	private:
		Entity eAds;
        AdAPI* adAPI;
		StorageAPI* storageAPI;
		int gameb4Ads;
        float lastAdTime;
};
