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

#include "base/StateMachine.h"

#include "Scenes.h"

#include "HeriswapGame.h"

#include "DepthLayer.h"

#include "modes/GameModeManager.h"

#include "base/EntityManager.h"
#include "base/ObjectSerializer.h"
#include "base/PlacementHelper.h"
#include "base/TimeUtil.h"
#include "Game_Private.h"

#include "systems/ButtonSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TransformationSystem.h"



struct AdsScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Entity eAds;
    AdAPI* adAPI;
    int gameb4Ads;
    float lastAdTime;
    float stateActiveDuration;

    AdsScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        eAds = theEntityManager.CreateEntity("eAds",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("eAds"));
        lastAdTime = -30;

        //at each start of the game, we set the number of party to '2' before first ad
        game->gameThreadContext->storageAPI->setOption("gameB4Ads", "2", "2");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
        game->datas->faderHelper.start(Fading::Out, 0.5);
    }

    bool updatePreEnter(Scene::Enum, float dt) override {
        return game->datas->faderHelper.update(dt);
    }

    void onEnter(Scene::Enum) override {
        stateActiveDuration = 0;
        gameb4Ads = ObjectSerializer<int>::string2object(game->gameThreadContext->storageAPI->getOption("gameB4Ads"));
        LOGI("GameB4Bds: " <<  gameb4Ads);
        if (gameb4Ads > 3) {
            gameb4Ads = 3;
        }

        float timeSinceLAstAd = TimeUtil::GetTime() - lastAdTime;

        // postpone ad if previous ad was shown less than 30sec ago
        if (gameb4Ads <= 0 && timeSinceLAstAd < 30) {
            gameb4Ads = 1;
        }

        if (gameb4Ads==0 || timeSinceLAstAd > 150) {
            if (game->gameThreadContext->adAPI->showAd()) {
                BUTTON(eAds)->enabled = true;
                gameb4Ads = 0;
                lastAdTime = TimeUtil::GetTime();
            } else {
                gameb4Ads = 1;
            }
        }
        RENDERING(eAds)->show = true;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float dt) override {
        stateActiveDuration += dt;
        if (gameb4Ads>0 || BUTTON(eAds)->clicked || game->gameThreadContext->adAPI->done()) {
            return Scene::CountDown;
        }
        return Scene::Ads;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
        BUTTON(eAds)->enabled = false;
        if (gameb4Ads==0)
            gameb4Ads=3;
        gameb4Ads--;
        game->gameThreadContext->storageAPI->setOption("gameB4Ads", ObjectSerializer<int>::object2string(gameb4Ads), "2");
    }

    void onExit(Scene::Enum) override {
        LOGW("ici on cache pour de vrai");
        RENDERING(eAds)->show = false;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateAdsSceneHandler(HeriswapGame* game) {
        return new AdsScene(game);
    }
}
