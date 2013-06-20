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
    along with Prototype.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BackgroundSystem.h"

#include "DepthLayer.h"

#include "base/PlacementHelper.h"
#include "base/Interval.h"

#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"

#define CAMERASPEED -1.f

INSTANCE_IMPL(BackgroundSystem);

BackgroundSystem::BackgroundSystem() : ComponentSystemImpl<BackgroundComponent>("Background") {
    skySpeed = -0.3;

    cloudStartX = Interval<float>(0.0,15.0);

    cloudY[0] = Interval<float>(PlacementHelper::GimpYToScreen(70), PlacementHelper::GimpYToScreen(240));
    cloudY[1] = Interval<float>(PlacementHelper::GimpYToScreen(390), PlacementHelper::GimpYToScreen(490));
    cloudY[2] = Interval<float>(PlacementHelper::GimpYToScreen(560), PlacementHelper::GimpYToScreen(650));

    cloudSize[0] = Interval<float>(PlacementHelper::GimpWidthToScreen(230), PlacementHelper::GimpWidthToScreen(380));
    cloudSize[1] = Interval<float>(PlacementHelper::GimpWidthToScreen(190), PlacementHelper::GimpWidthToScreen(230));
    cloudSize[2] = Interval<float>(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpWidthToScreen(180));

    LOGW("To Delete before commit! ! !");
    LOGI("70 = '" << PlacementHelper::GimpYToScreen(70) << "' 240 = '" << PlacementHelper::GimpYToScreen(240) << "'");
    LOGI("390 = '" << PlacementHelper::GimpYToScreen(390) << "' 490 = '" << PlacementHelper::GimpYToScreen(490) << "'");
    LOGI("560 = '" << PlacementHelper::GimpYToScreen(560) << "' 650 = '" << PlacementHelper::GimpYToScreen(650) << "'");

    LOGI("230 = '" << PlacementHelper::GimpWidthToScreen(230) << "' 380 = '" << PlacementHelper::GimpWidthToScreen(380) << "'");
    LOGI("190 = '" << PlacementHelper::GimpWidthToScreen(190) << "' 230 = '" << PlacementHelper::GimpWidthToScreen(190) << "'");
    LOGI("100 = '" << PlacementHelper::GimpWidthToScreen(100) << "' 180 = '" << PlacementHelper::GimpWidthToScreen(180) << "'");

    cloudSpeed[0] = Interval<float>(-0.4, -0.25);
    cloudSpeed[1] = Interval<float>(-0.25, -0.13);
    cloudSpeed[2] = Interval<float>(-0.1, -0.03);

    textures[0].push_back("haut_0"); width2HeightRatio[0].push_back(138.0 / 260.0);
    textures[0].push_back("haut_1"); width2HeightRatio[0].push_back(197.0 / 336.0);
    textures[0].push_back("haut_2"); width2HeightRatio[0].push_back(208.0 / 413.0);
    textures[1].push_back("moyen_0"); width2HeightRatio[1].push_back(126.0 / 292.0);
    textures[1].push_back("moyen_1"); width2HeightRatio[1].push_back(101.0 / 201.0);
    textures[2].push_back("bas_0"); width2HeightRatio[2].push_back(75.0 / 220.0);
    textures[2].push_back("bas_1"); width2HeightRatio[2].push_back(117.0 / 231.0);
    textures[2].push_back("bas_2"); width2HeightRatio[2].push_back(67.0 / 128.0);
    textures[2].push_back("bas_3"); width2HeightRatio[2].push_back(91.0 / 181.0);
    textures[2].push_back("bas_4"); width2HeightRatio[2].push_back(79.0 / 205.0);
}

void BackgroundSystem::initCloud(Entity e, int group) {
    float ratio = 1.67;

    float width = cloudSize[group].random();
    TRANSFORM(e)->position.x = cloudStartX.random();
    TRANSFORM(e)->position.y = cloudY[group].random();
    TRANSFORM(e)->z = 0.8; //DL_Cloud;

    int idx = glm::round(glm::linearRand(0.f, (float)(textures[group].size()-1)));
    RENDERING(e)->texture = theRenderingSystem.loadTextureFile(textures[group][idx]);
    RENDERING(e)->color = Color(1,1,1, glm::linearRand(0.6f, 0.9f));
    RENDERING(e)->show = true;
    TRANSFORM(e)->size = glm::vec2(width, width / ratio);
    BACKGROUND(e)->visible = false;
    BACKGROUND(e)->speed = cloudSpeed[group].random();
}

void BackgroundSystem::DoUpdate(float dt) {
    FOR_EACH_ENTITY_COMPONENT(Background, e, bc)
        if(bc->enable) {
            TransformationComponent* tc = TRANSFORM(e);
            tc->position.x += (skySpeed + bc->speed) * dt;
            if (!theRenderingSystem.isVisible(e)) {
                if (bc->visible)
                    initCloud(e, bc->group);
            } else {
                bc->visible = true;
            }
        }
    }
}

void BackgroundSystem::showAll() {
    FOR_EACH_ENTITY(Background, e)
        RENDERING(e)->show =
            BACKGROUND(e)->visible =
            BACKGROUND(e)->enable = true;
    }
}

void BackgroundSystem::hideAll() {
    FOR_EACH_ENTITY(Background, e)
        RENDERING(e)->show =
            BACKGROUND(e)->visible = false;
    }
}
