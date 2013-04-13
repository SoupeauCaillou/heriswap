/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

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
#include "HelpStateManager.h"

#include <glm/glm.hpp>

#include <base/PlacementHelper.h>
#include <base/TouchInputManager.h>

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

void HelpStateManager::Setup() {
	const Color green("green");

    title = theEntityManager.CreateEntity("title");
    ADD_COMPONENT(title, Transformation);
    TRANSFORM(title)->position = glm::vec2(0.f, (float)PlacementHelper::ScreenHeight * 0.4);
    TRANSFORM(title)->z = DL_HelpText;
    ADD_COMPONENT(title, TextRendering);
    TEXT_RENDERING(title)->show = false;
    TEXT_RENDERING(title)->positioning = TextRenderingComponent::CENTER;
    TEXT_RENDERING(title)->color = green;
    TEXT_RENDERING(title)->charHeight = PlacementHelper::GimpHeightToScreen(70);

	// title text + bg
	text = theEntityManager.CreateEntity("text");
	ADD_COMPONENT(text, Transformation);
    TRANSFORM(text)->position = glm::vec2(0.f, (float)PlacementHelper::ScreenHeight * 0.3);
    TRANSFORM(text)->z = DL_HelpText;
    TRANSFORM(text)->size = glm::vec2((float)PlacementHelper::ScreenWidth * 0.9f, 
                                      (float)PlacementHelper::ScreenHeight);
	ADD_COMPONENT(text, TextRendering);
	TEXT_RENDERING(text)->show = false;
	TEXT_RENDERING(text)->positioning = TextRenderingComponent::LEFT;
    TEXT_RENDERING(text)->flags |= TextRenderingComponent::MultiLineBit;
	TEXT_RENDERING(text)->color = green;
	TEXT_RENDERING(text)->charHeight = PlacementHelper::GimpHeightToScreen(50);

    postscriptum = theEntityManager.CreateEntity("postscriptum");
    ADD_COMPONENT(postscriptum, Transformation);
    TRANSFORM(postscriptum)->position = glm::vec2(0.f, (float)(-PlacementHelper::ScreenHeight * 0.5f + 0.5f * PlacementHelper::GimpHeightToScreen(40)));
    TRANSFORM(postscriptum)->z = DL_HelpText;
    TRANSFORM(postscriptum)->size = glm::vec2(PlacementHelper::ScreenWidth * 0.9, PlacementHelper::ScreenHeight);
    ADD_COMPONENT(postscriptum, TextRendering);
    TEXT_RENDERING(postscriptum)->show = false;
    TEXT_RENDERING(postscriptum)->positioning = TextRenderingComponent::CENTER;
    TEXT_RENDERING(postscriptum)->color = green;
    TEXT_RENDERING(postscriptum)->charHeight = PlacementHelper::GimpHeightToScreen(40);
}

void HelpStateManager::Enter() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");

	state = HowToPlay;

    // TODO !
	// setup how to play help page
    TEXT_RENDERING(text)->show = true;
    TEXT_RENDERING(title)->show = true;
    TEXT_RENDERING(postscriptum)->show = true;
    if (mode == Normal) {
 //        TEXT_RENDERING(text)->text = localizeAPI->text("help_mode1_1", "blabla") + "\n\n" +
	// localizeAPI->text("help_general_1", "blabla");
        // TEXT_RENDERING(title)->text = localizeAPI->text("mode_1", "Score race");
        // TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_continue", "Click to continue");
        TEXT_RENDERING(text)->text = localizeAPI->text("help_mode1_1") + "\n\n" +
            localizeAPI->text("help_general_1");
        TEXT_RENDERING(title)->text = localizeAPI->text("mode_1");
        TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_continue");
    } else if (mode == TilesAttack) {
        // TEXT_RENDERING(title)->text = localizeAPI->text("mode_2", "Time attack");
        // TEXT_RENDERING(text)->text = localizeAPI->text("help_mode2_1", "blabla");
        // TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_continue", "Click to continue");
        TEXT_RENDERING(title)->text = localizeAPI->text("mode_2");
        TEXT_RENDERING(text)->text = localizeAPI->text("help_mode2_1");
        TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_continue");
    } else {
 //        TEXT_RENDERING(title)->text = localizeAPI->text("mode_3", "100 seconds");
 //        TEXT_RENDERING(text)->text = localizeAPI->text("help_mode3_1", "blabla") + "\n\n" +
	// localizeAPI->text("help_general_1", "blabla");
 //        TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_continue", "Click to continue");
        TEXT_RENDERING(title)->text = localizeAPI->text("mode_3");
        TEXT_RENDERING(text)->text = localizeAPI->text("help_mode3_1") + "\n\n" + localizeAPI->text("help_general_1");
        TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_continue");
    }
}

GameState HelpStateManager::Update(float) {
	if (!theTouchInputManager.isTouched(0) && theTouchInputManager.wasTouched(0)) {
		if (state == HowToPlay) {
            if (mode == Normal) {
                // TEXT_RENDERING(text)->text = localizeAPI->text("help_general_2", "blabla") + " " +
                // localizeAPI->text("help_mode1_2", "blabla");
                TEXT_RENDERING(text)->text = localizeAPI->text("help_general_2") + " " + localizeAPI->text("help_mode1_2");
            } else if (mode == TilesAttack) {
                // TEXT_RENDERING(text)->text = localizeAPI->text("help_mode2_2", "blabla");
                TEXT_RENDERING(text)->text = localizeAPI->text("help_mode2_2");
            } else {
                // TEXT_RENDERING(text)->text = localizeAPI->text("help_general_2", "blabla");
                TEXT_RENDERING(text)->text = localizeAPI->text("help_general_2");
            }

            // TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_play", "Click to play");
            TEXT_RENDERING(postscriptum)->text = localizeAPI->text("help_click_play");
			state = Objective;

		} else {
			return oldState;
		}
	}
	return Help;
}

void HelpStateManager::Exit() {
	LOGI("'" << __PRETTY_FUNCTION__ << "'");
	TEXT_RENDERING(text)->show = false;
    TEXT_RENDERING(title)->show = false;
    TEXT_RENDERING(postscriptum)->show = false;
    theRenderingSystem.unloadAtlas("help");
}
