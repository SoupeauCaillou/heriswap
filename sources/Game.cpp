#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"

void Game::init(int windowW, int windowH) {
	theRenderingSystem.setWindowSize(windowW, windowH);

	Entity e =  10;
	theTransformationSystem.Add(e);
	TRANSFORM(e)->position = Vector2(0.3, -4.5);
	TRANSFORM(e)->rotation = 1.2;
	theRenderingSystem.Add(e);
	RENDERING(e)->texture = theRenderingSystem.loadTextureFile("ic_launcher.png");

	Entity e2 =  11;
	theTransformationSystem.Add(e2);
	TRANSFORM(e2)->position = Vector2(-2.3, -1.5);
	TRANSFORM(e2)->rotation = -0.3;
	theRenderingSystem.Add(e2);
	RENDERING(e2)->texture = theRenderingSystem.loadTextureFile("ic_launcher.png");
	RENDERING(e2)->size = Vector2(0.5, 2);
}

void Game::tick(float dt) {
	theTransformationSystem.Update(dt);
	
	theRenderingSystem.Update(dt);
}
