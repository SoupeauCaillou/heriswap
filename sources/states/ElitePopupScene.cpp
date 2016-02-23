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

#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

#include <sstream>

struct ElitePopupScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    //NormalGameModeManager* normalGameModeManager;
    Entity background, text;
    Entity eButton[2], eText[2];

    ElitePopupScene(HeriswapGame* game) : StateHandler<Scene::Enum>("elite_popup_scene") {
        this->game = game;
    }

    void setup(AssetAPI*) override {
        const Color green(HASH("green", 0x615465c4));
        background = theEntityManager.CreateEntityFromTemplate("popup/background");

        text = theEntityManager.CreateEntityFromTemplate("popup/text");

        std::stringstream a;
        for (int i=0; i<2; i++) {
            eText[i] = theEntityManager.CreateEntityFromTemplate("popup/button_text");
            eButton[i] = theEntityManager.CreateEntityFromTemplate("popup/button");

            TRANSFORM(eText[i])->position.y = TRANSFORM(eButton[i])->position.y = PlacementHelper::GimpYToScreen(850+i*183);
        }
        TEXT(text)->text = game->gameThreadContext->localizeAPI->text("change_difficulty");
        TEXT(eText[0])->text = game->gameThreadContext->localizeAPI->text("change_difficulty_yes");
        TEXT(eText[1])->text = game->gameThreadContext->localizeAPI->text("change_difficulty_no");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        RENDERING(background)->show = true;
        TEXT(text)->show = true;
        for (int i=0; i<2; i++) {
            RENDERING(eButton[i])->show = true;
            TEXT(eText[i])->show = true;
            BUTTON(eButton[i])->enabled = true;
        }
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        if (BUTTON(eButton[0])->clicked) {
            theHeriswapGridSystem.setGridFromDifficulty(theHeriswapGridSystem.nextDifficulty(theHeriswapGridSystem.sizeToDifficulty()));
            game->datas->mode2Manager[Normal]->points = 0;
            static_cast<NormalGameModeManager*>(game->datas->mode2Manager[Normal])->changeLevel(1);
            return Scene::Spawn;
        }
        else if (BUTTON(eButton[1])->clicked)
            return Scene::Spawn;
        return Scene::ElitePopup;
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
    StateHandler<Scene::Enum>* CreateElitePopupSceneHandler(HeriswapGame* game) {
        return new ElitePopupScene(game);
    }
}
