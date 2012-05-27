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
		
		static int levelToLeaveToDelete(int leavesMaxSize, int limit, int nb, int leavesDone);

	private:
		unsigned int leavesDone;
		bool succNoGridReset;
};

