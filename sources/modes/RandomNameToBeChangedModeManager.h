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

#include "GameModeManager.h"


class RandomNameToBeChangedGameModeManager : public GameModeManager {
	public:
		RandomNameToBeChangedGameModeManager(HeriswapGame* game, SuccessManager* successMgr, StorageAPI* sAPI);
		~RandomNameToBeChangedGameModeManager();
		void Setup();
		void Enter();
		void GameUpdate(float dt);
		float GameProgressPercent();
		void UiUpdate(float dt);
		void Exit();
		void TogglePauseDisplay(bool paused);

		bool LevelUp() { return false; };

		void WillScore(int count, int type, std::vector<Entity>& out);

		GameMode GetMode() { return RandomNameToBeChanged; };

void squall();

		void ScoreCalc(int nb, unsigned int type);
		int saveInternalState(uint8_t** out);
        const uint8_t* restoreInternalState(const uint8_t* in, int size);
	private:

		std::vector<Render> validBranchPos;

		bool squallGo;
		float squallDuration;
		std::vector<Entity> squallLeaves;
};

