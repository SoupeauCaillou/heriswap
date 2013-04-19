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

#include <vector>

#include "StateManager.h"
#include "modes/NormalModeManager.h"

typedef struct FeuilleOrientee {
	Entity e;
	glm::vec2 pos;
	int sens;
} FeuilleOrientee;

class LevelStateManager : public GameStateManager {
	public:

		LevelStateManager(NormalGameModeManager* mgr) : modeMgr(mgr) { };
		~LevelStateManager() { };
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

		int currentLevel;
		Entity smallLevel;
		Entity eBigLevel;

	private:
		Entity eGrid;
		std::vector<FeuilleOrientee> feuilles;
		Entity eSnowEmitter;
		Entity eSnowBranch;
		Entity eSnowGround;

		float duration;

		NormalGameModeManager* modeMgr;

		enum levelState {
            Start,
            GridHided,
            MusicStarted,
            BigScoreBeganToMove,
			BigScoreMoving
        } levelState;

};
