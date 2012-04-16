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
		void LoadScore(int mode);

	private:
		ScoreStorage* storage;
		Entity startbtn, switchMode, eScore[10];
		int mode;
};
