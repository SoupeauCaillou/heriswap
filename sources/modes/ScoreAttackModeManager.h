#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"

#include "GameModeManager.h"
#include "DepthLayer.h"

class ScoreAttackGameModeManager : public GameModeManager {
	public:
		ScoreAttackGameModeManager();
		~ScoreAttackGameModeManager();
		void Setup();
		bool Update(float dt);

		void UpdateUI(float dt);
		void HideUI(bool toHide);

		void LevelUp();
		//permet de savoir si on a change de niveau
		bool LeveledUp();

		GameMode GetMode();

		void ScoreCalc(int nb, int type);
		void Reset();

	private:
		class HUDManagerData;

		int bonus;
		float distance;
};
