#pragma once

#include "base/EntityManager.h"
#include "base/MathUtil.h"

#include "systems/System.h"

struct TwitchComponent {
    TwitchComponent() : minAngle(0), maxAngle(0), variance(0), speed(0), target(MIN), currentVariance(0) {}
    float minAngle, maxAngle;
    float variance;
    float speed;

    enum Target { MIN, MAX } target;
    float currentVariance;
};


#define theTwitchSystem TwitchSystem::GetInstance()
#define TWITCH(e) theTwitchSystem.Get(e)

UPDATABLE_SYSTEM(Twitch)
};

