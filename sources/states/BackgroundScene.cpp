/*
This file is part of RecursiveRunner.

@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
@author Soupe au Caillou - Gautier Pelloux-Prayer

RecursiveRunner is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

RecursiveRunner is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RecursiveRunner.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "base/StateMachine.h"

#include "Scenes.h"

#include "HeriswapGame.h"

#include "DepthLayer.h"

#include <base/PlacementHelper.h>
#include <base/Interval.h>
#include <base/EntityManager.h>

#include "systems/ADSRSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#define CAMERASPEED -1.f

struct BackgroundScene : public StateHandler<Scene::Enum> {
	HeriswapGame* game;

	// State variables
	float skySpeed;
	//clouds
	Interval<float> cloudStartX;
	Interval<float> cloudY[3];
	Interval<float> cloudSize[3];
	Interval<float> cloudSpeed[3];
	std::vector<std::string> textures[3];
    std::vector<float> width2HeightRatio[3];
    std::vector<Entity> clouds;

	BackgroundScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
	    this->game = game;

	    cloudY[0] = Interval<float>(PlacementHelper::GimpYToScreen(70), PlacementHelper::GimpYToScreen(240));
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

	void setup() {
		for(int j=0; j<3; j++) {
			// Actor* c = new Actor();
			Entity e = theEntityManager.CreateEntity("cloud");
			ADD_COMPONENT(e, Transformation);
			ADD_COMPONENT(e, Rendering);
			ADD_COMPONENT(e, ADSR);
			ADD_COMPONENT(e, Physics);
			// c->e = e;
			clouds.push_back(initCloud(e, j));
		}
	}

	///----------------------------------------------------------------------------//
	///------------------- PRIVATE FUNCTIONS --------------------------------------//
	///----------------------------------------------------------------------------//
	Entity initCloud(Entity cloud, int group) {
		float ratio = 1.67;

		float width = cloudSize[group].random();
		TRANSFORM(cloud)->position.x = cloudStartX.random();
		TRANSFORM(cloud)->position.y = cloudY[group].random();
		TRANSFORM(cloud)->z = DL_Cloud;

		int idx = glm::round(glm::linearRand(0.f, (float)(textures[group].size()-1)));
		RENDERING(cloud)->texture = theRenderingSystem.loadTextureFile(textures[group][idx]);
		RENDERING(cloud)->color = Color(1,1,1, glm::linearRand(0.6f, 0.9f));
		RENDERING(cloud)->show = true;
		TRANSFORM(cloud)->size = glm::vec2(width, width / ratio);
		
		PHYSICS(cloud)->linearVelocity = glm::vec2(cloudSpeed[group].random(), 0.f);
		
		return cloud;
	}

	///----------------------------------------------------------------------------//
	///--------------------- ENTER SECTION ----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreEnter(Scene::Enum) override {
	}

	void onEnter(Scene::Enum) override {
	}

	///----------------------------------------------------------------------------//
	///--------------------- UPDATE SECTION ---------------------------------------//
	///----------------------------------------------------------------------------//
	Scene::Enum update(float) override {

		return Scene::Background;
	}

	///----------------------------------------------------------------------------//
	///--------------------- EXIT SECTION -----------------------------------------//
	///----------------------------------------------------------------------------//
	void onPreExit(Scene::Enum) override {
	}

	void onExit(Scene::Enum) override {
	}
};

namespace Scene {
	StateHandler<Scene::Enum>* CreateBackgroundSceneHandler(HeriswapGame* game) {
    	return new BackgroundScene(game);
	}
}