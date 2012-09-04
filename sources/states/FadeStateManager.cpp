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
#include "FadeStateManager.h"

#include <base/TouchInputManager.h>
#include <base/EntityManager.h>

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "DepthLayer.h"

FadeGameStateManager::FadeGameStateManager(float d, FadeType fade, GameState whoAmI, GameState whoIsNext, GameStateManager* enter, GameStateManager* exitD):
	duration(d), fading(fade), iAm(whoAmI), heIs(whoIsNext), timeout(0), enterDelegate(enter), exitDelegate(exitD) {
}

void FadeGameStateManager::Setup() {
	eFading = theEntityManager.CreateEntity();
	ADD_COMPONENT(eFading, Transformation);
	ADD_COMPONENT(eFading, Rendering);
	TRANSFORM(eFading)->position = Vector2(0,0);
	TRANSFORM(eFading)->size = Vector2(10,20);
	RENDERING(eFading)->hide = true;
	RENDERING(eFading)->color = Color(0,0,0);
	TRANSFORM(eFading)->z = DL_Fading;

	ADD_COMPONENT(eFading, ADSR);
	ADSR(eFading)->idleValue = 0;
	ADSR(eFading)->attackValue = 1.0;
	ADSR(eFading)->attackTiming = duration;
	ADSR(eFading)->decayTiming = 0;
	ADSR(eFading)->sustainValue = 1.0;
	ADSR(eFading)->releaseTiming = .2;
	ADSR(eFading)->attackMode = Quadratic;
}

static void updateColor(Entity eFading, FadeType fading) {
	float value = ADSR(eFading)->value;
	if (fading == FadeIn)
		RENDERING(eFading)->color.a = 1 - value;
	else
		RENDERING(eFading)->color.a = value;
}

void FadeGameStateManager::Enter() {
	stateActiveDuration = 0;
	LOGW("Fade type: %d", fading);
	RENDERING(eFading)->hide = false;
	//update duration (can be changed)
	ADSR(eFading)->attackTiming = duration;
	ADSR(eFading)->active = true;

	accum = 0;
	updateColor(eFading, fading);

	if (enterDelegate)
		enterDelegate->Enter();
}

GameState FadeGameStateManager::Update(float dt) {
	stateActiveDuration += dt;
	updateColor(eFading, fading);
	/*if (theTouchInputManager.isTouched() && !theTouchInputManager.wasTouched()) {
		return heIs;
	}*/
	if (ADSR(eFading)->value == ADSR(eFading)->sustainValue) {
		accum += dt;
		if (accum >= timeout)
			return heIs;
		else
			return iAm;
	}
	return iAm;
}

void FadeGameStateManager::Exit() {
	LOGW("Duration : %.3f", stateActiveDuration);
	RENDERING(eFading)->hide = true;
	ADSR(eFading)->active = false;

    if (exitDelegate)
        exitDelegate->LateExit();
}
