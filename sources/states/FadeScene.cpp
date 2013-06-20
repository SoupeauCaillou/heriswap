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

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "DepthLayer.h"

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>

#include "systems/ADSRSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

struct FadeScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;
    FadingType::Enum type;
    float duration;
    Scene::Enum nextState;

    // State variables
    Entity eFading;
    float timeout, accum;

    FadeScene(HeriswapGame* game, FadingType::Enum pType, float pDuration, Scene::Enum pNextState):
    StateHandler<Scene::Enum>(), type(pType), duration(pDuration), nextState(pNextState), timeout(0) {
        this->game = game;
    }

    void setup() {
        eFading = theEntityManager.CreateEntity("eFading",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("eFading"));
        ADSR(eFading)->attackTiming = duration;
    }

    static void updateColor(Entity eFading, FadingType::Enum type) {
        float value = ADSR(eFading)->value;
        switch (type) {
            case FadingType::FadeIn:
                RENDERING(eFading)->color.a = 1 - value;
                break;
            case FadingType::FadeOut:
                RENDERING(eFading)->color.a = value;
                break;
        }
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
        LOGW("Fade type: '" << type << "'");
        RENDERING(eFading)->show = true;
        //update duration (can be changed)
        ADSR(eFading)->attackTiming = duration;
        ADSR(eFading)->active = true;

        accum = 0;
        updateColor(eFading, type);
    }

    // Return false, until fading (in or out) is finished
    bool updatePreEnter(Scene::Enum , float dt) override {
        updateColor(eFading, type);
        if (type == FadingType::FadeIn)
            return true;
        return internalUpdate(dt);
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float ) override {
        return nextState;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    bool updatePreExit(Scene::Enum , float dt) override {
        updateColor(eFading, type);
        if (type == FadingType::FadeOut)
            return true;
        return internalUpdate(dt);
    }

    void onExit(Scene::Enum) override {
        RENDERING(eFading)->show = false;
        ADSR(eFading)->active = false;
    }

    bool internalUpdate(float dt) {
        if (theTouchInputManager.isTouched() && !theTouchInputManager.wasTouched()) {
            return true;
        }
        const auto* adsr = ADSR(eFading);
        if (adsr->value == adsr->sustainValue) {
            accum += dt;
            if (accum >= timeout) {
                return true;
            }
        }
        return false;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateFadeSceneHandler(HeriswapGame* game, FadingType::Enum type, float duration, Scene::Enum nextState) {
        return new FadeScene(game, type, duration, nextState);
    }
}
