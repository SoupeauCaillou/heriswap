/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "UserInputStateManager.h"

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
#include "CombinationMark.h"

static void activateADSR(Entity e, float a, float s);
static void diffToGridCoords(const Vector2& c, int* i, int* j);


void UserInputGameStateManager::Setup() {
	swapAnimation = theEntityManager.CreateEntity();
	ADD_COMPONENT(swapAnimation, ADSR);

	ADD_COMPONENT(swapAnimation, Sound);
	originI = originJ = -1;
    swapI = swapJ = 0;

	ADSR(swapAnimation)->idleValue = 0;
	ADSR(swapAnimation)->attackValue = 1.0;
	ADSR(swapAnimation)->decayTiming = 0;
	ADSR(swapAnimation)->sustainValue = 1.0;

	rollback = theEntityManager.CreateEntity();
	ADD_COMPONENT(rollback, Morphing);
	MORPHING(rollback)->timing = 0.1;

	newGame = true;
}

void UserInputGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	dragged = 0;
	ADSR(swapAnimation)->active = false;
	ADSR(swapAnimation)->activationTime = 0;
	originI = originJ = -1;
	dragged = 0;

	currentCell = swappedCell = 0;

	successMgr->timeUserInputloop = 0.f;

	successMgr->sBimBamBoum(0);
}

static bool contains(const std::vector<Combinais>& combi, const GridComponent* g) {
	for (unsigned int i=0; i<combi.size(); i++) {
		for (unsigned int j=0; j<combi[i].points.size(); j++) {
			const Vector2& p = combi[i].points[j];
			if (p.X == g->i && p.Y == g->j)
				return true;
		}
	}
	return false;
}

static Entity cellUnderFinger(const Vector2& pos, bool preferInCombi) {
	std::vector<Combinais> combinaisons;// = theGridSystem.LookForCombination(false,false);
	const float maxDist = HeriswapGame::CellSize(theGridSystem.GridSize, 0).Y;

	// 4 nearest
	Entity e = 0;
	float nearestDist = maxDist;
	bool nearestInCombi = 0;

	std::vector<Entity> leaves = theGridSystem.RetrieveAllEntityWithComponent();
	for (std::vector<Entity>::iterator it=leaves.begin(); it!=leaves.end(); ++it) {
		float sqdist = Vector2::DistanceSquared(pos, TRANSFORM(*it)->worldPosition);
		if (sqdist < maxDist) {
			bool inCombi = contains(combinaisons, GRID(*it));

			if (sqdist < nearestDist || (inCombi && !nearestInCombi)) {
				nearestDist = sqdist;
				nearestInCombi = inCombi;
				e = *it;
			}
		}
	}
	return e;
}

static Entity moveToCell(Entity original, const Vector2& move, float threshold) {
#ifdef ANDROID
	if (move.LengthSquared() < threshold)
		return 0;
#endif

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
	if (newGame) {
		newGame = false;
		return CountDown;
	}
	successMgr->timeUserInputloop += dt;
	successMgr->sWhatToDo(theTouchInputManager.wasTouched(0) && theTouchInputManager.isTouched(0), dt);

	if (MORPHING(rollback)->active) {
		return UserInput;
	}

	if (!currentCell) {
		// beginning of drag
		if (!theTouchInputManager.wasTouched(0) &&
			theTouchInputManager.isTouched(0)) {
			const Vector2& pos = theTouchInputManager.getTouchLastPosition(0);
			currentCell = cellUnderFinger(pos, true);

			if (currentCell) {
				CombinationMark::markCellInCombination(currentCell);
			}
		}
	} else {
		const Vector2 posA = HeriswapGame::GridCoordsToPosition(GRID(currentCell)->i, GRID(currentCell)->j,theGridSystem.GridSize);
		const Vector2& pos = theTouchInputManager.getTouchLastPosition(0);
		// compute move
		Vector2 move = pos - posA;

		if (theTouchInputManager.isTouched(0)) {
			// swap cell on axis
			Entity c = moveToCell(currentCell, move, TRANSFORM(currentCell)->size.X * 0.01);

			// same cell, keep going
			if (c && (!swappedCell || swappedCell == c)) {
				if (!swappedCell) {
					CombinationMark::markCellInCombination(c);
				}
				swappedCell = c;

				const Vector2 posB = HeriswapGame::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
				float t = MathUtil::Min(1.0f, move.Length());
				TRANSFORM(currentCell)->position = MathUtil::Lerp(posA, posB, t);
				TRANSFORM(swappedCell)->position = MathUtil::Lerp(posA, posB, 1 - t);
			} else {
				if (swappedCell) {
					CombinationMark::clearCellInCombination(swappedCell);
					// different cell, restore pos
					TRANSFORM(swappedCell)->position = HeriswapGame::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
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
					TRANSFORM(swappedCell)->position = HeriswapGame::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
				} else {
					const Vector2 posB = HeriswapGame::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);

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
						SOUND(swapAnimation)->sound = theSoundSystem.loadSoundFile("audio/son_descend.ogg");
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
				Vector2 size = HeriswapGame::CellSize(theGridSystem.GridSize, GRID(e)->type); 
				float scale = ADSR(e)->value / size.X;
				TRANSFORM(e)->size = size * scale;
			}
		}
	}

	if (ADSR(swapAnimation)->activationTime >= 0 && originI >= 0 && originJ >= 0) {

		Vector2 pos1 = HeriswapGame::GridCoordsToPosition(originI, originJ, theGridSystem.GridSize);
		Vector2 pos2 = HeriswapGame::GridCoordsToPosition(originI + swapI, originJ + swapJ, theGridSystem.GridSize);

		Vector2 interp1 = MathUtil::Lerp(pos1, pos2, ADSR(swapAnimation)->value);
		Vector2 interp2 = MathUtil::Lerp(pos2, pos1, ADSR(swapAnimation)->value);

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
