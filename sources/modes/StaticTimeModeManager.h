#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"

#include "GameModeManager.h"
#include "DepthLayer.h"
#include "base/PlacementHelper.h"

class StaticTimeGameModeManager : public GameModeManager {
	public:
		StaticTimeGameModeManager(Game* game,SuccessAPI* successAPI);
		~StaticTimeGameModeManager();
		void Setup();
		void Enter();
		void Exit();
		void GameUpdate(float dt);
		float GameProgressPercent();
		void UiUpdate(float dt);
		void TogglePauseDisplay(bool paused);

		void LevelUp();
		//permet de savoir si on a change de niveau
		bool LeveledUp();

		GameMode GetMode();

		void ScoreCalc(int nb, int type);
	private:
		SuccessAPI* successAPI;
};
