#pragma once

#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "Game.h"
#include "GameStateManager.h"

enum FadeType {
	FadeIn, 
	FadeOut
};

class FadeGameStateManager : public GameStateManager {
	public:
		FadeGameStateManager(Entity eFade, FadeType fade, GameState whoAmI, GameState whoIsNext, float timeout = 0.0);
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
	private:
		Entity eFading, eThing;
		FadeType fading;
		GameState iAm, heIs;
		float timeout, accum;
};


