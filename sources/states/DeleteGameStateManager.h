#pragma once

#include <vector>

#include "states/GameStateManager.h"
#include "GridSystem.h"
#include "SuccessManager.h"

class DeleteGameStateManager : public GameStateManager {
	public:

		DeleteGameStateManager(SuccessManager* successMgr);
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

		void setAnimSpeed();
	private:

		// datas
		Entity eRemove;
		std::vector<Combinais> removing;
        std::vector<Entity> littleLeavesDeleted;
		SuccessManager* successMgr;
};
