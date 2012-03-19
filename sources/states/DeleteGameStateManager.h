#pragma once

#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "GameStateManager.h"
#include "GridSystem.h"
#include "CombinationMarkSystem.h"
#include "modes/GameModeManager.h"
#include "Game.h"

class DeleteGameStateManager : public GameStateManager {
	public:

		DeleteGameStateManager();
		~DeleteGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

	private:
		// datas
		Entity eRemove;
		std::vector<Combinais> removing;
};
