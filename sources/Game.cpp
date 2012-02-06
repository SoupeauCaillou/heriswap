#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/CombinaisonSystem.h"
#include "systems/GridSystem.h"

#include "base/TouchInputManager.h"
#include "base/MathUtil.h"

#include <sstream>

class Game::Data {
	public:
		Entity grid[8][8];
		Entity background;
		Entity swapper;

		Entity CreateEntity() {
			static unsigned int e = 1;
			return e++;
		}

		// drag/drop
		bool dragStarted;
		Entity dragged;
		int originI, originJ;
		int swapI, swapJ;
};

static const float offset = 0.2;
static const float scale = 0.95;
static const float size = (10 - 2 * offset) / 8;

Vector2 gridCoordsToPosition(int i, int j) {
	return Vector2(
		-5 + (i + 0.5) * size + offset,
		-5 + (j + 0.5)*size + offset);
}

bool isValidGridPosition(int i, int j) {
	return (i>=0 && j>=0 && i<8 && j<8);
}

void Game::init(int windowW, int windowH) {
	datas = new Data();

	theRenderingSystem.setWindowSize(windowW, windowH);

	datas->background = datas->CreateEntity();
	theTransformationSystem.Add(datas->background);
	theRenderingSystem.Add(datas->background);
	RENDERING(datas->background)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->background)->texture = theRenderingSystem.loadTextureFile("background.png");

	
	for(int i=0; i<8; i++) {
		for(int j=0; j<8; j++) {
			datas->grid[i][j] = datas->CreateEntity();
			theTransformationSystem.Add(datas->grid[i][j]);
			TRANSFORM(datas->grid[i][j])->position = gridCoordsToPosition(i, j);
			TRANSFORM(datas->grid[i][j])->rotation = 0;
			theRenderingSystem.Add(datas->grid[i][j]);
			

			
			theCombinaisonSystem.Add(datas->grid[i][j]);
			int r = MathUtil::RandomInt(8);
			std::stringstream s;
			s << r << ".png";
			RENDERING(datas->grid[i][j])->texture = theRenderingSystem.loadTextureFile(s.str());
			RENDERING(datas->grid[i][j])->size = size * scale;
			theADSRSystem.Add(datas->grid[i][j]);
			ADSR(datas->grid[i][j])->idleValue = size * scale;
			
			theGridSystem.Add(datas->grid[i][j]);
			GRID(datas->grid[i][j])->type = r;
			GRID(datas->grid[i][j])->row = i;
			GRID(datas->grid[i][j])->column = j;
		}
	}

	theADSRSystem.Add(datas->swapper);
	ADSR(datas->swapper)->idleValue = 0;
	ADSR(datas->swapper)->attackValue = 1.0;
	ADSR(datas->swapper)->attackTiming = 0.2;
	ADSR(datas->swapper)->decayTiming = 0;
	ADSR(datas->swapper)->sustainValue = 1.0;
	ADSR(datas->swapper)->releaseTiming = 0.2;
}

void activateADSR(Entity e, float a, float s) {
	float size = RENDERING(e)->size.X;
	ADSRComponent* ac = ADSR(e);
	ac->idleValue = size;
	ac->attackValue = size * a;
	ac->attackTiming = 0.3;
	ac->decayTiming = 0.2;
	ac->sustainValue = size * s;	
	ac->releaseTiming = 0.2;
	ac->active = true;
}

void diffToGridCoords(const Vector2& c, int* i, int* j) {
	*i = *j = 0;
	if (MathUtil::Abs(c.X) > MathUtil::Abs(c.Y)) {
		*i = (c.X < 0) ? -1 : 1;
	} else {
		*j = (c.Y < 0) ? -1 : 1;
	}
}

