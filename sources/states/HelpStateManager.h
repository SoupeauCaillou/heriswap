#pragma once

#include "modes/GameModeManager.h"
#include "states/GameStateManager.h"

class LocalizeAPI;

class HelpStateManager : public GameStateManager {
	public:

	HelpStateManager(LocalizeAPI* l) : localize(l) { };
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	GameState oldState;
	GameMode mode;
	
	private:
		LocalizeAPI* localize;
		Entity background;
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
