#pragma once

#include <vector>

#include "GameStateManager.h"
#include "modes/NormalModeManager.h"

typedef struct FeuilleOrientee {
	Entity e;
	Vector2 pos;
	int sens;
} FeuilleOrientee;

class LevelStateManager : public GameStateManager {
	public:

	LevelStateManager(NormalGameModeManager* mgr) : modeMgr(mgr) { };
	~LevelStateManager() { };
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
	Entity eSnowBranch;
	Entity eSnowGround;
	Entity eMusic;
	
	float duration;
	
	NormalGameModeManager* modeMgr;
	bool newLeavesGenerated;
};
