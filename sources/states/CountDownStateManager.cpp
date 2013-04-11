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
#include "systems/TransformationSystem.h"

#include "DepthLayer.h"

void CountDownStateManager::Setup() {
	counter = theEntityManager.CreateEntity();
	ADD_COMPONENT(counter, Transformation);
	ADD_COMPONENT(counter, TextRendering);
	TRANSFORM(counter)->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(650));
	TEXT_RENDERING(counter)->color = Color(3.0/255.0, 99.0/255, 71.0/255);
	TEXT_RENDERING(counter)->fontName = "gdtypo";
	TEXT_RENDERING(counter)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(counter)->charHeight = PlacementHelper::GimpHeightToScreen(300);
	TRANSFORM(counter)->z = DL_MainMenuFg;

	vorhang = theEntityManager.CreateEntity();
	ADD_COMPONENT(vorhang, Rendering);
	ADD_COMPONENT(vorhang, Transformation);
	TRANSFORM(vorhang)->z = DL_MainMenuBg;
	TRANSFORM(vorhang)->size = glm::vec2( (float)PlacementHelper::GimpWidth, 
										  (float)PlacementHelper::GimpHeight);
	RENDERING(vorhang)->color = Color(0.f, 0.f, 0.f, 0.2f);
}

void CountDownStateManager::Enter() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");

	if (mode != Normal) {
		TEXT_RENDERING(counter)->show = true;
		RENDERING(vorhang)->show = true;
	}
	timeRemaining = 3.f;
}

GameState CountDownStateManager::Update(float dt) {
	if (mode == Normal) {
		return UserInput;
	}

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
	LOGI("'" << __PRETTY_FUNCTION__ << "'");

	TEXT_RENDERING(counter)->show = false;
	RENDERING(vorhang)->show = false;
}
