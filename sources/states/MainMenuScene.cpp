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
#include "Game_Private.h"
#include "DepthLayer.h"

#include "modes/NormalModeManager.h"
#include "modes/Go100SecondsModeManager.h"
#include "modes/TilesAttackModeManager.h"

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
#include "systems/TextSystem.h"
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
    Entity menufg, menubg;

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
            eStart[i] = theEntityManager.CreateEntityFromTemplate("mainmenu/"+ a.str());

            TypedMorphElement<float>* sizeMorph = new TypedMorphElement<float>(
                &TEXT(eStart[i])->charHeight,
                TEXT(eStart[i])->charHeight,
                PlacementHelper::GimpHeightToScreen(54));
            MORPHING(eStart[i])->elements.push_back(sizeMorph);
            TypedMorphElement<float>* alignMorph = new TypedMorphElement<float>(
                &TEXT(eStart[i])->positioning, 0, 1);
            MORPHING(eStart[i])->elements.push_back(alignMorph);

            a.str("");
            a << "bStart_" << i;
            bStart[i] = theEntityManager.CreateEntityFromTemplate("mainmenu/" + a.str());
        }
        TEXT(eStart[0])->text = game->gameThreadContext->localizeAPI->text("mode_1");
        TEXT(eStart[1])->text = game->gameThreadContext->localizeAPI->text("mode_2");
        TEXT(eStart[2])->text = game->gameThreadContext->localizeAPI->text("mode_3");

        //Containers properties
        for (int i=0; i<3; i++) {
            TEXT(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(54);
            TEXT(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
            glm::vec2 target = glm::vec2((float)(PlacementHelper::GimpXToScreen(708)) ,
                                         (float)PlacementHelper::GimpYToScreen(147));
            TypedMorphElement<glm::vec2>* posMorph = new TypedMorphElement<glm::vec2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, target);
            MORPHING(eStart[i])->elements.push_back(posMorph);
        }

        for (int i=0; i<3; i++) {
            TEXT(eStart[i])->flags |= TextComponent::AdjustHeightToFillWidthBit;
            TRANSFORM(eStart[i])->size = TRANSFORM(bStart[i])->size * 0.9f;
        }

        menubg = theEntityManager.CreateEntityFromTemplate("mainmenu/background");
        menufg = theEntityManager.CreateEntityFromTemplate("mainmenu/foreground");

        game->herisson = theEntityManager.CreateEntityFromTemplate("mainmenu/herisson");

        TRANSFORM(game->herisson)->size = randomHerissonSize();
        TRANSFORM(game->herisson)->position = AnchorSystem::adjustPositionWithCardinal(randomHerissionStart(),
                TRANSFORM(game->herisson)->size, Cardinal::SE);

        a.str("");
        a << "herisson_" << glm::round(glm::linearRand(1.f, 8.f));
        ANIMATION(game->herisson)->name = a.str();

        modeTitleToReset = 0;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum pState) override {
        if (pState == Scene::Logo) {
            // setup fadein
            for (int i=0; i<3; ++i) {
                game->datas->faderHelper.registerFadingInEntity(eStart[i]);
                game->datas->faderHelper.registerFadingInEntity(bStart[i]);
            }
            game->datas->faderHelper.registerFadingInEntity(menufg);
            game->datas->faderHelper.registerFadingInEntity(menubg);
            game->datas->faderHelper.registerFadingInEntity(game->datas->gamecenterAPIHelper.signButton);
            game->datas->faderHelper.registerFadingInEntity(game->datas->gamecenterAPIHelper.achievementsButton);
            game->datas->faderHelper.registerFadingInEntity(game->datas->gamecenterAPIHelper.leaderboardsButton);
            game->datas->faderHelper.registerFadingInEntity(game->datas->soundButton);
            game->datas->faderHelper.registerFadingInEntity(game->herisson);
            game->datas->faderHelper.registerFadingInEntity(game->datas->sky);

            game->datas->faderHelper.registerFadingInCallback([this] () -> void {
                theBackgroundSystem.showAll();
                SCROLLING(game->datas->sky)->show = true;
            });
        } else {

            if (pState == Scene::Pause) {
                LOGI("aborted. going to main menu");
                if (game->datas->mode == Normal) {
                    static_cast<NormalGameModeManager*> (game->datas->mode2Manager[Normal])->Exit();
                } else if (game->datas->mode == TilesAttack) {
                    static_cast<TilesAttackGameModeManager*> (game->datas->mode2Manager[TilesAttack])->Exit();
                } else {
                    static_cast<Go100SecondsGameModeManager*> (game->datas->mode2Manager[Go100Seconds])->Exit();
                }
            }

            // preload sound effect
            theSoundSystem.loadSoundFile("audio/son_menu.ogg");

            for (int i=0; i<3; ++i) {
                TEXT(eStart[i])->show = true;
                RENDERING(bStart[i])->show = true;
                BUTTON(bStart[i])->enabled = true;
            }

            RENDERING(menufg)->show =
                RENDERING(menubg)->show =
                RENDERING(game->datas->soundButton)->show =
                RENDERING(game->herisson)->show = true;

            BUTTON(game->datas->soundButton)->enabled = true;


            SCROLLING(game->datas->sky)->show = true;

            theBackgroundSystem.showAll();

            if (modeTitleToReset) {
                theMorphingSystem.reverse(MORPHING(modeTitleToReset));
                MORPHING(modeTitleToReset)->activationTime = 0;
                MORPHING(modeTitleToReset)->active = true;
            }
        }
        choosenGameMode = Normal;
    }

    bool updatePreEnter(Scene::Enum from, float dt) {
        if (from == Scene::Logo)
            return game->datas->faderHelper.update(dt);
        else
            return true;
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
            TRANSFORM(game->herisson)->size = randomHerissonSize();
            TRANSFORM(game->herisson)->position = AnchorSystem::adjustPositionWithCardinal(randomHerissionStart(),
                TRANSFORM(game->herisson)->size, Cardinal::SE);
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
        }
        return Scene::MainMenu;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum to) override {
        ScoreStorageProxy ssp;
        ssp.setValue("time", ObjectSerializer<float>::object2string(timeElapsed), true);
        timeElapsed = 0.f;
        game->gameThreadContext->storageAPI->saveEntries((IStorageProxy*)&ssp);

        if (to == Scene::ModeMenu) {
            game->datas->mode = choosenGameMode;
        }
        for (int i = 0; i < 3; i++) {
            if (i != choosenGameMode)
                TEXT(eStart[i])->show = false;
            RENDERING(bStart[i])->show = false;
            BUTTON(bStart[i])->enabled = false;
        }

        if (modeTitleToReset) {
            theMorphingSystem.reverse(MORPHING(modeTitleToReset));
        }

        MORPHING(eStart[choosenGameMode])->active = true;
        modeTitleToReset = eStart[choosenGameMode];

        ANIMATION(game->herisson)->playbackSpeed = 4.5f;
    }

    bool updatePreExit(Scene::Enum, float) {
        return !MORPHING(eStart[choosenGameMode])->active;
    }

    void onExit(Scene::Enum) override {
        TEXT(eStart[choosenGameMode])->show = false;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateMainMenuSceneHandler(HeriswapGame* game) {
        return new MainMenuScene(game);
    }
}
