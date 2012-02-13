#include "GameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

class DeleteGameStateManager : public GameStateManager {
	public:

		DeleteGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

	public:
		// input/ouput
		struct Input {
			std::vector<Combinais> combinaisons;
		} input;
		struct Output {

		} output;
	private:
		// datas
		Entity eRemove;
		std::vector<Combinais> removing;
};
