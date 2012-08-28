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

class UserInputGameStateManager : public GameStateManager {
	public:

		UserInputGameStateManager(SuccessManager* smgr) : successMgr(smgr) {};
		void Setup();
		void Enter();
		GameState Update(float dt);
		void BackgroundUpdate(float dt);
		void Exit();

		Entity swapAnimation;
		//for count down in time attack mode
		bool newGame;
	private:
		// datas
		Entity dragged;
		bool dragStarted;
		int originI, originJ;
		int swapI, swapJ;
        std::vector<Entity> inCombinationCells;

        SuccessManager* successMgr;

        Entity currentCell, swappedCell;
        Entity rollback;
};
