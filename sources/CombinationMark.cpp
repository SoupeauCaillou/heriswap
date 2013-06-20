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
#include "HeriswapGame.h"

#include "systems/HeriswapGridSystem.h"
#include "systems/TwitchSystem.h"

#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

void markCellInCombination(Entity e);
void clearCellInCombination(Entity e);


void CombinationMark::markCellInCombination(Entity e){
    if (!e || TWITCH(e)->speed > 0)
        return;
    float angle = TRANSFORM(e)->rotation;
    TWITCH(e)->minAngle = angle - 0.4;
    TWITCH(e)->maxAngle = angle + 0.4;
    TWITCH(e)->variance = glm::linearRand(0.0f, 1.0f) * 0.2f;
    TWITCH(e)->speed = glm::linearRand(10.0f, 15.0f);
}

void CombinationMark::clearCellInCombination(Entity e) {
    if (!e)
        return;

    TWITCH(e)->speed = 0;

    // restore proper orientation
    TRANSFORM(e)->rotation = HeriswapGame::cellTypeToRotation(HERISWAPGRID(e)->type);
}
