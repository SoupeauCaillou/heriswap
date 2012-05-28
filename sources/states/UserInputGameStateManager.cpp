#include "UserInputGameStateManager.h"

#ifndef ANDROID
#include <GL/glew.h>
#include <GL/glfw.h>
#endif

#include "base/Log.h"
#include "base/TouchInputManager.h"
#include "base/EntityManager.h"

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"
#include "systems/MorphingSystem.h"

#include "GridSystem.h"
#include "Game.h"
#include "CombinationMark.h"

static void activateADSR(Entity e, float a, float s);
static void diffToGridCoords(const Vector2& c, int* i, int* j);


void UserInputGameStateManager::setAnimSpeed() {
	int difficulty = (theGridSystem.GridSize!=8)+1; //1 : normal, 2 : easy

	ADSR(eSwapper)->idleValue = 0;
	ADSR(eSwapper)->attackValue = 1.0;
	ADSR(eSwapper)->attackTiming = 0.07 * difficulty;
	ADSR(eSwapper)->decayTiming = 0;
	ADSR(eSwapper)->sustainValue = 1.0;
	ADSR(eSwapper)->releaseTiming = 0.07 * difficulty;
}

void UserInputGameStateManager::Setup() {
	eSwapper = theEntityManager.CreateEntity();
	ADD_COMPONENT(eSwapper, ADSR);

	ADD_COMPONENT(eSwapper, Sound);
	originI = originJ = -1;
    swapI = swapJ = 0;

	setAnimSpeed();
	
	rollback = theEntityManager.CreateEntity();
	ADD_COMPONENT(rollback, Morphing);
	MORPHING(rollback)->timing = 0.1;
}

void UserInputGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	dragged = 0;
	ADSR(eSwapper)->active = false;
	ADSR(eSwapper)->activationTime = 0;
	originI = originJ = -1;
	dragged = 0;
	
	currentCell = swappedCell = 0;

	successMgr->timeUserInputloop = 0.f;

	successMgr->sBimBamBoum(0);
}

static Entity cellUnderFinger(const Vector2& pos) {
	int i, j;
	for( i=0; i<theGridSystem.GridSize; i++) {
		for(j=0; j<theGridSystem.GridSize; j++) {
			Entity e = theGridSystem.GetOnPos(i,j);
			if (!e) continue;
			
			if (i>=1 && i <theGridSystem.GridSize-1) {
				if (ButtonSystem::inside(pos, TRANSFORM(e)->worldPosition, TRANSFORM(e)->size)) {
					return e;
				}
			} else {
				Vector2 s = TRANSFORM(e)->size;
				if (ButtonSystem::inside(pos, TRANSFORM(e)->worldPosition + s.X * 0.2, Vector2(s.X * 1.4, 1))) {
					return e;
				}				
			}
		}
	}
	return 0;
}

static Entity moveToCell(Entity original, const Vector2& move, float threshold) {
	if (move.LengthSquared() < threshold)
		return 0;
	
	int i = GRID(original)->i;
	int j = GRID(original)->j;

	if (MathUtil::Abs(move.X) > MathUtil::Abs(move.Y)) {
		if (move.X < 0) {
			return theGridSystem.GetOnPos(i-1,j);
		} else {
			return theGridSystem.GetOnPos(i+1,j);
		}
	} else {
		if (move.Y < 0) {
			return theGridSystem.GetOnPos(i,j-1);
		} else {
			return theGridSystem.GetOnPos(i,j+1);
		}		
	}
}

static void exchangeGridCoords(Entity a, Entity b) {
	int iA = GRID(a)->i;
	int jA = GRID(a)->j;
	GRID(a)->i = GRID(b)->i;
	GRID(a)->j = GRID(b)->j;
	GRID(b)->i = iA;
	GRID(b)->j = jA;
}

