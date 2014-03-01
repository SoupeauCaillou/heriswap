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

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "DepthLayer.h"

#include "base/EntityManager.h"
#include "base/PlacementHelper.h"

#include "systems/AnimationSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/MusicSystem.h"
#include "systems/SoundSystem.h"
#include "systems/SwypeButtonSystem.h"
#include "systems/TextSystem.h"
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
        eRestart = theEntityManager.CreateEntityFromTemplate("pause/eRestart");
        TEXT(eRestart)->text = game->gameThreadContext->localizeAPI->text("continue_");

        //Restart button
        bRestart = theEntityManager.CreateEntityFromTemplate("pause/bRestart");

        //Help Text
        eHelp = theEntityManager.CreateEntityFromTemplate("pause/eHelp");
        TEXT(eHelp)->text = game->gameThreadContext->localizeAPI->text("help");

        //Help button
        bHelp = theEntityManager.CreateEntityFromTemplate("pause/bHelp");

        //Abort button
        bAbort = theEntityManager.CreateEntityFromTemplate("pause/bAbort");
        //Abort text
        eAbort = theEntityManager.CreateEntityFromTemplate("pause/eAbort");
        TEXT(eAbort)->text = game->gameThreadContext->localizeAPI->text("give_up");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum from) override {
        theSoundSystem.loadSoundFile("audio/son_menu.ogg");
        TEXT(eRestart)->show = true;
        RENDERING(bRestart)->show = true;
        TEXT(eAbort)->show = true;
        RENDERING(bAbort)->show = true;
        TEXT(eHelp)->show = true;
        RENDERING(bHelp)->show = true;
        BUTTON(bRestart)->enabled = true;
        SWYPEBUTTON(bAbort)->enabled = true;
        TRANSFORM(bAbort)->position = SWYPEBUTTON(bAbort)->idlePos;

        // theMusicSystem.toggleMute(true);
        BUTTON(bHelp)->enabled = true;

        if (from != Scene::Help)
            previousState = from;
        else
            game->datas->mode2Manager[game->datas->mode]->showGameDecor(false);

        Entity herisson = game->datas->mode2Manager[game->datas->mode]->herisson;
        ANIMATION(herisson)->playbackSpeed = 0;

        game->datas->mode2Manager[game->datas->mode]->TogglePauseDisplay(true);

        game->stopInGameMusics();
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        if (SWYPEBUTTON(bAbort)->clicked) {
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
    void onPreExit(Scene::Enum to) override {
        game->datas->mode2Manager[game->datas->mode]->TogglePauseDisplay(false);
        if (to == Scene::Help)
            theHeriswapGridSystem.ShowAll(false);
    }

    void onExit(Scene::Enum) override {
        TEXT(eRestart)->show =
            TEXT(eHelp)->show =
            TEXT(eAbort)->show = false;

        RENDERING(bRestart)->show =
            RENDERING(bAbort)->show =
            RENDERING(bHelp)->show = false;

        BUTTON(bHelp)->enabled =
            BUTTON(bRestart)->enabled =
            SWYPEBUTTON(bAbort)->enabled = false;

        theMusicSystem.toggleMute(theSoundSystem.mute);

        Entity herisson = game->datas->mode2Manager[game->datas->mode]->herisson;
        ANIMATION(herisson)->playbackSpeed = 4.1f;
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreatePauseSceneHandler(HeriswapGame* game) {
        return new PauseScene(game);
    }
}
