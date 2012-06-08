#pragma once

#include "states/GameStateManager.h"
#include "modes/GameModeManager.h"
#include "AnimedEntity.h"
#include "Game.h"

class MainMenuGameStateManager : public GameStateManager {
	public:
		MainMenuGameStateManager(LocalizeAPI* lAPI, SuccessAPI* sAPI) : localizeAPI(lAPI), successAPI(sAPI) { };
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

		GameMode choosenGameMode;
		Entity eStart[2];
		Entity menufg, menubg;
		AnimatedActor* herisson;

	private:
		Entity bStart[2];
		Entity quitButton[2];
		Entity modeTitleToReset;
		LocalizeAPI* localizeAPI;
        SuccessAPI* successAPI;
};
