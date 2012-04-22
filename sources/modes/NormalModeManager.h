#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "GameModeManager.h"
#include "PlacementHelper.h"

class NormalGameModeManager : public GameModeManager {
	public:
		NormalGameModeManager(Game* game);
		~NormalGameModeManager();
		
		void Setup();
		void Enter();
		 float GameUpdate(float dt);
		void UiUpdate(float dt);
		void Exit();		
		void TogglePauseDisplay(bool paused);
		
		// scoring implementation
		void WillScore(int nb, int type, std::vector<Entity>& out);
        void ScoreCalc(int nb, int type);
		GameMode GetMode();
		void LevelUp();
		bool LeveledUp();

		
		//permet de savoir si on a change de niveau
		
		

		


        
		void Reset();
		
		int currentLevel() const { return level; }

	private:
		int level, remain[8];
		bool levelUp;
		float nextHerissonSpeed;
		float levelMoveDuration;
		bool levelUpPending;
};
