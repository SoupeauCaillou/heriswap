#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "systems/TextRenderingSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/System.h"

//#include "states/GameStateManager.h"


//FCRR : FPS Calculation Refresh Rate
#define FCRR 1.


enum GameMode {
	Normal = 1,
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

		virtual void UpdateUI(float dt) = 0;
		virtual void HideUI(bool toHide) = 0;

		virtual void ScoreCalc(int nb, int type) = 0;
		virtual std::string finalScore() = 0;

		virtual GameMode GetMode() = 0;

		virtual void LevelUp() = 0;
		//permet de savoir si on a change de niveau
		virtual bool LeveledUp() = 0;

		virtual void Reset() = 0;

		float time, limit;
		int score;
};
