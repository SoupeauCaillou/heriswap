#include "GameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

class FallGameStateManager : public GameStateManager {
	public:

	FallGameStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	private:
		// datas
		Entity eFall;
		std::vector<CellFall> falling;
		std::vector<Entity> combinationMark;
};
