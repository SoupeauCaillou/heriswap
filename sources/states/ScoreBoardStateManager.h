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


class ScoreStorage {
	public:
	struct ScoreEntry {
		int points;
		std::string name;
	};
	
	static bool ScoreEntryComp(const ScoreEntry& e1, const ScoreEntry& e2) {
		return e1.points > e2.points;
	}
	
	virtual std::vector<ScoreEntry> loadFromStorage() = 0;

	virtual void saveToStorage(const std::vector<ScoreEntry>& entries) = 0;
};


class ScoreBoardStateManager : public GameStateManager {
	public:
		ScoreBoardStateManager(ScoreStorage* storage);
		void Setup();
		
		void Enter();
		GameState Update(float dt);
		void Exit();

	private:
		ScoreStorage* storage;
		Entity startbtn, eScore[10];
};


