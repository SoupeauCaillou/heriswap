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

	private:
		// datas
		Entity eRemove;
		std::vector<Combinais> removing;
};
