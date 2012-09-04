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
#include "Go100SecondsModeManager.h"

#include <iomanip>
#include <sstream>

#include <base/PlacementHelper.h>
#include <base/MathUtil.h>

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/PhysicsSystem.h"

#include "DepthLayer.h"
#include "CombinationMark.h"
#include "GridSystem.h"

Go100SecondsGameModeManager::Go100SecondsGameModeManager(HeriswapGame* game, SuccessManager* successMgr, StorageAPI* sAPI) : GameModeManager(game, successMgr, sAPI) {
}

Go100SecondsGameModeManager::~Go100SecondsGameModeManager() {
}

void Go100SecondsGameModeManager::Setup() {
	GameModeManager::Setup();

	validBranchPos.clear();
	#include "PositionFeuilles.h"
	for (int i=0; i<8*6; i++) {
		Vector2 v(PlacementHelper::GimpXToScreen(pos[3*i]), PlacementHelper::GimpYToScreen(pos[3*i+1]));
		Render truc = {v, MathUtil::ToRadians(pos[3*i+2])};
		validBranchPos.push_back(truc);
	}

	for (int i = 0; i < 100;  i++) {
		squallLeaves.push_back(GameModeManager::createAndAddLeave(bonus, Vector2(0, 0), MathUtil::RandomFloat(7)));
		Entity e = squallLeaves[i];

		ADD_COMPONENT(e, Physics);
		PHYSICS(e)->mass = MathUtil::RandomFloatInRange(1.f, 10.f);
		PHYSICS(e)->gravity = Vector2(0,0*-10.f);

		RENDERING(e)->hide = true;
	}
}
#define RATE 1
void Go100SecondsGameModeManager::Enter() {
	time = 0;
	limit = 100;
	points = 0;
	squallGo = false;
	squallDuration = 0.f;
	bonus = MathUtil::RandomInt(theGridSystem.Types);
	pts.clear();
	pts.push_back(Vector2(0,0));

	pts.push_back(Vector2(limit,1));//need limit leaves to end game

	generateLeaves(0, 8);

	for (unsigned int i = 0; i < squallLeaves.size();  i++)
		RENDERING(squallLeaves[i])->texture = theRenderingSystem.loadTextureFile(HeriswapGame::cellTypeToTextureNameAndRotation(bonus, &TRANSFORM(squallLeaves[i])->rotation));

	scores = storageAPI->savedScores(Normal, theGridSystem.sizeToDifficulty());
	rank = GameModeManager::getMyRank(time, Go100Seconds, scores);

	GameModeManager::Enter();
}

void Go100SecondsGameModeManager::squall() {
	squallGo = true;
	float minX = PlacementHelper::ScreenWidth/2.+1;
	float maxX = PlacementHelper::ScreenWidth/2.+3;

	float minY = PlacementHelper::GimpYToScreen(1000);
	float maxY = -PlacementHelper::ScreenHeight/2.;
	for (unsigned int i = 0; i < squallLeaves.size();  i++) {

		Entity  e = squallLeaves[i];
		TRANSFORM(e)->position = Vector2(MathUtil::RandomFloatInRange(minX, maxX), MathUtil::RandomFloatInRange(minY, maxY));
		RENDERING(e)->texture = theRenderingSystem.loadTextureFile(HeriswapGame::cellTypeToTextureNameAndRotation(bonus, &TRANSFORM(e)->rotation));
		RENDERING(e)->hide = false;

		Force force;
		force.vector = Vector2(-45, 0);
		force.point =  Vector2( 0, TRANSFORM(e)->size.Y/2.);

		std::pair<Force, float> f (force, 1.);
		PHYSICS(e)->forces.push_back(f);
		PHYSICS(e)->mass = MathUtil::RandomFloatInRange(1.f, 10.f); // go update (dumb PhysicsSystem!)
	}
	//herisson's Y. He'll change his bonus behind this leaf
	TRANSFORM(squallLeaves[0])->position = Vector2(MathUtil::RandomFloatInRange(minX, maxX), PlacementHelper::GimpYToScreen(1028));
}

void Go100SecondsGameModeManager::Exit() {
	GameModeManager::Exit();
}

