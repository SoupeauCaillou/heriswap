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

#if SAC_RESTRICTIVE_PLUGINS
#include "api/InAppPurchaseAPI.h"
#endif

namespace Image {
    enum Enum {
        Background = 0,
        Wolf,
        Count
    };
}

namespace Button {
    enum Enum {
        Flattr = 0,
#if SAC_RESTRICTIVE_PLUGINS
        Iap,
#endif
        Web,
        Back,
        Count
    };
}

namespace Text {
    enum Enum {
        SupportUs = 0,
        AboutUs,
        Count
    };
}

struct AboutUsPopupScene : public StateHandler<Scene::Enum> {


    HeriswapGame* game;

    Entity images[Image::Count];
    Entity buttons[Button::Count];
    Entity texts[Text::Count];

    AboutUsPopupScene(HeriswapGame* g) : StateHandler<Scene::Enum>(), game(g) {
    }

    void setup() {
        const Color green("green");

        images[Image::Background] = theEntityManager.CreateEntityFromTemplate("aboutus/background");
        images[Image::Wolf] = theEntityManager.CreateEntityFromTemplate("aboutus/wolf");


        texts[Text::SupportUs] = theEntityManager.CreateEntityFromTemplate("aboutus/supportus_text");
        texts[Text::AboutUs] = theEntityManager.CreateEntityFromTemplate("aboutus/aboutus_text");

        buttons[Button::Flattr] = theEntityManager.CreateEntityFromTemplate("aboutus/flattr_button");
#if SAC_RESTRICTIVE_PLUGINS
        buttons[Button::Iap] = theEntityManager.CreateEntityFromTemplate("aboutus/iap_button");
#endif
        buttons[Button::Web] = theEntityManager.CreateEntityFromTemplate("aboutus/web_button");
        buttons[Button::Back] = theEntityManager.CreateEntityFromTemplate("modemenu/back_button");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
        for (int i=0; i<Text::Count; i++) {
            TEXT(texts[i])->show = true;
        }
        for (int i=0; i<Button::Count; i++) {
            RENDERING(buttons[i])->show =
                BUTTON(buttons[i])->enabled = true;
        }
        for (int i=0; i<Image::Count; i++) {
            RENDERING(images[i])->show = true;
        }
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        if (BUTTON(buttons[Button::Flattr])->clicked) {
            std::string url = game->gameThreadContext->localizeAPI->text("donate_flattr_url");
            game->gameThreadContext->openURLAPI->openURL(url);
            // return Scene::MainMenu;
        } else if (BUTTON(buttons[Button::Back])->clicked) {
            return Scene::MainMenu;
        } else if (BUTTON(buttons[Button::Web])->clicked) {
            std::string url = "http://soupeaucaillou.com";
            game->gameThreadContext->openURLAPI->openURL(url);
        }

#if SAC_RESTRICTIVE_PLUGINS
        if (BUTTON(buttons[Button::Iap])->clicked) {
            game->gameThreadContext->inAppPurchaseAPI->purchase("donate");
        }
#endif

        return Scene::AboutUsPopup;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        for (int i=0; i<Text::Count; i++) {
            TEXT(texts[i])->show = false;
        }
        for (int i=0; i<Button::Count; i++) {
            RENDERING(buttons[i])->show =
                BUTTON(buttons[i])->enabled = false;
        }
        for (int i=0; i<Image::Count; i++) {
            RENDERING(images[i])->show = false;
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateAboutUsPopupSceneHandler(HeriswapGame* game) {
        return new AboutUsPopupScene(game);
    }
}
