#pragma once

#include "GameStateManager.h"

enum FadeType {
	FadeIn,
	FadeOut
};

class FadeGameStateManager : public GameStateManager {
	public:
		FadeGameStateManager(float duration, FadeType fade, GameState whoAmI, GameState whoIsNext, GameStateManager* enterDelegate, GameStateManager* exitDelegate);
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
	private:
		float duration;
		Entity eFading;
		FadeType fading;
		GameState iAm, heIs;
		float timeout, accum;
		GameStateManager* enterDelegate, *exitDelegate;
};


