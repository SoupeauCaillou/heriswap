#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

#include "Game.h"
#include "GameStateManager.h"
#include "DepthLayer.h"

class ScoreBoardStateManager : public GameStateManager {
	public:
		ScoreBoardStateManager(ScoreStorage* storage);
		~ScoreBoardStateManager();
		void Setup();

		void Enter();
		GameState Update(float dt);
		void Exit();

	private:
		ScoreStorage* storage;
		Entity startbtn, eScore[10];
};
