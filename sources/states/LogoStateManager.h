#pragma once

#include "GameStateManager.h"

class LogoStateManager : public GameStateManager {
	public:
		LogoStateManager(GameState following);
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
        void LateExit();
	private:
		GameState following;
		Entity logo, animLogo, logobg;
		float duration;
		
		enum LogoStep {
			LogoStep1,
			LogoStep2,
			LogoStep3,
			LogoStep4,
			LogoStep5,
		} step;
};


