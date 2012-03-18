#pragma once

#include "Game.h"
#include "GameModeManager.h"

class StaticTimeGameModeManager : public GameModeManager {
	public:
		StaticTimeGameModeManager();
		~StaticTimeGameModeManager();
		void Setup();
		bool Update(float dt);

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
		int bonus;
		bool isReadyToStart;
};
