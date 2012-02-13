#include "GameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

class RevertSwapGameStateManager : public GameStateManager {
	public:

		RevertSwapGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

	public:
		// input/ouput
		struct Input {

		} input;
		struct Output {

		} output;

	private:
		// datas
};
