#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "GameModeManager.h"
#include "base/PlacementHelper.h"

class NormalGameModeManager : public GameModeManager {
	public:
		NormalGameModeManager(Game* game, SuccessAPI* successAP);
		~NormalGameModeManager();

		void Setup();
		void Enter();
		void GameUpdate(float dt);
		float GameProgressPercent();
		void UiUpdate(float dt);
		void Exit();
		void TogglePauseDisplay(bool paused);

		// scoring implementation
		void WillScore(int nb, int type, std::vector<Entity>& out);
        void ScoreCalc(int nb, int type);
		GameMode GetMode();
		void LevelUp();
		bool LeveledUp();

		Entity getSmallLevelEntity();

		void Reset();

		int currentLevel() const { return level; }

        int saveInternalState(uint8_t** out);
        const uint8_t* restoreInternalState(const uint8_t* in, int size);

	private:
		int level, remain[8];
		bool levelUp;
		float nextHerissonSpeed;
		float levelMoveDuration;
		bool levelUpPending;

		bool succTakeTime, succ1kpoints, succ100kpoints;
};