void Go100SecondsGameModeManager::GameUpdate(float dt) {
	time+=dt;
	//squall is running...
	if (squallGo) {
		squallDuration += dt;
		//if the central leaf is next to the herisson, change his bonus
		if (TRANSFORM(squallLeaves[0])->position.X <= TRANSFORM(herisson)->position.X) {
			LoadHerissonTexture(bonus+1);
			RENDERING(herisson)->texture = theRenderingSystem.loadTextureFile(c->anim[0]);
		}
		//make the tree leaves grow ...
		for (unsigned int i = 0; i < branchLeaves.size(); i++) {
			TRANSFORM(branchLeaves[i].e)->size =
				Vector2(HeriswapGame::CellSize(8) * HeriswapGame::CellContentScale() * MathUtil::Min(squallDuration, 1.f));
		}
		//check if every leaves has gone...
		bool ended = true;
		for (unsigned int i = 0 ; i < squallLeaves.size() ; i++) {
			if (TRANSFORM(squallLeaves[i])->position.X >= -PlacementHelper::ScreenWidth * 0.5 - TRANSFORM(squallLeaves[i])->size.X)
				ended = false;
		}
		//YES : stop the squall, keep playing !
		if (ended) {
			squallGo = false;
			squallDuration = 0.f;
			for (unsigned int i = 0; i < squallLeaves.size(); i++) {
				RENDERING(squallLeaves[i])->hide = true;
				PHYSICS(squallLeaves[i])->linearVelocity = Vector2::Zero;
				PHYSICS(squallLeaves[i])->angularVelocity = 0.f;
				PHYSICS(squallLeaves[i])->mass = 0.f; // stop update (dumb PhysicsSystem!)
			}


		}
	} else {
		//oh noes, no longer leaf on tree ! Give me new one
		if (branchLeaves.size() == 0) {
			//Ok, but first u'll have a new bonus
			bonus = MathUtil::RandomInt(theGridSystem.Types);
			//And leaves aren't magic, they need to grow ... be patient.
			generateLeaves(0, 8);
			for (unsigned int i = 0; i < branchLeaves.size(); i++)
				TRANSFORM(branchLeaves[i].e)->size = Vector2::Zero;
			//The squall will make them grow
			squall();
		}
	}

}

float Go100SecondsGameModeManager::GameProgressPercent() {
	return MathUtil::Min(1.0f, (float)time/limit);
}

void Go100SecondsGameModeManager::UiUpdate(float dt) {
	//Score
	{
	std::stringstream a;
	a.precision(0);
	a << rank << ". ";
	a << std::fixed << points;
	TEXT_RENDERING(uiHelper.scoreProgress)->text = a.str();
	}

	updateHerisson(dt, time, 0);

#ifdef DEBUG
	if (_debug) {
		for(int i=0; i<8; i++) {
			std::stringstream text;
			text << countBranchLeavesOfType(i);
			TEXT_RENDERING(debugEntities[2*i+1])->text = text.str();
			TEXT_RENDERING(debugEntities[2*i+1])->hide = false;
			TEXT_RENDERING(debugEntities[2*i+1])->color = Color(0.2, 0.2, 0.2);
		}
	}
#endif
}

void Go100SecondsGameModeManager::ScoreCalc(int nb, unsigned int type) {
	if (type == bonus) {
		points += 10*2*nb*nb*nb/6;
		deleteLeaves(~0b0, 2*nb);
	} else {
		points += 10*nb*nb*nb/6;
		deleteLeaves(~0b0, nb);
	}

	//update rank
	rank = GameModeManager::getMyRank((float)points, Normal, scores);

}

void Go100SecondsGameModeManager::TogglePauseDisplay(bool paused) {
	GameModeManager::TogglePauseDisplay(paused);
}

void Go100SecondsGameModeManager::WillScore(int count, int type, std::vector<Entity>& LeavesToDelete) {
    int nb = MathUtil::Min((int)branchLeaves.size(), count);
    for (unsigned int i=0; nb>0 && i<branchLeaves.size(); i++) {
		CombinationMark::markCellInCombination(branchLeaves[i].e);
        LeavesToDelete.push_back(branchLeaves[i].e);
        nb--;
    }
}

int Go100SecondsGameModeManager::saveInternalState(uint8_t** out) {
    uint8_t* tmp;
    int parent = GameModeManager::saveInternalState(&tmp);
    uint8_t* ptr = *out = new uint8_t[parent];
    ptr = (uint8_t*) mempcpy(ptr, tmp, parent);

    delete[] tmp;
    return (parent);
}

const uint8_t* Go100SecondsGameModeManager::restoreInternalState(const uint8_t* in, int size) {
    in = GameModeManager::restoreInternalState(in, size);
    
    TRANSFORM(herisson)->position.X = GameModeManager::position(time);
    
    return in;
}
