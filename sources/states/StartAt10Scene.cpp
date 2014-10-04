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

#include "../modes/NormalModeManager.h"

#include "base/EntityManager.h"
#include "base/PlacementHelper.h"

#include "systems/AnchorSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

#include <sstream>

struct StartAt10Scene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Entity background, text;
    Entity eButton[2], eText[2];

    StartAt10Scene(HeriswapGame* game) : StateHandler<Scene::Enum>("start_at_10_scene") {
        this->game = game;
    }

    void setup(AssetAPI*) override {
        background = theEntityManager.CreateEntityFromTemplate("popup/background");

        text = theEntityManager.CreateEntityFromTemplate("popup/text");

        for (int i=0; i<2; i++) {
            eButton[i] = theEntityManager.CreateEntityFromTemplate("popup/button");
            eText[i] = theEntityManager.CreateEntityFromTemplate("popup/button_text");
            ANCHOR(eText[i])->parent = eButton[i];

            TRANSFORM(eButton[i])->position.y = PlacementHelper::GimpYToScreen(850+i*183);
        }
        TEXT(text)->text = game->gameThreadContext->localizeAPI->text("start_at_level_10");
        TEXT(eText[0])->text = game->gameThreadContext->localizeAPI->text("start_at_level_10_yes");
        TEXT(eText[1])->text = game->gameThreadContext->localizeAPI->text("rate_never");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//

    void onEnter(Scene::Enum) override {
        game->datas->faderHelper.start(Fading::In, 0.5);
        game->datas->faderHelper.registerFadingInEntity(background);
        game->datas->faderHelper.registerFadingInEntity(text);
        for (int i=0; i<2; i++) {
            game->datas->faderHelper.registerFadingInEntity(eButton[i]);
            game->datas->faderHelper.registerFadingInEntity(eText[i]);
        }

        static_cast<NormalGameModeManager*>(game->datas->mode2Manager[Normal])->showGameDecor(true);
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float dt) override {
        if (!game->datas->faderHelper.update(dt))
            return Scene::StartAt10;
        else {
            RENDERING(background)->show = true;
            TEXT(text)->show = true;
            for (int i=0; i<2; i++) {
                RENDERING(eButton[i])->show = true;
                TEXT(eText[i])->show = true;
                BUTTON(eButton[i])->enabled = true;
            }
        }

        if (BUTTON(eButton[0])->clicked) {
            game->prepareNewGame();
            game->setupGameProp();
            game->datas->mode2Manager[Normal]->points = 0;
            static_cast<NormalGameModeManager*>(game->datas->mode2Manager[Normal])->changeLevel(10);

            return Scene::Spawn;
        }
        else if (BUTTON(eButton[1])->clicked)
            return Scene::CountDown;
        return Scene::StartAt10;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        RENDERING(background)->show = false;
        TEXT(text)->show = false;
        for (int i=0; i<2; i++) {
            RENDERING(eButton[i])->show = false;
            BUTTON(eButton[i])->enabled = false;
            TEXT(eText[i])->show = false;
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateStartAt10SceneHandler(HeriswapGame* game) {
        return new StartAt10Scene(game);
    }
}
