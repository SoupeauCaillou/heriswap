#pragma once

#include "GameModeManager.h"


class NormalGameModeManager : public GameModeManager {
	public:
		NormalGameModeManager(Game* game, SuccessManager* SuccessMgr);
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
		bool LevelUp();

		Entity getSmallLevelEntity();

		void Reset();

		int currentLevel() const { return level; }

        int saveInternalState(uint8_t** out);
        const uint8_t* restoreInternalState(const uint8_t* in, int size);

		Entity stressTrack;
	private:
		unsigned int level;
		int remain[8];
		float nextHerissonSpeed;
		float levelMoveDuration;
		bool levelUpPending;
};
