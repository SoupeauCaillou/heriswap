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

#include "modes/GameModeManager.h"
#include "StateManager.h"

class LocalizeAPI;

class HelpStateManager : public GameStateManager {
	public:

	HelpStateManager(LocalizeAPI* l) : localizeAPI(l) { };
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	GameState oldState;
	GameMode mode;

	private:
		LocalizeAPI* localizeAPI;
		Entity background, bg2;
		// howtoplay
		Entity title[2];
		// mode1
		Entity levelBig, levelSmall[3];
		// objective/mode2
		Entity difficulty[2];
		Entity leaveLeft;
		Entity bonus;

		enum State {
			HowToPlay,
			Objective
		} state;
};
