#pragma once

#include <sstream>

#include "base/EntityManager.h"
#include "base/MathUtil.h"

#include "systems/System.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ADSRSystem.h"

#include "Game.h"

/* state :
 * 1 = spawn
 * 2 = fall
 * 3 = delete
*/
struct CombinationMarkComponent {
	CombinationMarkComponent() {
		state = 0;
	}
	int state;
};


#define theCombinationMarkSystem CombinationMarkSystem::GetInstance()
#define COMBIN(e) theCombinationMarkSystem.Get(e)

UPDATABLE_SYSTEM(CombinationMark)

public : 
	void NewMarks(int stat, Vector2 coord);
	void DeleteMarks(int stat);

};

