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
#include "InGameUiHelper.h"

#include <base/PlacementHelper.h>
#include <base/EntityManager.h>

#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"
#include "systems/ButtonSystem.h"

#include "DepthLayer.h"

InGameUiHelper::InGameUiHelper() : built(false) {

}

void InGameUiHelper::build() {
	if (built)
		return;

	smallLevel = theEntityManager.CreateEntity();
	ADD_COMPONENT(smallLevel, Transformation);
	TRANSFORM(smallLevel)->position = Vector2(PlacementHelper::GimpXToScreen(680), PlacementHelper::GimpYToScreen(188));
	TRANSFORM(smallLevel)->z = DL_Score;
	ADD_COMPONENT(smallLevel, TextRendering);
	TEXT_RENDERING(smallLevel)->color = Color(1, 1, 1);
	TEXT_RENDERING(smallLevel)->fontName = "gdtypo";
	TEXT_RENDERING(smallLevel)->charHeight = PlacementHelper::GimpHeightToScreen(232-150);
	TEXT_RENDERING(smallLevel)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(smallLevel)->flags |= TextRenderingComponent::IsANumberBit;

	pauseButton = theEntityManager.CreateEntity();
	ADD_COMPONENT(pauseButton, Transformation);
    TRANSFORM(pauseButton)->size = Vector2(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpHeightToScreen(95));
    TransformationSystem::setPosition(TRANSFORM(pauseButton), Vector2(-PlacementHelper::GimpWidthToScreen(354), PlacementHelper::GimpYToScreen(1215)), TransformationSystem::W);
    TRANSFORM(pauseButton)->z = DL_Score;
	ADD_COMPONENT(pauseButton, Rendering);
	RENDERING(pauseButton)->texture = theRenderingSystem.loadTextureFile("pause");
    ADD_COMPONENT(pauseButton, Sound);
	ADD_COMPONENT(pauseButton, Button);
    BUTTON(pauseButton)->overSize = 1.6;
	BUTTON(pauseButton)->enabled = false;

	scoreProgress = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreProgress, Transformation);
	TRANSFORM(scoreProgress)->z = DL_Score;
	TRANSFORM(scoreProgress)->position = Vector2(0, PlacementHelper::GimpYToScreen(1215));
	ADD_COMPONENT(scoreProgress, TextRendering);
	TEXT_RENDERING(scoreProgress)->color = Color(3.0/255, 99.0/255, 71.0/255);
	TEXT_RENDERING(scoreProgress)->fontName = "typo";
	TEXT_RENDERING(scoreProgress)->charHeight = PlacementHelper::GimpHeightToScreen(47);
	TEXT_RENDERING(scoreProgress)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(scoreProgress)->flags |= TextRenderingComponent::IsANumberBit;

	built = true;
}

void InGameUiHelper::show() {
	if (!built)
		return;

	TEXT_RENDERING(smallLevel)->hide = false;
	RENDERING(pauseButton)->hide = false;
	TEXT_RENDERING(scoreProgress)->hide = false;
	BUTTON(pauseButton)->enabled=true;
}

void InGameUiHelper::update(float dt) {
	// handle button
	if (BUTTON(pauseButton)->clicked) {
		SOUND(pauseButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
		game->togglePause(true);
	}
}

void InGameUiHelper::hide() {
	if (!built)
		return;
	TEXT_RENDERING(smallLevel)->hide = true;
	RENDERING(pauseButton)->hide = true;
	BUTTON(pauseButton)->enabled = false;
	TEXT_RENDERING(scoreProgress)->hide = true;
}

void InGameUiHelper::destroy() {
	if (!built)
		return;
	theEntityManager.DeleteEntity(smallLevel);
	theEntityManager.DeleteEntity(pauseButton);
	theEntityManager.DeleteEntity(scoreProgress);
}
