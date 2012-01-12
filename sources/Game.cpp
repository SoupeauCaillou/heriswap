#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"

#include "base/TouchInputManager.h"

class Game::Data {
	public:
		Entity btn1, btn2;
		Entity btn1Scale;
};

void Game::init(int windowW, int windowH) {
	datas = new Data();

	theRenderingSystem.setWindowSize(windowW, windowH);

	datas->btn1 =  10;
	theTransformationSystem.Add(datas->btn1);
	TRANSFORM(datas->btn1)->position = Vector2(0.3, -4.5);
	TRANSFORM(datas->btn1)->rotation = 0;
	theRenderingSystem.Add(datas->btn1);
	RENDERING(datas->btn1)->texture = theRenderingSystem.loadTextureFile("ic_launcher.png");
	theButtonSystem.Add(datas->btn1);
		

	datas->btn2 =  11;
	theTransformationSystem.Add(datas->btn2);
	TRANSFORM(datas->btn2)->position = Vector2(-2.3, -1.5);
	TRANSFORM(datas->btn2)->rotation = 0;
	theRenderingSystem.Add(datas->btn2);
	RENDERING(datas->btn2)->texture = theRenderingSystem.loadTextureFile("ic_launcher.png");
	RENDERING(datas->btn2)->size = Vector2(3, 2);
	theButtonSystem.Add(datas->btn2);

	datas->btn1Scale = 12;
	theADSRSystem.Add(datas->btn1Scale);
	ADSRComponent* ac = ADSR(datas->btn1Scale);
	ac->idleValue = 1.0;
	ac->attackValue = 1.3;
	ac->attackTiming = 0.3;
	ac->decayTiming = 0.2;
	ac->sustainValue = 1.2;	
	ac->releaseTiming = 0.2;
}

void Game::tick(float dt) {
	theTouchInputManager.Update(dt);

	theTransformationSystem.Update(dt);

	theADSRSystem.Update(dt);

	theButtonSystem.Update(dt);

	ADSR(datas->btn1Scale)->active = BUTTON(datas->btn1)->mouseOver;
	RENDERING(datas->btn1)->size = Vector2(1, 1) * ADSR(datas->btn1Scale)->value;
	
	theRenderingSystem.Update(dt);
}
