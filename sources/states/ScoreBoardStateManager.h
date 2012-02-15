#include "GameStateManager.h"
#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

class ScoreBoardStateManager : public GameStateManager {
	public:
		ScoreBoardStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
	private:
		Entity startbtn;
};


