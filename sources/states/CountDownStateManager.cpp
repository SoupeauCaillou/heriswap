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
#include "CountDownStateManager.h"

#include <sstream>

#include <base/PlacementHelper.h>

#include "systems/TextRenderingSystem.h"

#include "Game.h"
#include "DepthLayer.h"

void CountDownStateManager::Setup() {
		counter = theTextRenderingSystem.CreateEntity();

		TRANSFORM(counter)->z = DL_MainMenuUITxt;
		TEXT_RENDERING(counter)->hide = true;
		TEXT_RENDERING(counter)->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(counter)->color = Color(0.f,0.f,0.f,1.f);
		TEXT_RENDERING(counter)->charHeight = PlacementHelper::GimpHeightToScreen(450);
		TEXT_RENDERING(counter)->isANumber = true;
}

void CountDownStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	TEXT_RENDERING(counter)->hide = false;

	timeRemaining = 3.f;
}

GameState CountDownStateManager::Update(float dt) {
	if (mode == Normal)
		return UserInput;

	timeRemaining -= dt;

	std::stringstream a;
	a << (int)timeRemaining+1;
	TEXT_RENDERING(counter)->text = a.str();

	if (timeRemaining <= 0.f) {
		return UserInput;
	}
	return CountDown;
}

void CountDownStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);

	TEXT_RENDERING(counter)->hide = true;
}
