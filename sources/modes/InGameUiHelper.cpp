#include "InGameUiHelper.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"
#include "systems/ButtonSystem.h"
#include "../PlacementHelper.h"
#include "../DepthLayer.h"

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
	TEXT_RENDERING(smallLevel)->fontName = "typo";
	TEXT_RENDERING(smallLevel)->charHeight = PlacementHelper::GimpHeightToScreen(232-150);
	TEXT_RENDERING(smallLevel)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(smallLevel)->isANumber = true;

	pauseButton = theEntityManager.CreateEntity();
	ADD_COMPONENT(pauseButton, Transformation);
    TRANSFORM(pauseButton)->size = Vector2(PlacementHelper::GimpWidthToScreen(80), PlacementHelper::GimpHeightToScreen(80));
    TransformationSystem::setPosition(TRANSFORM(pauseButton), Vector2(PlacementHelper::GimpXToScreen(108), PlacementHelper::GimpYToScreen(1215)), TransformationSystem::E);
    TRANSFORM(pauseButton)->z = DL_Score;
	ADD_COMPONENT(pauseButton, Rendering);
	RENDERING(pauseButton)->color = Color(3.0/255, 99.0/255, 71.0/255);
	RENDERING(pauseButton)->texture = theRenderingSystem.loadTextureFile("pause.png");
    ADD_COMPONENT(pauseButton, Sound);
	SOUND(pauseButton)->type = SoundComponent::EFFECT;
	ADD_COMPONENT(pauseButton, Button);

	scoreProgress = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreProgress, Transformation);
	TRANSFORM(scoreProgress)->z = DL_Score;
	TRANSFORM(scoreProgress)->position = Vector2(0, PlacementHelper::GimpYToScreen(1215));
	ADD_COMPONENT(scoreProgress, TextRendering);
	TEXT_RENDERING(scoreProgress)->color = Color(3.0/255, 99.0/255, 71.0/255);
	TEXT_RENDERING(scoreProgress)->fontName = "typo";
	TEXT_RENDERING(scoreProgress)->charHeight = PlacementHelper::GimpHeightToScreen(47);
	TEXT_RENDERING(scoreProgress)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(scoreProgress)->isANumber = true;

	built = true;
}

void InGameUiHelper::show() {
	if (!built)
		return;

	TEXT_RENDERING(smallLevel)->hide = false;
	RENDERING(pauseButton)->hide = false;
	TEXT_RENDERING(scoreProgress)->hide = false;
	BUTTON(pauseButton)->clicked=false;
}

void InGameUiHelper::update(float dt) {
	// handle button
	if (BUTTON(pauseButton)->clicked) {
		SOUND(pauseButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		game->togglePause(true);
	}
}

void InGameUiHelper::hide() {
	if (!built)
		return;
	TEXT_RENDERING(smallLevel)->hide = true;
	RENDERING(pauseButton)->hide = false;
	TEXT_RENDERING(scoreProgress)->hide = true;
}

void InGameUiHelper::destroy() {
	if (!built)
		return;
	theEntityManager.DeleteEntity(smallLevel);
	theEntityManager.DeleteEntity(pauseButton);
	theEntityManager.DeleteEntity(scoreProgress);
}
