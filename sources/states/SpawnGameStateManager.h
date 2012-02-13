#include "GameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

class SpawnGameStateManager : public GameStateManager {
	public:

		SpawnGameStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();

	private:
		// datas
		Entity eSpawn;
		std::vector<Feuille> spawning;
};
