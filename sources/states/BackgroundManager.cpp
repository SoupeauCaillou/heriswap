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
#include "BackgroundManager.h"

#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <base/PlacementHelper.h>
#include <base/Interval.h>
#include <base/EntityManager.h>

#include "systems/TransformationSystem.h"
#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"

#include "DepthLayer.h"

#define CAMERASPEED -1.f

struct BackgroundManager::Actor {
	Entity e;
	float speed;
	bool visible;
	int group;
};

struct BackgroundManager::AnimatedActor {
	int frames, ind;
	std::vector<std::string> anim;
	struct Actor actor;
};

BackgroundManager::BackgroundManager() {
	cloudY[0] = Interval<float>(PlacementHelper::GimpYToScreen(70.0), PlacementHelper::GimpYToScreen(240));
	cloudY[1] = Interval<float>(PlacementHelper::GimpYToScreen(390), PlacementHelper::GimpYToScreen(490));
	cloudY[2] = Interval<float>(PlacementHelper::GimpYToScreen(560), PlacementHelper::GimpYToScreen(650));

	cloudSize[0] = Interval<float>(PlacementHelper::GimpWidthToScreen(230), PlacementHelper::GimpWidthToScreen(380));
	cloudSize[1] = Interval<float>(PlacementHelper::GimpWidthToScreen(190), PlacementHelper::GimpWidthToScreen(230));
	cloudSize[2] = Interval<float>(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpWidthToScreen(180));

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


BackgroundManager::~BackgroundManager() {
	clouds.clear();
}

void BackgroundManager::Setup() {
	for(int j=0; j<3; j++) {
		Actor* c = new Actor();
		Entity e = theEntityManager.CreateEntity();
		theEntityManager.AddComponent(e, &theTransformationSystem);
		theEntityManager.AddComponent(e, &theADSRSystem);
		theEntityManager.AddComponent(e, &theRenderingSystem);
		c->e = e;
		clouds.push_back(initCloud(c, j));
	}
}

BackgroundManager::Actor* BackgroundManager::initCloud(Actor* c, int group) {
	float ratio = 1.67;

	float width = cloudSize[group].random();
	TRANSFORM(c->e)->position.x = cloudStartX.random();
	TRANSFORM(c->e)->position.y = cloudY[group].random();
	TRANSFORM(c->e)->z = DL_Cloud;

	int idx = glm::round(glm::linearRand(0.f, (float)(textures[group].size()-1)));
	RENDERING(c->e)->texture = theRenderingSystem.loadTextureFile(textures[group][idx]);
	RENDERING(c->e)->color = Color(1,1,1, glm::linearRand(0.6f, 0.9f));
	RENDERING(c->e)->show = true;
	TRANSFORM(c->e)->size = glm::vec2(width, width / ratio);
	c->visible = false;
	c->speed = cloudSpeed[group].random();
	c->group = group;
	
	return c;
}

void BackgroundManager::Enter() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
    for (int i=0; i<3; i++) {
        RENDERING(clouds[i]->e)->show = true;
    }
}

GameState BackgroundManager::Update(float dt __attribute__((unused))) {
	return Background;
}

void BackgroundManager::BackgroundUpdate(float dt) {
	for (std::vector<Actor*>::iterator it=clouds.begin(); it!=clouds.end(); ++it) {
		Actor* c = *it;

		TransformationComponent* tc = TRANSFORM(c->e);
		tc->position.x += (skySpeed + c->speed) * dt;
		if (!theRenderingSystem.isVisible(c->e)) {
			if (c->visible)
				initCloud(c, c->group);
		} else {
			c->visible = true;
		}
	}
}

void BackgroundManager::switchAnim(AnimatedActor* a)
{
	a->frames++;
	if (a->frames>=30/(glm::abs(a->actor.speed)+glm::abs(CAMERASPEED))) {
		RENDERING(a->actor.e)->texture = theRenderingSystem.loadTextureFile(a->anim[a->ind]);
		a->ind++;
		if (a->ind==(int)a->anim.size()) a->ind = 0;
		a->frames=0;
	}
}

void BackgroundManager::Exit() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
}
