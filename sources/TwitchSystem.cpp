#include "TwitchSystem.h"

#include <base/EntityManager.h>
#include <base/MathUtil.h>

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
        if (tc->speed * dt > MathUtil::Abs(target - trc->rotation)) {
            trc->rotation = target;

            if (tc->target == TwitchComponent::MIN)
                tc->target = TwitchComponent::MAX;
            else
                tc->target = TwitchComponent::MIN;
            tc->currentVariance = MathUtil::RandomFloatInRange(-tc->variance, tc->variance);
        } else {
            float sign = target - trc->rotation;
            trc->rotation = trc->rotation + sign * tc->speed * dt;
        }
    }
}

