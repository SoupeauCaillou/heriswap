#pragma once

#include <map>

enum GameMode {
	Normal,
	ScoreAttack,
	StaticTime
};


class GameModeManager {
	public:
		virtual ~GameModeManager() {}

		/* Do some initial setup if needed */
		virtual void Setup() = 0;
		/* Update gamestate, and returns true if end of the mode */
		virtual bool Update(float dt) = 0;
		/* Always called */
		virtual void BackgroundUpdate(float dt) {};
		/* Every modes need to calc score */
		virtual int GetBonus() = 0;
		virtual void LevelUp() = 0;
		//permet de savoir si on a change de niveau
		virtual bool LeveledUp() = 0;
		virtual void ScoreCalc(int nb, int type) = 0;
		virtual void Reset() = 0;
		virtual int GetRemain(int type) = 0;
		virtual int GetObj() = 0;
		virtual int GetLevel() = 0;

		float time, limit;
		int score;
};
