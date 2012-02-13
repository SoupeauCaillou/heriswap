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
		void Exit();

	public:
		// input/ouput
		struct Input {

		} input;
		struct Output {
			std::vector<Combinais> combinaisons;
		} output;
	private:
		// datas
		Entity eSwapper;
		bool dragStarted;
		Entity dragged;
		int originI, originJ;
		int swapI, swapJ;
};
