#include "CombinationMark.h"

#include <base/MathUtil.h>

#include "TwitchSystem.h"
#include "systems/TransformationSystem.h"

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
    if (e) TWITCH(e)->speed = 0;
}
