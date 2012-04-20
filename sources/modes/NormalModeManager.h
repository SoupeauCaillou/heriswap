#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "GameModeManager.h"
#include "PlacementHelper.h"

class NormalGameModeManager : public GameModeManager {
	public:
		NormalGameModeManager();
		~NormalGameModeManager();
		void Setup();
		float Update(float dt);

		void UpdateUI(float dt);
		void HideUI(bool toHide);

		void LevelUp();
		//permet de savoir si on a change de niveau
		bool LeveledUp();
		void deleteLeaves(int type, int nb);

		GameMode GetMode();


        void WillScore(int nb, int type, std::vector<Entity>& out);
        void ScoreCalc(int nb, int type);
		void Reset();
		
		int currentLevel() const { return level; }

	private:
		int level, remain[8];
		bool levelUp;
		float nextHerissonSpeed;
		float levelMoveDuration;
};
