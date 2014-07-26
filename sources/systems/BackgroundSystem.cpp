/*
    This file is part of Heriswap.

    @author Soupe au Caillou - Jordane Pelloux-Prayer
    @author Soupe au Caillou - Gautier Pelloux-Prayer
    @author Soupe au Caillou - Pierre-Eric Pelloux-Prayer

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


#include "BackgroundSystem.h"

#include "DepthLayer.h"

#include "base/PlacementHelper.h"
#include "base/Interval.h"

#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"

#include "util/Random.h"

#define CAMERASPEED -1.f

INSTANCE_IMPL(BackgroundSystem);

BackgroundSystem::BackgroundSystem() : ComponentSystemImpl<BackgroundComponent>(HASH("Background",0x5e4973c4)) {
    skySpeed = -0.3f;

    cloudStartX = Interval<float>(8.0f, 15.0f);

    cloudY[0] = Interval<float>(PlacementHelper::GimpYToScreen(70), PlacementHelper::GimpYToScreen(240));
    cloudY[1] = Interval<float>(PlacementHelper::GimpYToScreen(390), PlacementHelper::GimpYToScreen(490));
    cloudY[2] = Interval<float>(PlacementHelper::GimpYToScreen(560), PlacementHelper::GimpYToScreen(650));

    cloudSize[0] = Interval<float>(PlacementHelper::GimpWidthToScreen(230), PlacementHelper::GimpWidthToScreen(380));
    cloudSize[1] = Interval<float>(PlacementHelper::GimpWidthToScreen(190), PlacementHelper::GimpWidthToScreen(230));
    cloudSize[2] = Interval<float>(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpWidthToScreen(180));

    cloudSpeed[0] = Interval<float>(-0.4f, -0.25f);
    cloudSpeed[1] = Interval<float>(-0.25f, -0.13f);
    cloudSpeed[2] = Interval<float>(-0.1f, -0.03f);

    textures[0].push_back("haut_0"); width2HeightRatio[0].push_back(138.0f / 260.0f);
    textures[0].push_back("haut_1"); width2HeightRatio[0].push_back(197.0f / 336.0f);
    textures[0].push_back("haut_2"); width2HeightRatio[0].push_back(208.0f / 413.0f);
    textures[1].push_back("moyen_0"); width2HeightRatio[1].push_back(126.0f / 292.0f);
    textures[1].push_back("moyen_1"); width2HeightRatio[1].push_back(101.0f / 201.0f);
    textures[2].push_back("bas_0"); width2HeightRatio[2].push_back(75.0f / 220.0f);
    textures[2].push_back("bas_1"); width2HeightRatio[2].push_back(117.0f / 231.0f);
    textures[2].push_back("bas_2"); width2HeightRatio[2].push_back(67.0f / 128.0f);
    textures[2].push_back("bas_3"); width2HeightRatio[2].push_back(91.0f / 181.0f);
    textures[2].push_back("bas_4"); width2HeightRatio[2].push_back(79.0f / 205.0f);
}

void BackgroundSystem::initCloud(Entity e, int group) {
    float ratio = 1.67f;

    LOGF_IF(group < 0 || group > 2, "Invalid group value: " << group);
    float width = cloudSize[group].random();
    TRANSFORM(e)->position.x = cloudStartX.random();
    TRANSFORM(e)->position.y = cloudY[group].random();
    TRANSFORM(e)->z = DL_Cloud;

    int idx = Random::Int(0, textures[group].size()-1);
    RENDERING(e)->texture = theRenderingSystem.loadTextureFile(textures[group][idx].c_str());
    RENDERING(e)->color = Color(1,1,1, Random::Float(0.6f, 0.9f));
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
    END_FOR_EACH()
}

void BackgroundSystem::showAll() {
    FOR_EACH_ENTITY_COMPONENT(Background, e, bc)
        RENDERING(e)->show =
            bc->visible =
            bc->enable = true;
    END_FOR_EACH()
}

void BackgroundSystem::hideAll() {
    FOR_EACH_ENTITY_COMPONENT(Background, e, bc)
        RENDERING(e)->show =
            bc->visible = false;
    END_FOR_EACH()
}
