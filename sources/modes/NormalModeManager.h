#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "GameModeManager.h"

class NormalGameModeManager : public GameModeManager {
	public:
		NormalGameModeManager();
		~NormalGameModeManager();
		void Setup();
		bool Update(float dt);

		void UpdateUI(float dt);
		void HideUI(bool toHide);

		void LevelUp();
		//permet de savoir si on a change de niveau
		bool LeveledUp();
		void deleteLeaves(int type, int nb);

		GameMode GetMode();

		void ScoreCalc(int nb, int type);
		void Reset();

	private:
		class HUDManagerData;

		int level, obj[50], remain[8], bonus;
		bool levelUp;
		HUDManagerData* datas;
};
