#include "Game.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/TextRenderingSystem.h"
#include "GridSystem.h"

#include "systems/HUDManager.h"
#include "base/TouchInputManager.h"
#include "base/MathUtil.h"

#include <sstream>

#define GRIDSIZE 8

enum State {
	Spawn,
	UserInput,
	RevertSwap,
	Delete,
	Fall
};
class Game::Data {
	public:
		Data() {
			hud.Setup();
			state = Spawn;
		}
		
		State state;
		Entity background;
		Entity swapper, fall, remove, spawn;
		Entity CreateEntity() {
			static unsigned int e = 1;
			return e++;
		}
		// drag/drop
		bool dragStarted;
		Entity dragged;
		int originI, originJ;
		int swapI, swapJ;
		HUDManager hud;
		std::vector<CellFall> falling;
		std::vector<Combinais> removing;
		std::vector<Feuille> spawning;
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
	
	datas->remove = datas->CreateEntity();
	theADSRSystem.Add(datas->remove);
	ADSR(datas->remove)->idleValue = 0;
	ADSR(datas->remove)->attackValue = 0.5;
	ADSR(datas->remove)->attackTiming = 0.2;
	ADSR(datas->remove)->decayTiming = 0.2;
	ADSR(datas->remove)->sustainValue = 1.0;
	ADSR(datas->remove)->releaseTiming = 0; 
	
	datas->spawn = datas->CreateEntity();
	theADSRSystem.Add(datas->spawn);
	ADSR(datas->spawn)->idleValue = 0;
	ADSR(datas->spawn)->attackValue = 0.5;
	ADSR(datas->spawn)->attackTiming = 0.2;
	ADSR(datas->spawn)->decayTiming = 0.2;
	ADSR(datas->spawn)->sustainValue = 1.0;
	ADSR(datas->spawn)->releaseTiming = 0; 
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
				int r;
				int pb;
				/*ne pas generer de combinaison*/
				do {	
					pb = 0;
					r = MathUtil::RandomInt(8);
					Entity l[5],c[5];
					for (int k=0;k<5;k++){
						 l[k] = theGridSystem.GetOnPos(i+2-k,j);
						 c[k] = theGridSystem.GetOnPos(i,j+2-k);
					 }
					if (l[0] && l[1] && GRID(l[0])->type == r && r == GRID(l[1])->type)
						pb++;
					if (l[1] && l[3] && GRID(l[1])->type == r && r == GRID(l[3])->type)
						pb++;
					if (l[4] && l[3] && GRID(l[3])->type == r && r == GRID(l[4])->type)
						pb++;
					if (c[0] && c[1] && GRID(c[0])->type == r && r == GRID(c[1])->type)
						pb++;
					if (c[1] && c[3] && GRID(c[1])->type == r && r == GRID(c[3])->type)
						pb++;
					if (c[4] && c[3] && GRID(c[3])->type == r && r == GRID(c[4])->type)
						pb++;
					
				} while (pb!=0 && pb<15);
				
				Feuille nouvfe = {i,j,0,r};
				datas->spawning.push_back(nouvfe);
			}
		}	
	}							
}

void Game::updateSpawn(float dt) {
	fillTheBlank();
	ADSRComponent* transitionCree = ADSR(datas->spawn);

	if (!datas->spawning.empty()) {
		transitionCree->active = true;
		for ( std::vector<Feuille>::reverse_iterator it = datas->spawning.rbegin(); it != datas->spawning.rend(); ++it ) {
			if (it->fe == 0) {
				Entity e = datas->CreateEntity();
				theTransformationSystem.Add(e);
				TRANSFORM(e)->position = gridCoordsToPosition(it->X, it->Y);
				theRenderingSystem.Add(e);
				std::stringstream s;
				s << it->type << ".png";
				RENDERING(e)->texture = theRenderingSystem.loadTextureFile(s.str());
				RENDERING(e)->size = size * scale;
				theADSRSystem.Add(e);
				ADSR(e)->idleValue = size * scale;
				theGridSystem.Add(e);
				GRID(e)->type = it->type;
				GRID(e)->i = it->X;
				GRID(e)->j = it->Y;
				it->fe = e;
				std::cout << "nouvelle feuille en ("<<it->X<<","<<it->Y<<")\n";	
			} else if (transitionCree->value == 1){
				TRANSFORM(it->fe)->rotation = 0;
			} else {
				TRANSFORM(it->fe)->rotation = transitionCree->value*7;
			}
		}
		if (transitionCree->value == 1) {
			datas->spawning.clear();
			datas->state = UserInput;
			transitionCree->activationTime = 0;
			transitionCree->active = false;
		}
	} else {
		transitionCree->activationTime = 0;
		transitionCree->active = false;
		datas->state = UserInput;
	}
}

