#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

#include "ScoreBoardStateManager.h"
#include "GameStateManager.h"
#include "modes/GameModeManager.h"
#include "Game.h"


class ScoreStorage;

class EndMenuStateManager : public GameStateManager {
	public:
		EndMenuStateManager(ScoreStorage* str, GameModeManager* moding, GameMode modeeType);
		~EndMenuStateManager();
		void Setup();
		void Enter();

		GameState Update(float dt);
		void Exit();
	private:
		ScoreStorage* storage;
		Entity startbtn, eScore, eMsg;
		GameModeManager* mode;
		GameMode modeType;
};
