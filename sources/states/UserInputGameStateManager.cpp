#include "UserInputGameStateManager.h"
#include "GridSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "base/TouchInputManager.h"
#include "systems/ADSRSystem.h"
#include "base/EntityManager.h"
#include "Game.h"

static void activateADSR(Entity e, float a, float s);
static void diffToGridCoords(const Vector2& c, int* i, int* j);

UserInputGameStateManager::UserInputGameStateManager() {

}

void UserInputGameStateManager::Setup() {
	eSwapper = theEntityManager.CreateEntity();
	theADSRSystem.Add(eSwapper);
	ADSR(eSwapper)->idleValue = 0;
	ADSR(eSwapper)->attackValue = 1.0;
	ADSR(eSwapper)->attackTiming = 0.2;
	ADSR(eSwapper)->decayTiming = 0;
	ADSR(eSwapper)->sustainValue = 1.0;
	ADSR(eSwapper)->releaseTiming = 0.2;
}
	
void UserInputGameStateManager::Enter() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	dragged = 0;
	ADSR(eSwapper)->active = false;
}

GameState UserInputGameStateManager::Update(float dt) {
	/* drag/drop of cell */
	if (!theTouchInputManager.wasTouched() && 
		theTouchInputManager.isTouched()) {
		// don't start new drag while the previous one isn't finished
		if (!dragged) {
			// start drag: find nearest cell
			const Vector2& pos = theTouchInputManager.getTouchLastPosition();
			int i, j;
			for( i=0; i<theGridSystem.GridSize && !dragged; i++) {
				for(j=0; j<theGridSystem.GridSize; j++) {
					Entity e = theGridSystem.GetOnPos(i,j);
						if(e && ButtonSystem::inside(
						pos,
						TRANSFORM(e)->worldPosition,
						RENDERING(e)->size)) {
						dragged = e;
						break;
					}
				}
			}
			if (dragged) {
				i--;
				originI = i;
				originJ = j;

				activateADSR(dragged, 1.4, 1.2);

				// active neighboors
				activateADSR(theGridSystem.GetOnPos(i+1,j), 1.2, 1.1);
				activateADSR(theGridSystem.GetOnPos(i,j+1), 1.2, 1.1);
				activateADSR(theGridSystem.GetOnPos(i-1,j), 1.2, 1.1);
				activateADSR(theGridSystem.GetOnPos(i,j-1), 1.2, 1.1);
			}
		}
	} else if (theTouchInputManager.wasTouched() && dragged && ADSR(dragged)->active) {
		if (theTouchInputManager.isTouched()) {
			// continue drag
			Vector2 diff = theTouchInputManager.getTouchLastPosition() 
				- Game::GridCoordsToPosition(originI, originJ);

			if (diff.Length() > 1) {
				int i,j;
				diffToGridCoords(diff, &i, &j);

				if (theGridSystem.IsValidGridPosition(originI + i, originJ + j)) {
					if ((swapI == i && swapJ == j) ||
						(swapI == 0 && swapJ == 0)) {
						ADSR(eSwapper)->active = true;
						swapI = i;
						swapJ = j;
					} else {
						if (ADSR(eSwapper)->activationTime > 0) {
							ADSR(eSwapper)->active = false;
						} else {
							ADSR(eSwapper)->active = true;
							swapI = i;
							swapJ = j;
						}
					}
				} else {
					ADSR(eSwapper)->active = false;
				}
			} else {
				ADSR(eSwapper)->active = false;
			}
		} else {
			std::cout << "release " << std::endl;
			// release drag
			ADSR(theGridSystem.GetOnPos(originI,originJ))->active = false;
			if (theGridSystem.IsValidGridPosition(originI+1, originJ))
				ADSR(theGridSystem.GetOnPos(originI+1,originJ))->active = false;
			if (theGridSystem.IsValidGridPosition(originI, originJ+1))
				ADSR(theGridSystem.GetOnPos(originI,originJ+1))->active = false;
			if (theGridSystem.IsValidGridPosition(originI-1,originJ))
				ADSR(theGridSystem.GetOnPos(originI-1,originJ))->active = false;
			if (theGridSystem.IsValidGridPosition(originI,originJ-1))
				ADSR(theGridSystem.GetOnPos(originI,originJ-1))->active = false;
			ADSR(eSwapper)->active = false;

			/* must swap ? */			
			if (ADSR(eSwapper)->value >= 0.99) {
				Entity e2 = theGridSystem.GetOnPos(originI+ swapI,originJ+ swapJ);
				GRID(e2)->i = originI;
				GRID(e2)->j = originJ;
				GRID(e2)->checkedH = false;
				GRID(e2)->checkedV = false;
				
				Entity e1 = dragged ;
				GRID(e1)->i = originI + swapI;
				GRID(e1)->j = originJ + swapJ;
				GRID(e1)->checkedH = false;
				GRID(e1)->checkedV = false;

				std::vector<Combinais> combinaisons = theGridSystem.LookForCombinaison(false);
				if (combinaisons.empty()) {
				//if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS && combinaisons.empty()) {
					// revert swap
					GRID(e1)->i = originI;
					GRID(e1)->j = originJ;
					GRID(e2)->i = originI + swapI;
					GRID(e2)->j = originJ + swapJ;
					return UserInput;
				} else {
					return Delete;
				}
			}
		}
	} else {
		ADSR(eSwapper)->active = false;
		if (dragged) ADSR(dragged)->active = false;
	}

	if (dragged) {
		// reset 'dragged' cell only if both anim are ended
		if (!ADSR(dragged)->active && !ADSR(eSwapper)->active) {
			if (ADSR(eSwapper)->activationTime <= 0 && ADSR(dragged)->activationTime <= 0) {
				dragged = 0;
			}
		}
	}
	return UserInput;
}

void UserInputGameStateManager::BackgroundUpdate(float dt) {
	for(int i=0; i<theGridSystem.GridSize; i++) {
		for(int j=0; j<theGridSystem.GridSize; j++) {
			Entity e = theGridSystem.GetOnPos(i,j);
			if (e) {
				RENDERING(e)->size = ADSR(e)->value;
			}
		}
	}

	if (ADSR(eSwapper)->activationTime > 0) {
		Vector2 pos1 = Game::GridCoordsToPosition(originI, originJ);
		Vector2 pos2 = Game::GridCoordsToPosition(originI + swapI, originJ + swapJ);

		Vector2 interp1 = MathUtil::Lerp(pos1, pos2, ADSR(eSwapper)->value);
		Vector2 interp2 = MathUtil::Lerp(pos2, pos1, ADSR(eSwapper)->value);

		TRANSFORM(theGridSystem.GetOnPos(originI,originJ))->position = interp1;
		TRANSFORM(theGridSystem.GetOnPos(originI + swapI,originJ + swapJ))->position = interp2;
	}
}
	
void UserInputGameStateManager::Exit() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

static void activateADSR(Entity e, float a, float s) {
	if (!e)
		return;
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


