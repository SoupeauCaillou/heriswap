#include "HelpStateManager.h"

#include <base/PlacementHelper.h>

#include "systems/TransformationSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"

#include "Game.h"
#include "DepthLayer.h"

void HelpStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	background = theEntityManager.CreateEntity();
	ADD_COMPONENT(background, Rendering);
	ADD_COMPONENT(background, Transformation);
	TRANSFORM(background)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::ScreenHeight);
	TRANSFORM(background)->z = DL_Help1;
	
	bg2 = theEntityManager.CreateEntity();
	ADD_COMPONENT(bg2, Rendering);
	ADD_COMPONENT(bg2, Transformation);
	TRANSFORM(bg2)->z = DL_Help2;

	// title text + bg
	title[0] = theEntityManager.CreateEntity();
	ADD_COMPONENT(title[0], Transformation);
	TransformationSystem::setPosition(TRANSFORM(title[0]), Vector2(0, PlacementHelper::GimpYToScreen(110)), TransformationSystem::C);
	TRANSFORM(title[0])->z = DL_HelpText;
	ADD_COMPONENT(title[0], TextRendering);
	TEXT_RENDERING(title[0])->hide = true;
	TEXT_RENDERING(title[0])->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(title[0])->color = green;
	TEXT_RENDERING(title[0])->charHeight = PlacementHelper::GimpHeightToScreen(75);
	title[1] = theEntityManager.CreateEntity();
    ADD_COMPONENT(title[1], Transformation);
    TRANSFORM(title[1])->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(110));
    TRANSFORM(title[1])->z = DL_HelpTextBg;
    TRANSFORM(title[1])->position = TRANSFORM(title[0])->position;
    ADD_COMPONENT(title[1], Rendering);
    RENDERING(title[1])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
    RENDERING(title[1])->color.a = 0.5;
    
    bonus = theEntityManager.CreateEntity();
    ADD_COMPONENT(bonus, Transformation);
	TransformationSystem::setPosition(TRANSFORM(bonus), Vector2(PlacementHelper::GimpXToScreen(328), PlacementHelper::GimpYToScreen(1150)), TransformationSystem::C);
	TRANSFORM(bonus)->z = DL_HelpText;
	ADD_COMPONENT(bonus, TextRendering);
	TEXT_RENDERING(bonus)->hide = true;
	TEXT_RENDERING(bonus)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(bonus)->color = green;
	TEXT_RENDERING(bonus)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(bonus)->text = localize->text("bonus_x2", "Bonus x2");


    
    // level texts
    levelBig = theEntityManager.CreateEntity();
    ADD_COMPONENT(levelBig, Transformation);
	TransformationSystem::setPosition(TRANSFORM(levelBig), Vector2(PlacementHelper::GimpXToScreen(454), PlacementHelper::GimpYToScreen(595)), TransformationSystem::C);
	TRANSFORM(levelBig)->z = DL_HelpText;
	ADD_COMPONENT(levelBig, TextRendering);
	TEXT_RENDERING(levelBig)->hide = true;
	TEXT_RENDERING(levelBig)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(levelBig)->color = green;
	TEXT_RENDERING(levelBig)->charHeight = PlacementHelper::GimpHeightToScreen(55);
	TEXT_RENDERING(levelBig)->text = localize->text("level", "Niveau");
	for (int i=0; i<3; i++) {
    	levelSmall[i] = theEntityManager.CreateEntity();
    	ADD_COMPONENT(levelSmall[i], Transformation);
		TransformationSystem::setPosition(TRANSFORM(levelSmall[i]), Vector2(PlacementHelper::GimpXToScreen(100), PlacementHelper::GimpYToScreen(717 + 82 * i)), TransformationSystem::C);
		TRANSFORM(levelSmall[i])->z = DL_HelpText;
		ADD_COMPONENT(levelSmall[i], TextRendering);
		TEXT_RENDERING(levelSmall[i])->hide = true;
		TEXT_RENDERING(levelSmall[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(levelSmall[i])->color = green;
		TEXT_RENDERING(levelSmall[i])->charHeight = PlacementHelper::GimpHeightToScreen(55);			
		TEXT_RENDERING(levelSmall[i])->text = localize->text("lvlcap", "Niv");
	}
	
	// difficulty
	for (int i=0; i<2; i++) {
    	difficulty[i] = theEntityManager.CreateEntity();
    	ADD_COMPONENT(difficulty[i], Transformation);
		TransformationSystem::setPosition(TRANSFORM(difficulty[i]), Vector2(PlacementHelper::GimpXToScreen(218 + i*360), PlacementHelper::GimpYToScreen(385)), TransformationSystem::C);
		TRANSFORM(difficulty[i])->z = DL_HelpText;
		ADD_COMPONENT(difficulty[i], TextRendering);
		TEXT_RENDERING(difficulty[i])->hide = true;
		TEXT_RENDERING(difficulty[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(difficulty[i])->color = green;
		TEXT_RENDERING(difficulty[i])->charHeight = PlacementHelper::GimpHeightToScreen(45);
		TEXT_RENDERING(difficulty[i])->text = ((i==0) ? localize->text("diff_1", "Débutant") : localize->text("diff_2", "Confirmé")) + ":";
	}
	
	// leaves left
    leaveLeft = theEntityManager.CreateEntity();
    ADD_COMPONENT(leaveLeft, Transformation);
	TransformationSystem::setPosition(TRANSFORM(leaveLeft), Vector2(PlacementHelper::GimpXToScreen(210), PlacementHelper::GimpYToScreen(730)), TransformationSystem::C);
	TRANSFORM(leaveLeft)->z = DL_HelpText;
	ADD_COMPONENT(leaveLeft, TextRendering);
	TEXT_RENDERING(leaveLeft)->hide = true;
	TEXT_RENDERING(leaveLeft)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(leaveLeft)->color = green;
	TEXT_RENDERING(leaveLeft)->charHeight = PlacementHelper::GimpHeightToScreen(55);
	TEXT_RENDERING(leaveLeft)->text = localize->text("leave_left", "feuilles restantes");
}

void HelpStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);
	
	state = HowToPlay;

	// setup how to play help page
	RENDERING(background)->texture = theRenderingSystem.loadTextureFile("bg_help_howto");
	RENDERING(background)->hide = false;
	TEXT_RENDERING(title[0])->hide = false;
	TEXT_RENDERING(title[0])->text = localize->text("how_to_play", "Comment jouer ?");
	RENDERING(title[1])->hide = false;
	TEXT_RENDERING(bonus)->hide = false;
}

GameState HelpStateManager::Update(float dt) {
	if (!theTouchInputManager.isTouched() && theTouchInputManager.wasTouched()) {
		if (state == HowToPlay) {
			if (mode == Normal) {
				RENDERING(bg2)->texture = theRenderingSystem.loadTextureFile("bg_help_obj_score");
				TRANSFORM(bg2)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(482*2));
				TransformationSystem::setPosition(TRANSFORM(bg2), Vector2(0, PlacementHelper::GimpYToScreen(112*2)), TransformationSystem::N);
			} else {
				RENDERING(bg2)->texture = theRenderingSystem.loadTextureFile("bg_help_obj_time");
				TRANSFORM(bg2)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(495*2));
				TransformationSystem::setPosition(TRANSFORM(bg2), Vector2(0, PlacementHelper::GimpYToScreen(101*2)), TransformationSystem::N);
			}
			std::cout << TRANSFORM(bg2)->size << ", " << TRANSFORM(bg2)->position << std::endl;
			RENDERING(bg2)->hide = false;
			
			TEXT_RENDERING(title[0])->text = localize->text("objective", "Objectif");
			if (mode == Normal) {
				TEXT_RENDERING(levelBig)->hide = false;
				for (int i=0; i<3; i++) {
					TEXT_RENDERING(levelSmall[i])->hide = false;
				}
			} else {
				TEXT_RENDERING(leaveLeft)->hide = false;
				for (int i=0; i<2; i++) {
					TEXT_RENDERING(difficulty[i])->hide = false;
				}			
			}
			TEXT_RENDERING(bonus)->hide = true;
			state = Objective;
		} else {
			return oldState;
		}
	}
	return Help;
}

void HelpStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);

	TEXT_RENDERING(levelBig)->hide = true;
	for (int i=0; i<3; i++) {
		TEXT_RENDERING(levelSmall[i])->hide = true;
	}
	TEXT_RENDERING(leaveLeft)->hide = true;
	for (int i=0; i<2; i++) {
		TEXT_RENDERING(difficulty[i])->hide = true;
	}
	RENDERING(background)->hide = true;
	RENDERING(bg2)->hide = true;
	TEXT_RENDERING(title[0])->hide = true;
	RENDERING(title[1])->hide = true;
	TEXT_RENDERING(bonus)->hide = true;
	
	theRenderingSystem.unloadAtlas("help");
}
