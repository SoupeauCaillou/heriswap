#pragma once

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "Game.h"
#include "GameStateManager.h"

class LogoStateManager : public GameStateManager {
	public:
		LogoStateManager(GameState following, Entity logo, Entity logobg);
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
        void LateExit();
	private:
		GameState following;
		Entity logo, animLogo, logobg;
		float duration;
};


