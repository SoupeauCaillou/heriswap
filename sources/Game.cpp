#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"

#include "base/TouchInputManager.h"
#include "base/MathUtil.h"

#include <sstream>

class Game::Data {
	public:
		Entity grid[8][8];
		Entity background;

		Entity CreateEntity() {
			static unsigned int e = 1;
			return e++;
		}
};

void Game::init(int windowW, int windowH) {
	datas = new Data();

	theRenderingSystem.setWindowSize(windowW, windowH);

	datas->background = datas->CreateEntity();
	theTransformationSystem.Add(datas->background);
	theRenderingSystem.Add(datas->background);
	RENDERING(datas->background)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->background)->texture = theRenderingSystem.loadTextureFile("background.png");

	float offset = 0.2;
	float scale = 0.95;
	float size = (10 - 2 * offset) / 8;
	for(int i=0; i<8; i++) {
		for(int j=0; j<8; j++) {
			datas->grid[i][j] = datas->CreateEntity();
			theTransformationSystem.Add(datas->grid[i][j]);
			TRANSFORM(datas->grid[i][j])->position = Vector2(
				-5 + (i + 0.5) * size + offset,  -5 + (j + 0.5)*size + offset);
			TRANSFORM(datas->grid[i][j])->rotation = 0;
			theRenderingSystem.Add(datas->grid[i][j]);
			int r = MathUtil::RandomInt(8);
			std::stringstream s;
			s << r << ".png";
			RENDERING(datas->grid[i][j])->texture = theRenderingSystem.loadTextureFile(s.str());
			RENDERING(datas->grid[i][j])->size = size * scale;
		}
	}
}

void Game::tick(float dt) {
	theTouchInputManager.Update(dt);

	theTransformationSystem.Update(dt);

	theADSRSystem.Update(dt);

	theButtonSystem.Update(dt);

	theRenderingSystem.Update(dt);
}
