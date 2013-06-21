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
#include "Game_Private.h"
#include "DepthLayer.h"

#include "systems/BackgroundSystem.h"

#include "api/StorageAPI.h"

#include "base/EntityManager.h"
#include "base/ObjectSerializer.h"
#include "base/PlacementHelper.h"

#include "systems/AnimationSystem.h"
#include "systems/AutoDestroySystem.h"
#include "systems/ButtonSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScrollingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/AnchorSystem.h"

#include "util/ScoreStorageProxy.h"

#include <iomanip>
#include <sstream>
#include <vector>

static glm::vec2 randomHerissonSize() {
    return
        glm::vec2((float)PlacementHelper::GimpWidthToScreen(310), (float)PlacementHelper::GimpHeightToScreen(253)) * glm::linearRand(.3f, 1.f);
}

static glm::vec2 randomHerissionStart() {
    return glm::vec2(
        (float)PlacementHelper::GimpXToScreen(-glm::round(glm::linearRand(0.f, 299.f))),
        (float)PlacementHelper::GimpYToScreen(glm::round(glm::linearRand(830.f, 1149.f))));
}

struct MainMenuScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    GameMode choosenGameMode;
    Entity eStart[3];
    Entity modeTitleToReset;
    Entity bStart[3];
    Entity quitButton[2];

    float timeElapsed;

    MainMenuScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        const Color green("green");

        std::stringstream a;
        //Creating text entities
        for (int i=0; i<3; i++) {
            a.str("");
            a << "eStart_" << i;
            eStart[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("mainmenu/"+ a.str()));

            TypedMorphElement<float>* sizeMorph = new TypedMorphElement<float>(&TEXT_RENDERING(eStart[i])->charHeight, TEXT_RENDERING(eStart[i])->charHeight, PlacementHelper::GimpHeightToScreen(54));
            MORPHING(eStart[i])->elements.push_back(sizeMorph);

            a.str("");
            a << "bStart_" << i;
            bStart[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("mainmenu/" + a.str()));

            // TRANSFORM(bStart[i])->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708),
            //                                        (float)PlacementHelper::GimpHeightToScreen(147));
            // TRANSFORM(bStart[i])->position.x = 0;
        }
        TEXT_RENDERING(eStart[0])->text = game->gameThreadContext->localizeAPI->text("mode_1");
        TEXT_RENDERING(eStart[1])->text = game->gameThreadContext->localizeAPI->text("mode_2");
        TEXT_RENDERING(eStart[2])->text = game->gameThreadContext->localizeAPI->text("mode_3");

        //Containers properties
        for (int i=0; i<3; i++) {
            TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(54);
            float w = theTextRenderingSystem.computeTextRenderingComponentWidth(TEXT_RENDERING(eStart[i]));
            TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
            glm::vec2 target = glm::vec2((float)(PlacementHelper::GimpXToScreen(700) - w) ,
                                         (float)PlacementHelper::GimpYToScreen(100));
            TypedMorphElement<glm::vec2>* posMorph = new TypedMorphElement<glm::vec2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, target);
            MORPHING(eStart[i])->elements.push_back(posMorph);
        }

        for (int i=0; i<3; i++) {
            TEXT_RENDERING(eStart[i])->flags |= TextRenderingComponent::AdjustHeightToFillWidthBit;
            TRANSFORM(eStart[i])->size = TRANSFORM(bStart[i])->size * 0.9f;
        }

        game->menubg = theEntityManager.CreateEntity("menubg",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("mainmenu/background"));

        game->menufg = theEntityManager.CreateEntity("menufg",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("mainmenu/foreground"));

        game->herisson = theEntityManager.CreateEntity("herisson_menu",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("mainmenu/herisson"));

        // glm::vec2((float)PlacementHelper::GimpWidthToScreen(310), (float)PlacementHelper::GimpHeightToScreen(253)) * glm::linearRand(.3f, 1.f);
        TRANSFORM(game->herisson)->size = randomHerissonSize();
        TRANSFORM(game->herisson)->position = AnchorSystem::adjustPositionWithCardinal(randomHerissionStart(),
                TRANSFORM(game->herisson)->size, Cardinal::SE);

        a.str("");
        a << "herisson_" << glm::round(glm::linearRand(1.f, 8.f));
        ANIMATION(game->herisson)->name = a.str();

        quitButton[0] = theEntityManager.CreateEntity("quitButton_0",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("mainmenu/quit_text"));


        //TRANSFORM(quitButton[0])->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(1215));
        TEXT_RENDERING(quitButton[0])->text = " " + game->gameThreadContext->localizeAPI->text("quit") + " ";
        // TEXT_RENDERING(quitButton[0])->charHeight = PlacementHelper::GimpHeightToScreen(60);

        quitButton[1] = theEntityManager.CreateEntity("quitButton_1",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("mainmenu/quit_button"));

        // float hhh = PlacementHelper::GimpHeightToScreen(95);
        // float www = hhh / 0.209;
        // TRANSFORM(quitButton[1])->size = glm::vec2(www, www * 0.209);
        // TRANSFORM(quitButton[1])->position = glm::vec2(0, PlacementHelper::GimpYToScreen(1215));
        modeTitleToReset = 0;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum pState) override {
        switch (pState) {
            case Scene::Pause:
                LOGI("aborted. going to main menu");
                // RENDERING(game->datas->soundButton)->show = true;
                // game->datas->state2Manager[game->datas->stateBeforePause]->Exit();
                // game->datas->mode2Manager[game->datas->mode]->Exit();
                // static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->gameOverState = ModeMenuStateManager::NoGame;
                // stopInGameMusics();
                break;
            case Scene::ModeMenu:
                // RENDERING(game->datas->soundButton)->show = true;
                // game->datas->state2Manager[oldState]->LateExit();
                break;
            default:
                break;
        }

        // preload sound effect
        theSoundSystem.loadSoundFile("audio/son_menu.ogg");

        for (int i=0; i<3; ++i) {
            TEXT_RENDERING(eStart[i])->show = true;
            RENDERING(bStart[i])->show = true;
            BUTTON(bStart[i])->enabled = true;
        }

        RENDERING(game->menufg)->show =
            RENDERING(game->menubg)->show =
            RENDERING(game->datas->socialGamNet)->show =
            RENDERING(game->datas->soundButton)->show =
            RENDERING(game->herisson)->show = true;

        TEXT_RENDERING(quitButton[0])->show =
            RENDERING(quitButton[1])->show = true;

        BUTTON(game->datas->socialGamNet)->enabled =
            BUTTON(game->datas->soundButton)->enabled =
            BUTTON(quitButton[1])->enabled = true;

        SCROLLING(game->datas->sky)->show = true;

        theBackgroundSystem.showAll();

        if (modeTitleToReset) {
            theMorphingSystem.reverse(MORPHING(modeTitleToReset));
            MORPHING(modeTitleToReset)->activationTime = 0;
            MORPHING(modeTitleToReset)->active = true;
        }

        choosenGameMode = Normal;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float dt) override {
        if (TRANSFORM(game->herisson)->position.x < PlacementHelper::GimpXToScreen(800)+TRANSFORM(game->herisson)->size.x) {
            TRANSFORM(game->herisson)->position.x += ANIMATION(game->herisson)->playbackSpeed/8. * dt;
        } else {
            std::stringstream a;
            a << "herisson_" << glm::round(glm::linearRand(1.f, 8.f));
            ANIMATION(game->herisson)->name = a.str();
            ANIMATION(game->herisson)->playbackSpeed = glm::linearRand(2.0f, 4.0f);
            auto textureSize = theRenderingSystem.getTextureSize("herisson_1_5");
            float r = glm::linearRand(.3f, 1.f);
            TRANSFORM(game->herisson)->size = randomHerissonSize();
            TRANSFORM(game->herisson)->position = AnchorSystem::adjustPositionWithCardinal(randomHerissionStart(),
                TRANSFORM(game->herisson)->size, Cardinal::SE);
        }
        game->title = 0;
        if (!modeTitleToReset || (modeTitleToReset && !MORPHING(modeTitleToReset)->active)) {
            if (BUTTON(bStart[0])->clicked) {
                choosenGameMode = Normal;
                SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
                game->title = eStart[0];
                return Scene::ModeMenu;
            }
            if(BUTTON(bStart[1])->clicked){
                choosenGameMode = TilesAttack;

                SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
                game->title = eStart[1];
                return Scene::ModeMenu;
            }
            if(BUTTON(bStart[2])->clicked){
                choosenGameMode = Go100Seconds;
                SOUND(bStart[2])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
                game->title = eStart[2];
                return Scene::ModeMenu;
            }
            if (BUTTON(quitButton[1])->clicked) {
                return Scene::ExitState;
            }
        }
        return Scene::MainMenu;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
        ScoreStorageProxy ssp;
        ssp.setValue("time", ObjectSerializer<float>::object2string(timeElapsed), true);
        timeElapsed = 0.f;
        game->gameThreadContext->storageAPI->saveEntries((IStorageProxy*)&ssp);
    }

    void onExit(Scene::Enum to) override {
        if (to == Scene::ModeMenu) {
            game->datas->mode = choosenGameMode;
        }

        for (int i = 0; i < 3; i++) {
            if (i != choosenGameMode)
                TEXT_RENDERING(eStart[i])->show = false;
            RENDERING(bStart[i])->show = false;
            BUTTON(bStart[i])->enabled = false;
        }

        if (modeTitleToReset) {
            theMorphingSystem.reverse(MORPHING(modeTitleToReset));
        }

        MORPHING(eStart[choosenGameMode])->active = true;
        modeTitleToReset = eStart[choosenGameMode];

        ANIMATION(game->herisson)->playbackSpeed = 4.5f;

        TEXT_RENDERING(quitButton[0])->show = false;
        RENDERING(game->datas->socialGamNet)->show =
            // RENDERING(game->datas->soundButton)->show =
        RENDERING(quitButton[1])->show = false;

        // SCROLLING(game->datas->sky)->show = false;

        BUTTON(game->datas->socialGamNet)->enabled =
            // BUTTON(game->datas->soundButton)->enabled =
        BUTTON(quitButton[1])->enabled = false;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateMainMenuSceneHandler(HeriswapGame* game) {
        return new MainMenuScene(game);
    }
}
