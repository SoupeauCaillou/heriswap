#pragma once

#include "GameModeManager.h"


class TilesAttackGameModeManager : public GameModeManager {
	public:
		TilesAttackGameModeManager(Game* game, SuccessManager* successMgr);
		~TilesAttackGameModeManager();
		void Setup();
		void Enter();
		void GameUpdate(float dt);
		float GameProgressPercent();
		void UiUpdate(float dt);
		void Exit();
		void TogglePauseDisplay(bool paused);

		bool LevelUp();

		void WillScore(int count, int type, std::vector<Entity>& out);

		GameMode GetMode();

		void ScoreCalc(int nb, int type);
	private:
		unsigned int leavesDone;
		bool succNoGridReset;
};

