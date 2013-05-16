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

#include "api/StorageAPI.h"

#include "base/EntityManager.h"
#include "base/ObjectSerializer.h"
#include "base/PlacementHelper.h"

#include "systems/AnimationSystem.h"
#include <systems/AutoDestroySystem.h>
#include <systems/ButtonSystem.h>
#include "systems/MorphingSystem.h"
#include <systems/PhysicsSystem.h>
#include <systems/RenderingSystem.h>
#include "systems/SoundSystem.h"
#include <systems/TextRenderingSystem.h>
#include <systems/TransformationSystem.h>

#include "util/ScoreStorageProxy.h"

#include <iomanip>
#include <sstream>
#include <vector>

struct MainMenuScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    GameMode choosenGameMode;
    Entity eStart[3];
    Entity menufg, menubg;
    Entity herisson;
    Entity modeTitleToReset;
    Entity bStart[3];
    Entity quitButton[2];

    float timeElapsed;

    MainMenuScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        const Color green("green");

        //Creating text entities
        for (int i=0; i<3; i++) {
            std::stringstream a;
            a.str("");
            a << "eStart_" << i;
            eStart[i] = theEntityManager.CreateEntity(a.str());
            ADD_COMPONENT(eStart[i], Transformation);
            ADD_COMPONENT(eStart[i], TextRendering);

            TRANSFORM(eStart[i])->z = DL_MainMenuUITxt;
            TEXT_RENDERING(eStart[i])->show = false;
            TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::CENTER;
            TEXT_RENDERING(eStart[i])->color = green;
            TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
            ADD_COMPONENT(eStart[i], Morphing);
            TypedMorphElement<float>* sizeMorph = new TypedMorphElement<float>(&TEXT_RENDERING(eStart[i])->charHeight, TEXT_RENDERING(eStart[i])->charHeight, PlacementHelper::GimpHeightToScreen(54));
            MORPHING(eStart[i])->elements.push_back(sizeMorph);
            MORPHING(eStart[i])->timing = 0.2;

            a.str("");
            a << "bStart_" << i;
            bStart[i] = theEntityManager.CreateEntity(a.str());
            ADD_COMPONENT(bStart[i], Transformation);
            TRANSFORM(bStart[i])->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708), 
                                                   (float)PlacementHelper::GimpHeightToScreen(147));
            TRANSFORM(bStart[i])->position.x = 0;
            TRANSFORM(bStart[i])->z = DL_MainMenuUIBg;
            ADD_COMPONENT(bStart[i], Rendering);
            RENDERING(bStart[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
            RENDERING(bStart[i])->color.a = 0.5;

            TRANSFORM(eStart[i])->parent = bStart[i];
            TRANSFORM(eStart[i])->position = glm::vec2(0.f);
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
            ADD_COMPONENT(bStart[i], Sound);
            ADD_COMPONENT(bStart[i], Button);
            BUTTON(bStart[i])->enabled = false;
        }

        for (int i=0; i<3; i++) {
            TEXT_RENDERING(eStart[i])->flags |= TextRenderingComponent::AdjustHeightToFillWidthBit;
            TRANSFORM(eStart[i])->size = TRANSFORM(bStart[i])->size * 0.9f;
        }

        menubg = theEntityManager.CreateEntity("menubg");
        ADD_COMPONENT(menubg, Transformation);
        TRANSFORM(menubg)->size = glm::vec2((float)PlacementHelper::ScreenWidth, 
                                            (float)PlacementHelper::GimpHeightToScreen(570));
        TransformationSystem::setPosition(TRANSFORM(menubg), 
                                          glm::vec2(0.f, 
                                                    (float)PlacementHelper::GimpYToScreen(542)), 
                                          TransformationSystem::N);
        TRANSFORM(menubg)->z = DL_MainMenuBg;
        ADD_COMPONENT(menubg, Rendering);
        RENDERING(menubg)->texture = theRenderingSystem.loadTextureFile("2emeplan");
        RENDERING(menubg)->show = false;
        RENDERING(menubg)->opaqueType = RenderingComponent::NON_OPAQUE;

        menufg = theEntityManager.CreateEntity("menufg");
        ADD_COMPONENT(menufg, Transformation);
        TRANSFORM(menufg)->size = glm::vec2((float)PlacementHelper::ScreenWidth, 
                                            (float)PlacementHelper::GimpHeightToScreen(570));
        TransformationSystem::setPosition(TRANSFORM(menufg), 
                                          glm::vec2(0.f, PlacementHelper::GimpYToScreen(1280)), 
                                          TransformationSystem::S);
        TRANSFORM(menufg)->z = DL_MainMenuFg;
        ADD_COMPONENT(menufg, Rendering);
        RENDERING(menufg)->texture = theRenderingSystem.loadTextureFile("1erplan");
        RENDERING(menufg)->show = false;
        RENDERING(menufg)->opaqueType = RenderingComponent::NON_OPAQUE;
        
        herisson = theEntityManager.CreateEntity("herisson_menu");
        ADD_COMPONENT(herisson, Transformation);
        TRANSFORM(herisson)->z = DL_MainMenuHerisson;
        
        TRANSFORM(herisson)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(310), (float)PlacementHelper::GimpHeightToScreen(253)) * glm::linearRand(.3f, 1.f);
        TransformationSystem::setPosition(TRANSFORM(herisson), 
                                          glm::vec2((float)(PlacementHelper::GimpXToScreen(-glm::round(glm::linearRand(0.f, 299.f))) - TRANSFORM(herisson)->size.x), 
                                                    (float)PlacementHelper::GimpYToScreen(glm::round(glm::linearRand(830.f,1149.f)))), 
                                          TransformationSystem::SW);
        ADD_COMPONENT(herisson, Rendering);
        ADD_COMPONENT(herisson, Animation);
        ANIMATION(herisson)->playbackSpeed = 4.1f;
        std::stringstream a;
        a << "herisson_" << glm::round(glm::linearRand(1.f, 8.f));
        ANIMATION(herisson)->name = a.str();

        quitButton[0] = theEntityManager.CreateEntity("quitButton_0");
        ADD_COMPONENT(quitButton[0], Transformation);
        TRANSFORM(quitButton[0])->z = DL_MainMenuUITxt;
        TRANSFORM(quitButton[0])->position = glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(1215));
        ADD_COMPONENT(quitButton[0], TextRendering);
        TEXT_RENDERING(quitButton[0])->text = " " + game->gameThreadContext->localizeAPI->text("quit") + " ";
        TEXT_RENDERING(quitButton[0])->show = false;
        TEXT_RENDERING(quitButton[0])->positioning = TextRenderingComponent::CENTER;
        TEXT_RENDERING(quitButton[0])->color = green;
        TEXT_RENDERING(quitButton[0])->charHeight = PlacementHelper::GimpHeightToScreen(60);

        quitButton[1] = theEntityManager.CreateEntity("quitButton_1");
        ADD_COMPONENT(quitButton[1], Transformation);
        float hhh = PlacementHelper::GimpHeightToScreen(95);
        float www = hhh / 0.209;
        TRANSFORM(quitButton[1])->size = glm::vec2(www, www * 0.209);
        TRANSFORM(quitButton[1])->position = glm::vec2(0, PlacementHelper::GimpYToScreen(1215));
        TRANSFORM(quitButton[1])->z = DL_MainMenuUIBg;
        ADD_COMPONENT(quitButton[1], Rendering);
        RENDERING(quitButton[1])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
        RENDERING(quitButton[1])->color.a = 0.5;
        ADD_COMPONENT(quitButton[1], Button);

        modeTitleToReset = 0;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum pState) override {
        switch (pState) {
            case Scene::Pause:
                LOGI("aborted. going to main menu");
                RENDERING(game->datas->soundButton)->show = true;
                // game->datas->state2Manager[game->datas->stateBeforePause]->Exit();
                // game->datas->mode2Manager[game->datas->mode]->Exit();
                // static_cast<ModeMenuStateManager*> (datas->state2Manager[ModeMenu])->gameOverState = ModeMenuStateManager::NoGame;
                // stopInGameMusics();
                break;
            case Scene::ModeMenu:
                RENDERING(game->datas->soundButton)->show = true;
                // game->datas->state2Manager[oldState]->LateExit();
                break;
        }
    
        // preload sound effect
        theSoundSystem.loadSoundFile("audio/son_menu.ogg");

        for (int i=0; i<3; ++i) {
            TEXT_RENDERING(eStart[i])->show = true;
            RENDERING(bStart[i])->show = true;
            BUTTON(bStart[i])->enabled = true;
        }

        RENDERING(menufg)->show = 
            RENDERING(menubg)->show = 
            RENDERING(herisson)->show = true;
        
        TEXT_RENDERING(quitButton[0])->show =
            RENDERING(quitButton[1])->show = true;
        BUTTON(quitButton[1])->enabled = true;    

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
        if (TRANSFORM(herisson)->position.x < PlacementHelper::GimpXToScreen(800)+TRANSFORM(herisson)->size.x) {
            TRANSFORM(herisson)->position.x += ANIMATION(herisson)->playbackSpeed/8. * dt;
        } else {
            std::stringstream a;
            a << "herisson_" << glm::round(glm::linearRand(1.f, 8.f));
            ANIMATION(herisson)->name = a.str();
            ANIMATION(herisson)->playbackSpeed = glm::linearRand(2.0f, 4.0f);

            TRANSFORM(herisson)->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(310), 
                                                  (float)PlacementHelper::GimpHeightToScreen(253)) * glm::linearRand(.3f, 1.f); //size
            TransformationSystem::setPosition(TRANSFORM(herisson), 
                                              glm::vec2((float)(PlacementHelper::GimpXToScreen(-glm::round(glm::linearRand(0.f, 299.f)))-TRANSFORM(herisson)->size.x), 
                                                        (float)PlacementHelper::GimpYToScreen(glm::round(glm::linearRand(830.f, 1149.f)))), 
                                              TransformationSystem::SW);//offset
        }

        if (!modeTitleToReset || (modeTitleToReset && !MORPHING(modeTitleToReset)->active)) {
            if (BUTTON(bStart[0])->clicked) {
                choosenGameMode = Normal;
                SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
                return Scene::ModeMenu;
            }
            if(BUTTON(bStart[1])->clicked){
                choosenGameMode = TilesAttack;
                SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
                return Scene::ModeMenu;
            }
            if(BUTTON(bStart[2])->clicked){
                choosenGameMode = Go100Seconds;
                SOUND(bStart[2])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
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

    void onExit(Scene::Enum) override {
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

        ANIMATION(herisson)->playbackSpeed = 4.5f;

        TEXT_RENDERING(quitButton[0])->show = false;
        RENDERING(quitButton[1])->show = false;
        BUTTON(quitButton[1])->enabled = false;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateMainMenuSceneHandler(HeriswapGame* game) {
        return new MainMenuScene(game);
    }
}