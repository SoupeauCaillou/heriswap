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

#include "GameStateManager.h"
#include "GridSystem.h"

class FallGameStateManager : public GameStateManager {
	public:

	FallGameStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	Entity fallAnimation;
	private:
		// datas
		std::vector<CellFall> falling;
};
