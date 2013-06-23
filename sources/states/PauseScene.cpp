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

#include "base/EntityManager.h"
#include "base/PlacementHelper.h"

#include "systems/ButtonSystem.h"
#include "systems/MusicSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

struct PauseScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    // GameMode mode;
    // HelpStateManager* helpMgr;
    Entity eRestart, bRestart;
    Entity eAbort, bAbort;
    Entity eHelp, bHelp;
    Scene::Enum previousState;

    PauseScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        const Color green("green");

        //Restart Text
        eRestart = theEntityManager.CreateEntity("eRestart",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("pause/eRestart"));
        TEXT_RENDERING(eRestart)->text = game->gameThreadContext->localizeAPI->text("continue_");

        //Restart button
        bRestart = theEntityManager.CreateEntity("bRestart",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("pause/bRestart"));

        //Help Text
        eHelp = theEntityManager.CreateEntity("eHelp",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("pause/eHelp"));
        TEXT_RENDERING(eHelp)->text = game->gameThreadContext->localizeAPI->text("help");

        //Help button
        bHelp = theEntityManager.CreateEntity("bHelp",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("pause/bHelp"));

        //Abort text
        eAbort = theEntityManager.CreateEntity("eAbort",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("pause/eAbort"));

        TEXT_RENDERING(eAbort)->text = game->gameThreadContext->localizeAPI->text("give_up");

        //Abort button
        bAbort = theEntityManager.CreateEntity("bAbort",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("pause/bAbort"));
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum from) override {
        theSoundSystem.loadSoundFile("audio/son_menu.ogg");
        TEXT_RENDERING(eRestart)->show = true;
        RENDERING(bRestart)->show = true;
        TEXT_RENDERING(eAbort)->show = true;
        RENDERING(bAbort)->show = true;
        TEXT_RENDERING(eHelp)->show = true;
        RENDERING(bHelp)->show = true;
        BUTTON(bRestart)->enabled = true;
        BUTTON(bAbort)->enabled = true;

        // theMusicSystem.toggleMute(true);
        BUTTON(bHelp)->enabled = true;

        previousState = from;

        // helpMgr->oldState = Pause;
        // helpMgr->mode = mode;

        game->datas->mode2Manager[game->datas->mode]->TogglePauseDisplay(true);

        game->stopInGameMusics();
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        if (BUTTON(bAbort)->clicked) {
            SOUND(bAbort)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            return Scene::MainMenu;
        } if (BUTTON(bRestart)->clicked) {
            SOUND(bRestart)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            return previousState;
        } if (BUTTON(bHelp)->clicked) {
            SOUND(bHelp)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
            return Scene::Help;
        }
        return Scene::Pause;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
        game->datas->mode2Manager[game->datas->mode]->TogglePauseDisplay(false);
    }

    void onExit(Scene::Enum) override {
        LOGI("'" << __PRETTY_FUNCTION__ << "'");
        TEXT_RENDERING(eRestart)->show = false;
        RENDERING(bRestart)->show = false;
        TEXT_RENDERING(eAbort)->show = false;
        RENDERING(bAbort)->show = false;
        TEXT_RENDERING(eHelp)->show = false;
        RENDERING(bHelp)->show = false;
        BUTTON(bRestart)->enabled = false;
        BUTTON(bAbort)->enabled = false;
        theMusicSystem.toggleMute(theSoundSystem.mute);

        BUTTON(bHelp)->enabled = false;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreatePauseSceneHandler(HeriswapGame* game) {
        return new PauseScene(game);
    }
}
