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

#include <base/PlacementHelper.h>

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

void HelpStateManager::Setup() {
	const Color green("green");

    title = theEntityManager.CreateEntity();
    ADD_COMPONENT(title, Transformation);
    TRANSFORM(title)->position = Vector2(0, PlacementHelper::ScreenHeight * 0.4);
    TRANSFORM(title)->z = DL_HelpText;
    ADD_COMPONENT(title, TextRendering);
    TEXT_RENDERING(title)->hide = true;
    TEXT_RENDERING(title)->positioning = TextRenderingComponent::CENTER;
    TEXT_RENDERING(title)->color = green;
    TEXT_RENDERING(title)->charHeight = PlacementHelper::GimpHeightToScreen(70);
 
	// title text + bg
	text = theEntityManager.CreateEntity();
	ADD_COMPONENT(text, Transformation);
    TRANSFORM(text)->position = Vector2(0, PlacementHelper::ScreenHeight * 0.3);
    TRANSFORM(text)->z = DL_HelpText;
    TRANSFORM(text)->size = Vector2(PlacementHelper::ScreenWidth * 0.9, PlacementHelper::ScreenHeight);
	ADD_COMPONENT(text, TextRendering);
	TEXT_RENDERING(text)->hide = true;
	TEXT_RENDERING(text)->positioning = TextRenderingComponent::LEFT;
    TEXT_RENDERING(text)->flags |= TextRenderingComponent::MultiLineBit;
	TEXT_RENDERING(text)->color = green;
	TEXT_RENDERING(text)->charHeight = PlacementHelper::GimpHeightToScreen(55);
}

void HelpStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	state = HowToPlay;

	// setup how to play help page
    TEXT_RENDERING(text)->hide = false;
    TEXT_RENDERING(title)->hide = false;
    if (mode == Normal) {
        TEXT_RENDERING(text)->text = 
            "L'objectif dans ce mode est de marquer le plus de points.\n\n"
            "Les points se gagnent en alignant au moins 3 feuilles ×feuille1,1,1× ×feuille1,1,1× ×feuille1,1,1× identiques.\n\n"
            "Ces combinaisons se créent en échangeant 2 feuilles ×feuille2,1,1× ×feuille3,1,1× voisines.\n\n"
            "Le jeu se joue en un temps limité.\n\n"
            "Le ×herisson_2_5,2,1.8× représente le temps restant et la partie se termine"
            " lorsqu'il arrive au bord droit de l'écran. "
            "Le type de ×feuille5,1,1× sur son dos rapporte plus de points.\n\n"
            "Chaque combinaison réussie rapporte un peu de temps. "
            "Pour gagner beaucoup de temps, il faut terminer un niveau. "
            "Un niveau est terminé lorsque l'arbre est vide.";
        TEXT_RENDERING(title)->text = localizeAPI->text("mode_1", "Score race");
    } else if (mode == TilesAttack) {
        TEXT_RENDERING(title)->text = localizeAPI->text("mode_2", "Time attack");
        TEXT_RENDERING(text)->text = "";
    } else {
        TEXT_RENDERING(title)->text = localizeAPI->text("mode_3", "100 seconds");
        TEXT_RENDERING(text)->text = "";
    }
}

GameState HelpStateManager::Update(float dt) {
	if (!theTouchInputManager.isTouched(0) && theTouchInputManager.wasTouched(0)) {
		if (state == HowToPlay) {
			state = Objective;
		} else {
			return oldState;
		}
	}
	return Help;
}

void HelpStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(text)->hide = true;
    TEXT_RENDERING(title)->hide = true;
    theRenderingSystem.unloadAtlas("help");
}
