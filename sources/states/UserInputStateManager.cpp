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

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/compatibility.hpp>

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

void UserInputGameStateManager::Setup() {
	swapAnimation = theEntityManager.CreateEntity("swapAnimation");
	ADD_COMPONENT(swapAnimation, ADSR);

	ADD_COMPONENT(swapAnimation, Sound);
	originI = originJ = -1;
    swapI = swapJ = 0;

	ADSR(swapAnimation)->idleValue = 0;
	ADSR(swapAnimation)->attackValue = 1.0;
	ADSR(swapAnimation)->decayTiming = 0;
	ADSR(swapAnimation)->sustainValue = 1.0;

	rollback = theEntityManager.CreateEntity("rollback");
	ADD_COMPONENT(rollback, Morphing);
	MORPHING(rollback)->timing = 0.1;

	newGame = true;
}

void UserInputGameStateManager::Enter() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
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
			const glm::vec2& p = combi[i].points[j];
			if (p.x == g->i && p.y == g->j)
				return true;
		}
	}
	return false;
}

static Entity cellUnderFinger(const glm::vec2& pos, bool) {
	std::vector<Combinais> combinaisons;// = theGridSystem.LookForCombination(false,false);
	const float maxDist = HeriswapGame::CellSize(theGridSystem.GridSize, 0).y;

	// 4 nearest
	Entity e = 0;
	float nearestDist = maxDist;
	bool nearestInCombi = 0;

	std::vector<Entity> leaves = theGridSystem.RetrieveAllEntityWithComponent();
	for (std::vector<Entity>::iterator it=leaves.begin(); it!=leaves.end(); ++it) {
		float sqdist = glm::distance2(pos, TRANSFORM(*it)->worldPosition);
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

static Entity moveToCell(Entity original, const glm::vec2& move, float) {
#ifdef ANDROID
	if (glm::length2(move) < threshold)
		return 0;
#endif

	int i = GRID(original)->i;
	int j = GRID(original)->j;

	if (glm::abs(move.x) > glm::abs(move.y)) {
		if (move.x < 0) {
			return theGridSystem.GetOnPos(i-1,j);
		} else {
			return theGridSystem.GetOnPos(i+1,j);
		}
	} else {
		if (move.y < 0) {
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
			const glm::vec2& pos = theTouchInputManager.getTouchLastPosition(0);
			currentCell = cellUnderFinger(pos, true);

			if (currentCell) {
				CombinationMark::markCellInCombination(currentCell);
			}
		}
	} else {
		const glm::vec2 posA = HeriswapGame::GridCoordsToPosition(GRID(currentCell)->i, GRID(currentCell)->j,theGridSystem.GridSize);
		const glm::vec2& pos = theTouchInputManager.getTouchLastPosition(0);
		// compute move
		glm::vec2 move = pos - posA;

		if (theTouchInputManager.isTouched(0)) {
			// swap cell on axis
			Entity c = moveToCell(currentCell, move, TRANSFORM(currentCell)->size.x * 0.01);

			// same cell, keep going
			if (c && (!swappedCell || swappedCell == c)) {
				if (!swappedCell) {
					CombinationMark::markCellInCombination(c);
				}
				swappedCell = c;

				const glm::vec2 posB = HeriswapGame::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
				float t = glm::min(1.0f, glm::length(move));
				TRANSFORM(currentCell)->position = glm::lerp(posA, posB, t);
				TRANSFORM(swappedCell)->position = glm::lerp(posA, posB, 1 - t);
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

				if (glm::length(move) < TRANSFORM(currentCell)->size.x * 0.5) {
					// restore position
					TRANSFORM(currentCell)->position = posA;
					TRANSFORM(swappedCell)->position = HeriswapGame::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);
				} else {
					const glm::vec2 posB = HeriswapGame::GridCoordsToPosition(GRID(swappedCell)->i, GRID(swappedCell)->j,theGridSystem.GridSize);

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
						MORPHING(rollback)->elements.push_back(new TypedMorphElement<glm::vec2>(
							&TRANSFORM(currentCell)->position, TRANSFORM(currentCell)->position, posA));
						MORPHING(rollback)->elements.push_back(new TypedMorphElement<glm::vec2>(
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
				glm::vec2 size = HeriswapGame::CellSize(theGridSystem.GridSize, GRID(e)->type); 
				float scale = ADSR(e)->value / size.x;
				TRANSFORM(e)->size = size * scale;
			}
		}
	}

	if (ADSR(swapAnimation)->activationTime >= 0 && originI >= 0 && originJ >= 0) {

		glm::vec2 pos1 = HeriswapGame::GridCoordsToPosition(originI, originJ, theGridSystem.GridSize);
		glm::vec2 pos2 = HeriswapGame::GridCoordsToPosition(originI + swapI, originJ + swapJ, theGridSystem.GridSize);

		glm::vec2 interp1 = glm::lerp(pos1, pos2, ADSR(swapAnimation)->value);
		glm::vec2 interp2 = glm::lerp(pos2, pos1, ADSR(swapAnimation)->value);

		Entity e1 = theGridSystem.GetOnPos(originI,originJ);
		Entity e2 = theGridSystem.GetOnPos(originI + swapI,originJ + swapJ);

		if (e1)
			TRANSFORM(e1)->position = interp1;
		if (e2)
			TRANSFORM(e2)->position = interp2;
	}
}

void UserInputGameStateManager::Exit() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
    inCombinationCells.clear();

    successMgr->sLuckyLuke();
    successMgr->sWhatToDo(false, 0.f);
}
