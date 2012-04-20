#pragma once

#include "base/EntityManager.h"
#include "base/MathUtil.h"

#include "Game.h"
#include "GameStateManager.h"
#include "GridSystem.h"

typedef struct FeuilleOrientee {
	Entity e;
	Vector2 pos;
	int sens;
} FeuilleOrientee;

class LevelStateManager : public GameStateManager {
	public:

	LevelStateManager();
	~LevelStateManager();
	void Setup();
	void Enter();
	GameState Update(float dt);
	void Exit();
	
	int currentLevel;
	Entity smallLevel;

	private:
	Entity eGrid;
	std::vector<FeuilleOrientee> feuilles;
	Entity eBigLevel;
	Entity eSnowEmitter;
	Entity eDesaturate;
	Entity eSnowBranch;
	Entity eSnowGround;
	Entity eMusic;
	
	float duration;
};
