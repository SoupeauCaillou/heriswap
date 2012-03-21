#pragma once

#include "Game.h"
#include "GameModeManager.h"

class ScoreAttackGameModeManager : public GameModeManager {
	public:
		ScoreAttackGameModeManager();
		~ScoreAttackGameModeManager();
		void Setup();
		bool Update(float dt);
		void UpdateUI(float dt, int state);
		void HideUI(bool toHide);


		int GetBonus();
		void LevelUp();
		//permet de savoir si on a change de niveau
		bool LeveledUp();
		void ScoreCalc(int nb, int type);
		void Reset();
		int GetRemain(int type);
		int GetObj();
		int GetLevel();

	private:
		int level, obj[50], remain[8], bonus;
		bool isReadyToStart, levelUp;
};
