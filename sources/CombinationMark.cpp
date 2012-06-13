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
#include "CombinationMark.h"

#include <base/MathUtil.h>

#include "TwitchSystem.h"
#include "systems/TransformationSystem.h"
#include "GridSystem.h"
#include "Game.h"

void markCellInCombination(Entity e);
        void clearCellInCombination(Entity e);


void CombinationMark::markCellInCombination(Entity e){
    if (!e || TWITCH(e)->speed > 0)
        return;
    float angle = TRANSFORM(e)->rotation;
    TWITCH(e)->minAngle = angle - 0.4;
    TWITCH(e)->maxAngle = angle + 0.4;
    TWITCH(e)->variance = MathUtil::RandomFloat() * 0.2;
    TWITCH(e)->speed = MathUtil::RandomFloatInRange(10, 15);
}

void CombinationMark::clearCellInCombination(Entity e) {
    if (!e)
		return;

	TWITCH(e)->speed = 0;

    // restore proper orientation
    TRANSFORM(e)->rotation = Game::cellTypeToRotation(GRID(e)->type);
}
