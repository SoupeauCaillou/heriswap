#pragma once

#include "GameModeManager.h"
#include "DepthLayer.h"

class StaticTimeModeManager : public GameModeManager {
	public:
		StaticTimeModeManager();
		~StaticTimeModeManager();
		void Setup();
		bool Update(float dt);

		void UpdateUI(float dt);
		void HideUI(bool toHide);

		void LevelUp();
		//permet de savoir si on a change de niveau
		bool LeveledUp();

		void ScoreCalc(int nb, int type);
		void Reset();
		std::string finalScore();

	private:
		class HUDManagerData;
		
		int bonus;
		bool isReadyToStart;
		HUDManagerData* datas;
};