void Game::updateUserInput(float dt) {
	/* drag/drop of cell */
	if (!theTouchInputManager.wasTouched() && 
		theTouchInputManager.isTouched()) {
		// don't start new drag while the previous one isn't finished
		if (!datas->dragged) {
			// start drag
			// find nearest cell
			const Vector2& pos = theTouchInputManager.getTouchLastPosition();
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
		}
	} else if (theTouchInputManager.wasTouched() && datas->dragged && ADSR(datas->dragged)->active) {
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

				std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison();
				if (combinaisons.empty()) {
					// revert swap
					datas->state = RevertSwap;
					datas->dragged = 0;
				} else {
					ADSR(datas->swapper)->activationTime = 0;
					datas->dragged = 0;
					datas->state = Delete;
					ADSR(datas->remove)->activationTime = 0;
					datas->removing = combinaisons;
				}
			}
		}
	} else {
		ADSR(datas->swapper)->active = false;
		if (datas->dragged) ADSR(datas->dragged)->active = false;
	}

	if (datas->dragged) {
		// reset 'dragged' cell only if both anim are ended
		if (!ADSR(datas->dragged)->active && !ADSR(datas->swapper)->active) {
			if (ADSR(datas->swapper)->activationTime <= 0 && ADSR(datas->dragged)->activationTime <= 0) {
				datas->dragged = 0;
			}
		}
	}
}

void Game::updateRevertSwap(float dt) {
	if (ADSR(datas->swapper)->activationTime <= 0) {
		datas->state = UserInput;
	}
}

void Game::updateDelete(float dt) {
	ADSRComponent* transitionSuppr = ADSR(datas->remove);
	if (!datas->removing.empty()) {
		transitionSuppr->active = true;
		for ( std::vector<Combinais>::reverse_iterator it = datas->removing.rbegin(); it != datas->removing.rend(); ++it ) {
			if (transitionSuppr->value == 1)
				datas->hud.ScoreCalc(it->points.size());	
			for ( std::vector<Vector2>::reverse_iterator itV = (it->points).rbegin(); itV != (it->points).rend(); ++itV ) {
				Entity e = theGridSystem.GetOnPos(itV->X,itV->Y);
				TRANSFORM(e)->rotation = transitionSuppr->value*7;
				if (transitionSuppr->value == 1) {
					std::cout << "suppression en ("<<itV->X<<","<<itV->Y<<")\n";
					if (e){
						theRenderingSystem.Delete(e);
						theTransformationSystem.Delete(e);
						theADSRSystem.Delete(e);
						theGridSystem.Delete(e);
					}
				}
			}
			
		}
		if (transitionSuppr->value == 1) {
			datas->removing.clear();
			datas->falling = theGridSystem.TileFall();
			datas->state = Fall;
			ADSR(datas->fall)->activationTime = 0;
		}
	} else {
		transitionSuppr->active = false;
		datas->state = Spawn;
	}
	//std::cout << transitionSuppr->value << std::endl;
}

void Game::updateFall(float dt) {
	ADSRComponent* transition = ADSR(datas->fall);
	if (!datas->falling.empty()) {
		transition->active = true;
		for(std::vector<CellFall>::iterator it=datas->falling.begin(); it!=datas->falling.end(); ++it) {
			const CellFall& f = *it;
			Vector2 targetPos = gridCoordsToPosition(f.x, f.toY);
			Vector2 originPos = gridCoordsToPosition(f.x, f.fromY);
			GRID(f.e)->checkedH = GRID(f.e)->checkedV = false;
			TRANSFORM(f.e)->position = MathUtil::Lerp(originPos, targetPos, transition->value);
			if (transition->value == 1) {
				GRID(f.e)->j = f.toY;
			}
		}
		if (transition->value == 1) {
			datas->falling.clear();
			datas->state = Spawn;
		}
	} else {
		transition->active = false;
		datas->state = Spawn;
	}
}

void Game::tick(float dt) {
	theTouchInputManager.Update(dt);

	switch (datas->state) {
		case Spawn:
			updateSpawn(dt);
			break;
		case UserInput:
			updateUserInput(dt);
			break;
		case RevertSwap:
			updateRevertSwap(dt);
			break;
		case Delete:
			updateDelete(dt);
			break;
		case Fall:
			updateFall(dt);
			break;
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

	//fillTheBlank();
	datas->hud.Update(dt);
	theTransformationSystem.Update(dt);
	theTextRenderingSystem.Update(dt);
	theRenderingSystem.Update(dt);
}
