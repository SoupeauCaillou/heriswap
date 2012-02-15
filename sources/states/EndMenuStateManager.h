#include "GameStateManager.h"
#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/PlayerSystem.h"

class EndMenuStateManager : public GameStateManager {
	public:
		EndMenuStateManager();
		void Setup();
		void Enter();
		GameState Update(float dt);
		void Exit();
	private:
		Entity startbtn, eScore;
};


