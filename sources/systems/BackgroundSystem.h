/*
    This file is part of Prototype.

    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Jordane Pelloux-Prayer

    Prototype is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    Prototype is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Prototype. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "systems/System.h"

struct BackgroundComponent {
    BackgroundComponent() : speed(0), group(0), enable(false), visible(false) {}
    float speed;
    int group;
    bool enable;
    bool visible;
};

#define theBackgroundSystem BackgroundSystem::GetInstance()
#define BACKGROUND(e) theBackgroundSystem.Get(e)

UPDATABLE_SYSTEM(Background)

public:
    void hideAll();
    void showAll();

    void initCloud(Entity e, int group);
private:
    float skySpeed;
    //clouds
    Interval<float> cloudStartX;
    Interval<float> cloudY[3];
    Interval<float> cloudSize[3];
    Interval<float> cloudSpeed[3];
    std::vector<std::string> textures[3];
    std::vector<float> width2HeightRatio[3];

};
