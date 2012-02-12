#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "GridSystem.h"

//#include "systems/HUDManager.h"
#include "base/TouchInputManager.h"
#include "base/MathUtil.h"

#include <sstream>

#define GRIDSIZE 8
class Game::Data {
	public:
		Data() {
			//hud.Setup();
		}
		
		Entity background;
		Entity swapper, fall;

		Entity CreateEntity() {
			static unsigned int e = 1;
			return e++;
		}
		// drag/drop
		bool dragStarted;
		Entity dragged;
		int originI, originJ;
		int swapI, swapJ;
		//HUDManager hud;
		std::vector<CellFall> falling;
};	

static const float offset = 0.2;
static const float scale = 0.95;
static const float size = (10 - 2 * offset) / GRIDSIZE;

Vector2 gridCoordsToPosition(int i, int j) {
	return Vector2(
		-5 + (i + 0.5) * size + offset,
		-5 + (j + 0.5)*size + offset);
}

bool isValidGridPosition(int i, int j) {
	return (i>=0 && j>=0 && i<GRIDSIZE && j<GRIDSIZE);
}

void Game::init(int windowW, int windowH) {
	datas = new Data();
	theRenderingSystem.setWindowSize(windowW, windowH);

	theGridSystem.GridSize = GRIDSIZE;
	datas->background = datas->CreateEntity();
	theTransformationSystem.Add(datas->background);
	theRenderingSystem.Add(datas->background);
	RENDERING(datas->background)->size = Vector2(10, 10.0 * windowH / windowW);
	RENDERING(datas->background)->texture = theRenderingSystem.loadTextureFile("background.png");
	
	fillTheBlank();
	
	datas->swapper = datas->CreateEntity();
	theADSRSystem.Add(datas->swapper);
	ADSR(datas->swapper)->idleValue = 0;
	ADSR(datas->swapper)->attackValue = 1.0;
	ADSR(datas->swapper)->attackTiming = 0.2;
	ADSR(datas->swapper)->decayTiming = 0;
	ADSR(datas->swapper)->sustainValue = 1.0;
	ADSR(datas->swapper)->releaseTiming = 0.2;

	datas->fall = datas->CreateEntity();
	theADSRSystem.Add(datas->fall);
	ADSR(datas->fall)->idleValue = 0;
	ADSR(datas->fall)->attackValue = 0.5;
	ADSR(datas->fall)->attackTiming = 0.2;
	ADSR(datas->fall)->decayTiming = 0.2;
	ADSR(datas->fall)->sustainValue = 1.0;
	ADSR(datas->fall)->releaseTiming = 0; 
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


void Game::fillTheBlank()
{
	for (int i=0; i<theGridSystem.GridSize; i++){
		for (int j=0; j<theGridSystem.GridSize; j++){
			if (theGridSystem.GetOnPos(i,j) == 0){
				Entity e =  datas->CreateEntity();
				theTransformationSystem.Add(e);
				TRANSFORM(e)->position = gridCoordsToPosition(i, j);
				TRANSFORM(e)->rotation = 0;
				theRenderingSystem.Add(e);
							
				int r = MathUtil::RandomInt(8);
				std::stringstream s;
				s << r << ".png";
				RENDERING(e)->texture = theRenderingSystem.loadTextureFile(s.str());
				RENDERING(e)->size = size * scale;
				theADSRSystem.Add(e);
				ADSR(e)->idleValue = size * scale;
				
				theGridSystem.Add(e);
				GRID(e)->type = r;
				GRID(e)->i = i;
				GRID(e)->j = j;
				std::cout << "nouvelle feuille en ("<<i<<","<<j<<")\n";
			}
		}	
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
		for( i=0; i<theGridSystem.GridSize && !datas->dragged; i++) {
			for(j=0; j<theGridSystem.GridSize; j++) {
				Entity e = theGridSystem.GetOnPos(i,j);

				if(e && ButtonSystem::inside(
					pos, 
					TRANSFORM(e)->worldPosition,
					RENDERING(e)->size)) {
					datas->dragged = e;
					break;
				}
			}
		}

		if (datas->dragged) {
			i--;
			datas->originI = i;
			datas->originJ = j;
			std::cout << i << ", " << j << std::endl;

			activateADSR(datas->dragged, 1.4, 1.2);

			// active neighboors
			if ((i+1)<GRIDSIZE)
				activateADSR(theGridSystem.GetOnPos(i+1,j), 1.2, 1.1);
			if ((j+1)<GRIDSIZE)
				activateADSR(theGridSystem.GetOnPos(i,j+1), 1.2, 1.1);
			if ((i-1)>=0)
				activateADSR(theGridSystem.GetOnPos(i-1,j), 1.2, 1.1);
			if ((j-1)>=0)
				activateADSR(theGridSystem.GetOnPos(i,j-1), 1.2, 1.1);
		}
	} else if (theTouchInputManager.wasTouched() && datas->dragged) {
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
			ADSR(theGridSystem.GetOnPos(datas->originI,datas->originJ))->active = false;
			if ((datas->originI+1)<GRIDSIZE)
				ADSR(theGridSystem.GetOnPos(datas->originI+1,datas->originJ))->active = false;
			if ((datas->originJ+1)<GRIDSIZE)
				ADSR(theGridSystem.GetOnPos(datas->originI,datas->originJ+1))->active = false;
			if ((datas->originI-1)>=0)
				ADSR(theGridSystem.GetOnPos(datas->originI-1,datas->originJ))->active = false;
			if ((datas->originJ-1)>=0)
				ADSR(theGridSystem.GetOnPos(datas->originI,datas->originJ-1))->active = false;
			ADSR(datas->swapper)->active = false;

			/* must swap ? */
			
			if (ADSR(datas->swapper)->value >= 0.99) {
				Entity e2 = theGridSystem.GetOnPos(datas->originI+ datas->swapI,datas->originJ+ datas->swapJ);
				GRID(e2)->i = datas->originI;
				GRID(e2)->j = datas->originJ;
				GRID(e2)->checkedH = false;
				GRID(e2)->checkedV = false;
				
				Entity e1 = datas->dragged ;
				GRID(e1)->i = datas->originI + datas->swapI;
				GRID(e1)->j = datas->originJ + datas->swapJ;
				GRID(e1)->checkedH = false;
				GRID(e1)->checkedV = false;
				

				std::cout << "swaped ("<<datas->originI<<","<<datas->originJ<<") avec ("<<datas->originI + datas->swapI<<","<<datas->originJ + datas->swapJ<<")\n";
				ADSR(datas->swapper)->activationTime = 0;
			}
		}

	} else {
		// cancel drag
		datas->dragged = 0;
		ADSR(datas->swapper)->active = false;
	}

	theADSRSystem.Update(dt);

	for(int i=0; i<GRIDSIZE; i++) {
		for(int j=0; j<GRIDSIZE; j++) {
			Entity e = theGridSystem.GetOnPos(i,j);
			if (e) {
				RENDERING(e)->size = ADSR(e)->value;
			}
		}
	}

	if (ADSR(datas->swapper)->activationTime > 0) {
		Vector2 pos1 = gridCoordsToPosition(datas->originI, datas->originJ);
		Vector2 pos2 = gridCoordsToPosition(datas->originI + datas->swapI, datas->originJ + datas->swapJ);

		Vector2 interp1 = MathUtil::Lerp(pos1, pos2, ADSR(datas->swapper)->value);
		Vector2 interp2 = MathUtil::Lerp(pos2, pos1, ADSR(datas->swapper)->value);

		TRANSFORM(theGridSystem.GetOnPos(datas->originI,datas->originJ))->position = interp1;
		TRANSFORM(theGridSystem.GetOnPos(datas->originI + datas->swapI,datas->originJ + datas->swapJ))->position = interp2;
	}

	theButtonSystem.Update(dt);
	//theGridSystem.Update(dt);
	
	std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(3);
	if (combinaisons.size()>0){
		for ( std::vector<Combinais>::reverse_iterator it = combinaisons.rbegin(); it != combinaisons.rend(); ++it ) {
			//HUDManager.ScoreCalc(it->points.size());
			for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
				std::cout << "suppression en ("<<itV->X<<","<<itV->Y<<")\n";
				Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
				if (e){
					theRenderingSystem.Delete(e);
					theTransformationSystem.Delete(e);
					theADSRSystem.Delete(e);
					theGridSystem.Delete(e);
				}
			}
		}
		datas->falling = theGridSystem.TileFall();
		ADSR(datas->fall)->active = true;
		std::cout << datas->falling.size() << " falling" << std::endl;
	}

	if (!datas->falling.empty()) {
		ADSRComponent* transition = ADSR(datas->fall);
		transition->active = true;
		for(std::vector<CellFall>::iterator it=datas->falling.begin(); it!=datas->falling.end(); ++it) {
			const CellFall& f = *it;
			Vector2 targetPos = gridCoordsToPosition(f.x, f.toY);
			Vector2 originPos = gridCoordsToPosition(f.x, f.fromY);
			TRANSFORM(f.e)->position = MathUtil::Lerp(originPos, targetPos, transition->value);
			if (transition->value == 1) {
				GRID(f.e)->j = f.toY;
			}
		}
		if (transition->value == 1) {
			datas->falling.clear();
			fillTheBlank();
		}
	} else {
		ADSR(datas->fall)->active = false;
	}
	//fillTheBlank();
	//datas->hud.Update(dt);
	theTransformationSystem.Update(dt);
	theRenderingSystem.Update(dt);
}
