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

#include <api/OpenURLAPI.h>
    
#include <glm/glm.hpp>

#include <sstream>

struct AboutUsPopupScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    //NormalGameModeManager* normalGameModeManager;
    Entity background, text;
    Entity eButton[3], eText[3];

    AboutUsPopupScene(HeriswapGame* g) : StateHandler<Scene::Enum>(), game(g) {
    }

    void setup() {
        const Color green("green");
        background = theEntityManager.CreateEntityFromTemplate("donate_popup_background");

        text = theEntityManager.CreateEntityFromTemplate("donate_popup_text");
        
        std::stringstream a;
        for (int i=0; i<3; i++) {
            eText[i] = theEntityManager.CreateEntityFromTemplate("donate_popup_button_text");
            eButton[i] = theEntityManager.CreateEntityFromTemplate("donate_popup_button");
            
            TRANSFORM(eText[i])->position.y = TRANSFORM(eButton[i])->position.y = 
                PlacementHelper::GimpYToScreen(800 + 200 * (i-1));
        }
        TEXT(text)->text = game->gameThreadContext->localizeAPI->text("donate");
        TEXT(eText[0])->text = game->gameThreadContext->localizeAPI->text("donate_flattr");
        TEXT(eText[1])->text = game->gameThreadContext->localizeAPI->text("donate_googleinapp");
        TEXT(eText[2])->text = game->gameThreadContext->localizeAPI->text("donate_sac_website");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        RENDERING(background)->show = true;
        TEXT(text)->show = true;
        for (int i=0; i<3; i++) {
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
            std::string url = game->gameThreadContext->localizeAPI->text("donate_flattr_url");
            game->gameThreadContext->openURLAPI->openURL(url);
            // return Scene::MainMenu;
        } else if (BUTTON(eButton[1])->clicked) {
            return Scene::MainMenu;
        } else if (BUTTON(eButton[2])->clicked) {
            std::string url = "http://soupeaucaillou.com";
            game->gameThreadContext->openURLAPI->openURL(url);
        }
        return Scene::AboutUsPopup;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        RENDERING(background)->show = false;
        TEXT(text)->show = false;
        for (int i=0; i<3; i++) {
            RENDERING(eButton[i])->show = false;
            BUTTON(eButton[i])->enabled = false;
            TEXT(eText[i])->show = false;
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateAboutUsPopupSceneHandler(HeriswapGame* game) {
        return new AboutUsPopupScene(game);
    }
}
