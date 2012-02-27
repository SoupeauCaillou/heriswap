#include "LevelStateManager.h"

#include "base/EntityManager.h"
#include "Game.h"

LevelStateManager::LevelStateManager() {

}

void LevelStateManager::Setup() {

}

void LevelStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

GameState LevelStateManager::Update(float dt) {
	return Spawn;
}

void LevelStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}


