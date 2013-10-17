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

#include <base/PlacementHelper.h>
#include <base/TouchInputManager.h>

#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

struct HelpScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Scene::Enum oldState;
    Entity title,text,postscriptum;

    enum State {
        HowToPlay,
        Objective
    } state;

    HelpScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        const Color green("green");

        title = theEntityManager.CreateEntityFromTemplate("help/title");

        // title text + bg
        text = theEntityManager.CreateEntityFromTemplate("help/text");

        postscriptum = theEntityManager.CreateEntityFromTemplate("help/postscriptum");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum oldState) override {
        state = HowToPlay;
        this->oldState = oldState;

        // TODO !
        // setup how to play help page
        TEXT(text)->show = true;
        TEXT(title)->show = true;
        TEXT(postscriptum)->show = true;
        if (game->datas->mode == Normal) {
            TEXT(text)->text = game->gameThreadContext->localizeAPI->text("help_mode1_1") + "\n\n" +
                game->gameThreadContext->localizeAPI->text("help_general_1");
            TEXT(title)->text = game->gameThreadContext->localizeAPI->text("mode_1");
            TEXT(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_continue");
        } else if (game->datas->mode == TilesAttack) {
            TEXT(title)->text = game->gameThreadContext->localizeAPI->text("mode_2");
            TEXT(text)->text = game->gameThreadContext->localizeAPI->text("help_mode2_1");
            TEXT(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_continue");
        } else {
            TEXT(title)->text = game->gameThreadContext->localizeAPI->text("mode_3");
            TEXT(text)->text = game->gameThreadContext->localizeAPI->text("help_mode3_1") + "\n\n" +
                game->gameThreadContext->localizeAPI->text("help_general_1");
            TEXT(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_continue");
        }

        game->datas->mode2Manager[game->datas->mode]->showGameDecor(true);
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        if (!theTouchInputManager.isTouched(0) && theTouchInputManager.wasTouched(0)) {
            if (state == HowToPlay) {
                if (game->datas->mode == Normal) {
                    TEXT(text)->text = game->gameThreadContext->localizeAPI->text("help_general_2") + " " +
                        game->gameThreadContext->localizeAPI->text("help_mode1_2");
                } else if (game->datas->mode == TilesAttack) {
                    TEXT(text)->text = game->gameThreadContext->localizeAPI->text("help_mode2_2");
                } else {
                    TEXT(text)->text = game->gameThreadContext->localizeAPI->text("help_general_2");
                }

                TEXT(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_play");
                state = Objective;

            } else {
                if (oldState == Scene::ModeMenu)
                    return Scene::CountDown;
                else
                    return oldState;
            }
        }
        return Scene::Help;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        TEXT(text)->show = false;
        TEXT(title)->show = false;
        TEXT(postscriptum)->show = false;
        theRenderingSystem.unloadAtlas("help");
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateHelpSceneHandler(HeriswapGame* game) {
        return new HelpScene(game);
    }
}
