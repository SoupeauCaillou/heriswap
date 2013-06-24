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
#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

struct RateItScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Entity textToRead, textToReadContainer;
    Entity boutonText[3], boutonContainer[3];

    RateItScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        //Creating text entities
        textToReadContainer = theEntityManager.CreateEntity("textToReadContainer",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("rateit/container"));

        textToRead = theEntityManager.CreateEntity("textToRead",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("rateit/text"));

        TEXT_RENDERING(textToRead)->text = game->gameThreadContext->localizeAPI->text("please_rate_it");
        // ??
        // TRANSFORM(textToRead)->position.x = TRANSFORM(textToReadContainer)->position.x = 0;

        std::stringstream a;
        for (int i = 0; i < 3; i++) {
            a.str("");
            a << "boutonText_" << i;
            boutonText[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("rateit/button_text"));

            a.str("");
            a << "boutonContainer_" << i;
            boutonContainer[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("rateit/button"));

            TRANSFORM(boutonText[i])->position.y =
                TRANSFORM(boutonContainer[i])->position.y = (float)PlacementHelper::GimpYToScreen(650 + i*183);
        }
        TEXT_RENDERING(boutonText[0])->text = game->gameThreadContext->localizeAPI->text("rate_now");
        TEXT_RENDERING(boutonText[1])->text = game->gameThreadContext->localizeAPI->text("rate_later");
        TEXT_RENDERING(boutonText[2])->text = game->gameThreadContext->localizeAPI->text("rate_never");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        RENDERING(game->menubg)->show =
            RENDERING(game->menufg)->show = true;

        RENDERING(textToReadContainer)->show = true;
        TEXT_RENDERING(textToRead)->show = true;
        for (int i=0; i<3; i++) {
            RENDERING(boutonContainer[i])->show = true;
            TEXT_RENDERING(boutonText[i])->show = true;
            BUTTON(boutonContainer[i])->enabled = true;
        }
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        //want to rate
        if (BUTTON(boutonContainer[0])->clicked) {
            SOUND(boutonContainer[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            game->gameThreadContext->communicationAPI->rateItNow();
            return Scene::ModeMenu;
        //will rate later
        } else if(BUTTON(boutonContainer[1])->clicked){
            SOUND(boutonContainer[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            game->gameThreadContext->communicationAPI->rateItLater();
            return Scene::ModeMenu;
        //won't never rate
        } else if(BUTTON(boutonContainer[2])->clicked){
            SOUND(boutonContainer[2])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            game->gameThreadContext->communicationAPI->rateItNever();
            return Scene::ModeMenu;
        }
        return Scene::RateIt;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        RENDERING(textToReadContainer)->show = false;
        TEXT_RENDERING(textToRead)->show = false;
        for (int i=0; i<3; i++) {
            TEXT_RENDERING(boutonText[i])->show = false;
            RENDERING(boutonContainer[i])->show = false;
            BUTTON(boutonContainer[i])->enabled = false;
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateRateItSceneHandler(HeriswapGame* game) {
        return new RateItScene(game);
    }
}
