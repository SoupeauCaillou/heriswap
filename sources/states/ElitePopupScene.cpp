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

#include "../modes/NormalModeManager.h"

#include "base/EntityManager.h"
#include "base/PlacementHelper.h"

#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

#include <sstream>

struct ElitePopupScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    //NormalGameModeManager* normalGameModeManager;
    Entity background, text;
    Entity eButton[2], eText[2];

    ElitePopupScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        const Color green("green");
        background = theEntityManager.CreateEntity("background",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("background"));

        text = theEntityManager.CreateEntity("change_difficulty_text",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("change_difficulty_text"));
        // ADD_COMPONENT(text, Transformation);
        // TRANSFORM(text)->position = TRANSFORM(background)->position;
        // TRANSFORM(text)->size = TRANSFORM(background)->size;
        // TRANSFORM(text)->size.x *= 0.9f;
        // TRANSFORM(text)->size.y = (float)PlacementHelper::GimpHeightToScreen(147);
        // TransformationSystem::setPosition(TRANSFORM(text),
        //                                glm::vec2(0.f, (float)PlacementHelper::GimpYToScreen(236)),
        //                                TransformationSystem::N);
        // TEXT_RENDERING(text)->positioning = TextRenderingComponent::LEFT;
        // TEXT_RENDERING(text)->color = green;
        // TEXT_RENDERING(text)->charHeight = PlacementHelper::GimpHeightToScreen(55);
        // TEXT_RENDERING(text)->flags |= TextRenderingComponent::MultiLineBit;
        std::stringstream a;
        for (int i=0; i<2; i++) {
            a.str("");
            a << "change_difficulty_button_text_" << i;
            eText[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("change_difficulty_button_text"));

            TRANSFORM(eText[i])->z = DL_MainMenuUITxt;
            // TEXT_RENDERING(eText[i])->positioning = TextRenderingComponent::CENTER;
            // TEXT_RENDERING(eText[i])->color = green;
            // TEXT_RENDERING(eText[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);

            a.str("");
            a << "change_difficulty_button_" << i;
            eButton[i] = theEntityManager.CreateEntity(a.str(),
                EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("change_difficulty_button"));
            // ADD_COMPONENT(eButton[i], Transformation);
            // TRANSFORM(eButton[i])->size = glm::vec2((float)PlacementHelper::GimpWidthToScreen(708),
            //                                         (float)PlacementHelper::GimpHeightToScreen(147));
            // TRANSFORM(eButton[i])->z = DL_MainMenuUIBg;
            // ADD_COMPONENT(eButton[i], Rendering);
            // RENDERING(eButton[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
            // RENDERING(eButton[i])->color.a = 0.5;
            // ADD_COMPONENT(eButton[i], Button);
            // BUTTON(eButton[i])->enabled = false;

            // TRANSFORM(eText[i])->position.x = TRANSFORM(eButton[i])->position.x = 0;
            // TRANSFORM(eText[i])->position.y = TRANSFORM(eButton[i])->position.y = PlacementHelper::GimpYToScreen(850+i*183);
        }
        TEXT_RENDERING(text)->text = game->gameThreadContext->localizeAPI->text("change_difficulty");
        TEXT_RENDERING(eText[0])->text = game->gameThreadContext->localizeAPI->text("change_difficulty_yes");
        TEXT_RENDERING(eText[1])->text = game->gameThreadContext->localizeAPI->text("change_difficulty_no");
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        RENDERING(background)->show = true;
        TEXT_RENDERING(text)->show = true;
        for (int i=0; i<2; i++) {
            RENDERING(eButton[i])->show = true;
            TEXT_RENDERING(eText[i])->show = true;
            BUTTON(eButton[i])->enabled = true;
        }
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        if (BUTTON(eButton[0])->clicked) {
            LOGW("Change difficulty");
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
        TEXT_RENDERING(text)->show = false;
        for (int i=0; i<2; i++) {
            RENDERING(eButton[i])->show = false;
            BUTTON(eButton[i])->enabled = false;
            TEXT_RENDERING(eText[i])->show = false;
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateElitePopupSceneHandler(HeriswapGame* game) {
        return new ElitePopupScene(game);
    }
}
