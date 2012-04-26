#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"

#include "GameModeManager.h"
#include "DepthLayer.h"
#include "base/PlacementHelper.h"

class ScoreAttackGameModeManager : public GameModeManager {
	public:
		ScoreAttackGameModeManager(Game* game);
		~ScoreAttackGameModeManager();
		void Setup();
		void Enter();
		void GameUpdate(float dt);
		float GameProgressPercent();
		void UiUpdate(float dt);
		void Exit();
		void TogglePauseDisplay(bool paused);

		void LevelUp();
		bool LeveledUp();

		GameMode GetMode();

		void ScoreCalc(int nb, int type);
};
