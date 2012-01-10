#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

void Game::init(int windowW, int windowH) {
	theRenderingSystem.setWindowSize(windowW, windowH);

	Entity e =  10;
	theTransformationSystem.Add(e);
	theRenderingSystem.Add(e);
	RENDERING(e)->texture = theRenderingSystem.loadTextureFile("ic_launcher.png");
}

void Game::tick(float dt) {
	theTransformationSystem.Update(dt);
	
	theRenderingSystem.Update(dt);
}
