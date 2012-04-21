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
	TRANSFORM(pauseButton)->position = Vector2(PlacementHelper::GimpXToScreen(23), PlacementHelper::GimpYToScreen(1215));
	TRANSFORM(pauseButton)->z = DL_Score;
	ADD_COMPONENT(pauseButton, TextRendering);
	TEXT_RENDERING(pauseButton)->color = Color(3.0/255, 99.0/255, 71.0/255);
	TEXT_RENDERING(pauseButton)->text = "Pause";
	TEXT_RENDERING(pauseButton)->fontName = "typo";
	TEXT_RENDERING(pauseButton)->charHeight = PlacementHelper::GimpHeightToScreen(30);
	TEXT_RENDERING(pauseButton)->positioning = TextRenderingComponent::LEFT;

	pauseContainer = theEntityManager.CreateEntity();
	ADD_COMPONENT(pauseContainer, Transformation);
	ADD_COMPONENT(pauseContainer, Container);
	ADD_COMPONENT(pauseContainer, Sound);
	SOUND(pauseContainer)->type = SoundComponent::EFFECT;
	ADD_COMPONENT(pauseContainer, Button);
	ADD_COMPONENT(pauseContainer, Rendering);
	RENDERING(pauseContainer)->color = Color(0.0, .0, .0, .0);
	TRANSFORM(pauseContainer)->z = DL_Score;
	CONTAINER(pauseContainer)->entities.push_back(pauseButton);
	CONTAINER(pauseContainer)->includeChildren = true;

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
	TEXT_RENDERING(pauseButton)->hide = false;
	RENDERING(pauseContainer)->hide = false;
	TEXT_RENDERING(scoreProgress)->hide = false;
	BUTTON(pauseContainer)->clicked=false;
}

void InGameUiHelper::update(float dt) {
	// handle button
	if (BUTTON(pauseContainer)->clicked) {
		SOUND(pauseContainer)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		BUTTON(pauseContainer)->clicked=false;
		game->togglePause(true);
	}
}

void InGameUiHelper::hide() {
	if (!built)
		return;
	TEXT_RENDERING(smallLevel)->hide = true;
	TEXT_RENDERING(pauseButton)->hide = true;
	RENDERING(pauseContainer)->hide = false;
	TEXT_RENDERING(scoreProgress)->hide = true;
}

void InGameUiHelper::destroy() {
	if (!built)
		return;
	theEntityManager.DeleteEntity(smallLevel);
	theEntityManager.DeleteEntity(pauseButton);
	theEntityManager.DeleteEntity(pauseContainer);
	theEntityManager.DeleteEntity(scoreProgress);
}
