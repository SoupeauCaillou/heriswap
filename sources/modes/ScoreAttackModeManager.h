#pragma once


#include "GameModeManager.h"


class ScoreAttackGameModeManager : public GameModeManager {
	public:
		ScoreAttackGameModeManager(Game* game, SuccessAPI* successAP);
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
	private:
		SuccessAPI* successAPI;
};
