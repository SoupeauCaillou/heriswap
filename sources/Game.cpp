#include "Game.h"
#include "systems/TransformationSystem.h"

void Game::init() {

}

void Game::tick(float dt) {
	theTransformationSystem.Update(dt);
	// ...
}
