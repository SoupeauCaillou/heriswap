#include "LevelStateManager.h"


LevelStateManager::LevelStateManager() {

}

LevelStateManager::~LevelStateManager() {

}

void LevelStateManager::Setup() {

}

void LevelStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
}

GameState LevelStateManager::Update(float dt) {
	return Spawn;
}

void LevelStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
}