GameState UserInputGameStateManager::Update(float dt) {
	successMgr->timeUserInputloop += dt;
	successMgr->sWhatToDo(theTouchInputManager.wasTouched() && theTouchInputManager.isTouched(), dt);

	if (MORPHING(rollback)->active) {
		return UserInput;
	}

	if (!currentCell) {
		// beginning of drag
		if (!theTouchInputManager.wasTouched() &&
			theTouchInputManager.isTouched()) {			
			const Vector2& pos = theTouchInputManager.getTouchLastPosition();
			currentCell = cellUnderFinger(pos);
			
			if (currentCell) {
				CombinationMark::markCellInCombination(currentCell);
			}
		}
	} else {
		const Vector2 posA = Game::GridCoordsToPosition(GRID(currentCell)->i, GRID(currentCell)->j,theGridSystem.GridSize);
		const Vector2& pos = theTouchInputManager.getTouchLastPosition();
		// compute move
		Vector2 move = pos - posA;

		if (theTouchInputManager.isTouched()) {
			// swap cell on axis
			Entity c = moveToCell(currentCell, move, TRANSFORM(currentCell)->size.X * 0.01);
			
			// same cell, keep going
			if (c && (!swappedCell || swappedCell == c)) {
				if (!swappedCell) {
					CombinationMark::markCellInCombination(c);	
				}
				swappedCell = c;

				const Vector2 posB = Game::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
				float t = MathUtil::Min(1.0f, move.Length());
				TRANSFORM(currentCell)->position = MathUtil::Lerp(posA, posB, t);
				TRANSFORM(swappedCell)->position = MathUtil::Lerp(posA, posB, 1 - t);
			} else {
				if (swappedCell) {
					CombinationMark::clearCellInCombination(swappedCell);
					// different cell, restore pos
					TRANSFORM(swappedCell)->position = Game::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
				}
				if (!c) {
					TRANSFORM(currentCell)->position = posA;
				} else {
					CombinationMark::markCellInCombination(c);	
				}
				swappedCell = c;
			}
		} else {
			// release
			CombinationMark::clearCellInCombination(currentCell);
			if (swappedCell) {
				CombinationMark::clearCellInCombination(swappedCell);

				if (move.Length() < TRANSFORM(currentCell)->size.X * 0.5) {
					// restore position
					TRANSFORM(currentCell)->position = posA;
					TRANSFORM(swappedCell)->position = Game::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
				} else {
					const Vector2 posB = Game::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);

					int typeA = GRID(currentCell)->type;
					int typeB = GRID(swappedCell)->type;
					// exchange types
					GRID(currentCell)->type = typeB;
					GRID(swappedCell)->type = typeA;
					// check combi
					std::vector<Combinais> combinaisons = theGridSystem.LookForCombination(false,false);
					// restore types
					GRID(currentCell)->type = typeA;
					GRID(swappedCell)->type = typeB;

					if (combinaisons.empty()) {
						// cancel swap
						theMorphingSystem.clear(MORPHING(rollback));
						MORPHING(rollback)->elements.push_back(new TypedMorphElement<Vector2>(
							&TRANSFORM(currentCell)->position, TRANSFORM(currentCell)->position, posA));
						MORPHING(rollback)->elements.push_back(new TypedMorphElement<Vector2>(
							&TRANSFORM(swappedCell)->position, TRANSFORM(swappedCell)->position, posB));
						MORPHING(rollback)->active = true;
						SOUND(eSwapper)->sound = theSoundSystem.loadSoundFile("audio/son_descend.ogg");
					} else {
						GRID(currentCell)->checkedH  = false;
						GRID(currentCell)->checkedV = false;
						GRID(swappedCell)->checkedH = false;
						GRID(swappedCell)->checkedV = false;
						exchangeGridCoords(currentCell, swappedCell);
						TRANSFORM(currentCell)->position = posB;
						TRANSFORM(swappedCell)->position = posA;
						return Delete;
					}
				}
			} else {
				TRANSFORM(currentCell)->position = posA;
			}
			swappedCell = currentCell = 0;
		}
	}
	return UserInput;
}

void UserInputGameStateManager::BackgroundUpdate(float dt __attribute__((unused))) {
	for(int i=0; i<theGridSystem.GridSize; i++) {
		for(int j=0; j<theGridSystem.GridSize; j++) {
			Entity e = theGridSystem.GetOnPos(i,j);
			if (e) {
				TRANSFORM(e)->size = ADSR(e)->value;
			}
		}
	}

	if (ADSR(eSwapper)->activationTime >= 0 && originI >= 0 && originJ >= 0) {

		Vector2 pos1 = Game::GridCoordsToPosition(originI, originJ, theGridSystem.GridSize);
		Vector2 pos2 = Game::GridCoordsToPosition(originI + swapI, originJ + swapJ, theGridSystem.GridSize);

		Vector2 interp1 = MathUtil::Lerp(pos1, pos2, ADSR(eSwapper)->value);
		Vector2 interp2 = MathUtil::Lerp(pos2, pos1, ADSR(eSwapper)->value);

		Entity e1 = theGridSystem.GetOnPos(originI,originJ);
		Entity e2 = theGridSystem.GetOnPos(originI + swapI,originJ + swapJ);

		if (e1)
			TRANSFORM(e1)->position = interp1;
		if (e2)
			TRANSFORM(e2)->position = interp2;
	}
}

void UserInputGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
    inCombinationCells.clear();

    successMgr->sLuckyLuke();
    successMgr->sWhatToDo(false, 0.f);
}

static void activateADSR(Entity e, float a, float s) {
	if (!e)
		return;
	float size = TRANSFORM(e)->size.X;
	ADSRComponent* ac = ADSR(e);
	ac->idleValue = size;
	ac->attackValue = size * a;
	ac->attackTiming = 0.1;
	ac->decayTiming = 0.0;
	ac->sustainValue = size * s;
	ac->releaseTiming = 0.08;
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
