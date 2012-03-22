#pragma once

#include "GameModeManager.h"
#include "DepthLayer.h"

class NormalGameModeManager : public GameModeManager {
	public:
		NormalGameModeManager();
		~NormalGameModeManager();
		void Setup();
		bool Update(float dt);

		void UpdateUI(float dt);
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
		std::string finalScore();

	private:
		class HUDManagerData;
		int level, obj[50], remain[8], bonus;
		bool isReadyToStart, levelUp;
		HUDManagerData* datas;
};