void Game::tick(float dt) {
	theTouchInputManager.Update(dt);

	/* drag/drop of cell */
	if (!theTouchInputManager.wasTouched() && 
		theTouchInputManager.isTouched()) {
		// start drag
		// find nearest cell
		const Vector2& pos = theTouchInputManager.getTouchLastPosition();
		datas->dragged = 0;
		int i, j;
		for( i=0; i<8 && !datas->dragged; i++) {
			for(j=0; j<8; j++) {
				if(ButtonSystem::inside(
					pos, 
					TRANSFORM(datas->grid[i][j])->worldPosition,
					RENDERING(datas->grid[i][j])->size)) {
					datas->dragged = datas->grid[i][j];
					break;
				}
			}
		}

		if (datas->dragged) {
			i--;
			datas->dragStarted = true;
			datas->originI = i;
			datas->originJ = j;
			std::cout << i << ", " << j << std::endl;

			activateADSR(datas->dragged, 1.4, 1.2);

			// active neighboors
			if ((i+1)<8)
				activateADSR(datas->grid[i+1][j], 1.2, 1.1);
			if ((j+1)<8)
				activateADSR(datas->grid[i][j+1], 1.2, 1.1);
			if ((i-1)>=0)
				activateADSR(datas->grid[i-1][j], 1.2, 1.1);
			if ((j-1)>=0)
				activateADSR(datas->grid[i][j-1], 1.2, 1.1);
		}
	} else if (theTouchInputManager.wasTouched() && datas->dragStarted) {
		if (theTouchInputManager.isTouched()) {
			// continue drag
			Vector2 diff = theTouchInputManager.getTouchLastPosition() 
				- gridCoordsToPosition(datas->originI, datas->originJ);

			if (diff.Length() > 1) {
				int i,j;
				diffToGridCoords(diff, &i, &j);

				if (isValidGridPosition(datas->originI + i, datas->originJ + j)) {
					if ((datas->swapI == i && datas->swapJ == j) ||
						(datas->swapI == 0 && datas->swapJ == 0)) {
						ADSR(datas->swapper)->active = true;
						datas->swapI = i;
						datas->swapJ = j;
					} else {
						if (ADSR(datas->swapper)->activationTime > 0) {
							ADSR(datas->swapper)->active = false;
						} else {
							ADSR(datas->swapper)->active = true;
							datas->swapI = i;
							datas->swapJ = j;
						}
					}
				} else {
					ADSR(datas->swapper)->active = false;
				}
			} else {
				ADSR(datas->swapper)->active = false;
			}

		} else {
			std::cout << "release " << std::endl;
			// release drag
			ADSR(datas->grid[datas->originI][datas->originJ])->active = false;
			if ((datas->originI+1)<8)
				ADSR(datas->grid[datas->originI+1][datas->originJ])->active = false;
			if ((datas->originJ+1)<8)
				ADSR(datas->grid[datas->originI][datas->originJ+1])->active = false;
			if ((datas->originI-1)>=0)
				ADSR(datas->grid[datas->originI-1][datas->originJ])->active = false;
			if ((datas->originJ-1)>=0)
				ADSR(datas->grid[datas->originI][datas->originJ-1])->active = false;
			ADSR(datas->swapper)->active = false;

			/* must swap ? */
			if (ADSR(datas->swapper)->value >= 0.99) {
				datas->grid[datas->originI][datas->originJ] = 
					datas->grid[datas->originI + datas->swapI][datas->originJ + datas->swapJ];
				datas->grid[datas->originI + datas->swapI][datas->originJ + datas->swapJ] = datas->dragged;
				ADSR(datas->swapper)->activationTime = 0;
			}
		}

	} else {
		// cancel drag
		ADSR(datas->swapper)->active = false;
	}
	theTransformationSystem.Update(dt);

	theADSRSystem.Update(dt);

	for(int i=0; i<8; i++) {
		for(int j=0; j<8; j++) {
			RENDERING(datas->grid[i][j])->size = ADSR(datas->grid[i][j])->value;
		}
	}

	if (ADSR(datas->swapper)->activationTime > 0) {
		Vector2 pos1 = gridCoordsToPosition(datas->originI, datas->originJ);
		Vector2 pos2 = gridCoordsToPosition(datas->originI + datas->swapI, datas->originJ + datas->swapJ);

		Vector2 interp1 = MathUtil::Lerp(pos1, pos2, ADSR(datas->swapper)->value);
		Vector2 interp2 = MathUtil::Lerp(pos2, pos1, ADSR(datas->swapper)->value);

		TRANSFORM(datas->grid[datas->originI][datas->originJ])->position = interp1;
		TRANSFORM(datas->grid[datas->originI + datas->swapI][datas->originJ + datas->swapJ])->position = interp2;
	}

	theButtonSystem.Update(dt);
	theGridSystem.Update(dt);
	
	theRenderingSystem.Update(dt);
}
