/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

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


class NormalGameModeManager : public GameModeManager {
	public:
		NormalGameModeManager(HeriswapGame* game, SuccessManager* SuccessMgr, StorageAPI* sAPI);
		~NormalGameModeManager();

		void Setup();
		void Enter();
		void GameUpdate(float dt, Scene::Enum state);
		float GameProgressPercent();
		void UiUpdate(float dt);
		void Exit();
		void TogglePauseDisplay(bool paused);

		// scoring implementation
		void WillScore(int nb, int type, std::vector<BranchLeaf>& out);
        void ScoreCalc(int nb, unsigned int type);
		GameMode GetMode();
		bool LevelUp();

		Entity getSmallLevelEntity();

		void Reset();

		void changeLevel(int newLevel);
		int currentLevel() const { return level; }

        int saveInternalState(uint8_t** out);
        const uint8_t* restoreInternalState(const uint8_t* in, int size);

		Entity stressTrack;

		std::vector<Entity> leavesInHelpCombination;

		static int levelToLeaveToDelete(int type, int nb, int maxRemain, int done, int leftOnBranchOfSameType);

	private:
		void startLevel(int lvl);

		unsigned int level;
		int remain[8];
		bool helpAvailable;
		float nextHerissonSpeed;
		float levelMoveDuration;
};

