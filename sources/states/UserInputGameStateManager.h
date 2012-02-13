#include "GameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "base/TouchInputManager.h"
#include "systems/ADSRSystem.h"

class UserInputGameStateManager : public GameStateManager {
	public:

		UserInputGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void BackgroundUpdate(float dt);
		void Exit();

	private:
		// datas
		Entity eSwapper;
		bool dragStarted;
		Entity dragged;
		int originI, originJ;
		int swapI, swapJ;
};
