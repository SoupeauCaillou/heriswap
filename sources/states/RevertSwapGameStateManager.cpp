#include "RevertSwapGameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

RevertSwapGameStateManager::RevertSwapGameStateManager() {

}

void RevertSwapGameStateManager::Setup() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}
	
void RevertSwapGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

GameState RevertSwapGameStateManager::Update(float dt) {
/*
	if (ADSR(datas->swapper)->activationTime <= 0) {
		return UserInput;
	} else {
		return RevertSwap;
	}
*/
	return RevertSwap;
}
	
void RevertSwapGameStateManager::Exit() {

}

