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
#include "TwitchSystem.h"

#include <base/EntityManager.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include "systems/TransformationSystem.h"

INSTANCE_IMPL(TwitchSystem);

TwitchSystem::TwitchSystem() : ComponentSystemImpl<TwitchComponent>("twitch_") {
}

void TwitchSystem::DoUpdate(float dt) {
    for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
        Entity a = (*it).first;
        TwitchComponent* tc = (*it).second;
        TransformationComponent* trc = TRANSFORM(a);

        if (tc->maxAngle == tc->minAngle || tc->speed == 0)
            continue;

        float target = (tc->target == TwitchComponent::MIN) ?
            (tc->minAngle + tc->currentVariance) : (tc->maxAngle + tc->currentVariance);
        if (tc->speed * dt > glm::abs(target - trc->rotation)) {
            trc->rotation = target;

            if (tc->target == TwitchComponent::MIN)
                tc->target = TwitchComponent::MAX;
            else
                tc->target = TwitchComponent::MIN;
            tc->currentVariance = glm::linearRand(-tc->variance, tc->variance);
        } else {
            float sign = target - trc->rotation;
            trc->rotation = trc->rotation + sign * tc->speed * dt;
        }
    }
}

