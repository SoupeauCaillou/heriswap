#include "GameStateManager.h"
#include "systems/PlayerSystem.h"

class LevelStateManager : public GameStateManager {
	public:

	LevelStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();

	private:
	
};

