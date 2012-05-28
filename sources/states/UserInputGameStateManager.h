#pragma once

#include "GameStateManager.h"
#include "SuccessManager.h"

class UserInputGameStateManager : public GameStateManager {
	public:

		UserInputGameStateManager(SuccessManager* smgr) : successMgr(smgr) {};
		void Setup();
		void Enter();
		GameState Update(float dt);
		void BackgroundUpdate(float dt);
		void Exit();
		
		void setAnimSpeed();
	private:
		// datas
		Entity eSwapper, dragged;
		bool dragStarted;
		int originI, originJ;
		int swapI, swapJ;
        std::vector<Entity> inCombinationCells;
        
        SuccessManager* successMgr;
        
        Entity currentCell, swappedCell;
        Entity rollback;
};
