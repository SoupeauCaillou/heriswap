#include "MainMenuGameStateManager.h"

#include <base/EntityManager.h>
#include <base/TouchInputManager.h>
#include <base/PlacementHelper.h>

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"

void MainMenuGameStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	//Creating text entities
	for (int i=0; i<2; i++) {
		eStart[i] = theTextRenderingSystem.CreateEntity();

		TRANSFORM(eStart[i])->z = DL_MainMenuUITxt;
		TEXT_RENDERING(eStart[i])->hide = true;
		TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::LEFT;
		TEXT_RENDERING(eStart[i])->color = green;
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		ADD_COMPONENT(eStart[i], Morphing);
	    TypedMorphElement<float>* sizeMorph = new TypedMorphElement<float>(&TEXT_RENDERING(eStart[i])->charHeight, TEXT_RENDERING(eStart[i])->charHeight, PlacementHelper::GimpHeightToScreen(54));
	    MORPHING(eStart[i])->elements.push_back(sizeMorph);
	    MORPHING(eStart[i])->timing = 0.2;

	    bStart[i] = theEntityManager.CreateEntity();
	    ADD_COMPONENT(bStart[i], Transformation);
	    TRANSFORM(bStart[i])->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	    TRANSFORM(bStart[i])->position.X = 0;
	    TRANSFORM(bStart[i])->z = DL_MainMenuUIBg;
	    ADD_COMPONENT(bStart[i], Rendering);
	    RENDERING(bStart[i])->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	    RENDERING(bStart[i])->color.a = 0.5;
	}
	TEXT_RENDERING(eStart[0])->text = localizeAPI->text("mode_1", "Course au score");
	TRANSFORM(eStart[0])->position.X = PlacementHelper::GimpXToScreen(75);
	TRANSFORM(eStart[0])->position.Y = TRANSFORM(bStart[0])->position.Y = PlacementHelper::GimpYToScreen(156);
	TEXT_RENDERING(eStart[1])->text = localizeAPI->text("mode_2", "Contre la montre");
	TRANSFORM(eStart[1])->position.X = PlacementHelper::GimpXToScreen(75);
	TRANSFORM(eStart[1])->position.Y = TRANSFORM(bStart[1])->position.Y = PlacementHelper::GimpYToScreen(156+183);

	//Containers properties
	for (int i=0; i<2; i++) {
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(54);
		float w = theTextRenderingSystem.computeTextRenderingComponentWidth(TEXT_RENDERING(eStart[i]));
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		Vector2 target = Vector2(PlacementHelper::GimpXToScreen(700) - w ,PlacementHelper::GimpYToScreen(100));
		TypedMorphElement<Vector2>* posMorph = new TypedMorphElement<Vector2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, target);
        MORPHING(eStart[i])->elements.push_back(posMorph);
		ADD_COMPONENT(bStart[i], Sound);
		ADD_COMPONENT(bStart[i], Button);
		BUTTON(bStart[i])->enabled = false;
	}
	
	menubg = theEntityManager.CreateEntity();
	ADD_COMPONENT(menubg, Transformation);
	TRANSFORM(menubg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menubg), Vector2(0, PlacementHelper::GimpYToScreen(542)), TransformationSystem::N);
	TRANSFORM(menubg)->z = DL_MainMenuBg;
	ADD_COMPONENT(menubg, Rendering);
	RENDERING(menubg)->texture = theRenderingSystem.loadTextureFile("2emeplan");
	RENDERING(menubg)->hide = true;

	menufg = theEntityManager.CreateEntity();
	ADD_COMPONENT(menufg, Transformation);
	TRANSFORM(menufg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menufg), Vector2(0, PlacementHelper::GimpYToScreen(1280)), TransformationSystem::S);
	TRANSFORM(menufg)->z = DL_MainMenuFg;
	ADD_COMPONENT(menufg, Rendering);
	RENDERING(menufg)->texture = theRenderingSystem.loadTextureFile("1erplan");
	RENDERING(menufg)->hide = true;

	herisson = new AnimatedActor();
	herisson->frames=0;
	herisson->actor.speed = 4.1;
	Entity a = theEntityManager.CreateEntity();
	ADD_COMPONENT(a, Transformation);
	TRANSFORM(a)->z = DL_MainMenuHerisson;
	ADD_COMPONENT(a, Rendering);
	herisson->actor.e = a;
	herisson->anim.clear();
	loadHerissonTexture(MathUtil::RandomInt(8)+1, herisson);
	herisson->actor.speed = MathUtil::RandomFloatInRange(2.0f,4.0f);
	TRANSFORM(a)->size = Vector2(PlacementHelper::GimpWidthToScreen(310), PlacementHelper::GimpHeightToScreen(253))*MathUtil::RandomFloatInRange(.3f,1.f);
	TransformationSystem::setPosition(TRANSFORM(a), Vector2(PlacementHelper::GimpXToScreen(-MathUtil::RandomInt(300))-TRANSFORM(a)->size.X, PlacementHelper::GimpYToScreen(MathUtil::RandomIntInRange(830,1150))), TransformationSystem::SW);

	modeTitleToReset = 0;
}

void MainMenuGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	// preload sound effect
	theSoundSystem.loadSoundFile("audio/son_menu.ogg");

	RENDERING(herisson->actor.e)->hide = false;

	for (int i=0; i<2; i++) {
		RENDERING(bStart[i])->hide = false;
		TEXT_RENDERING(eStart[i])->hide = false;
		BUTTON(bStart[i])->enabled = true;
	}
	RENDERING(menubg)->hide = false;
	RENDERING(menufg)->hide = false;

	if (modeTitleToReset) {
		theMorphingSystem.reverse(MORPHING(modeTitleToReset));
		MORPHING(modeTitleToReset)->activationTime = 0;
		MORPHING(modeTitleToReset)->active = true;
	}
}

GameState MainMenuGameStateManager::Update(float dt) {
	Entity a = herisson->actor.e;
	switchAnim(herisson);
	if (TRANSFORM(a)->position.X < PlacementHelper::GimpXToScreen(800)+TRANSFORM(a)->size.X) {
		TRANSFORM(a)->position.X += herisson->actor.speed/8.*dt;
	} else {
		herisson->anim.clear();
		loadHerissonTexture(MathUtil::RandomInt(8)+1, herisson);//random texture
		herisson->actor.speed = MathUtil::RandomFloatInRange(2.0f,4.0f);//speed
		TRANSFORM(a)->size = Vector2(PlacementHelper::GimpWidthToScreen(310), PlacementHelper::GimpHeightToScreen(253))*MathUtil::RandomFloatInRange(.3f,1.f);//size
		TransformationSystem::setPosition(TRANSFORM(a), Vector2(PlacementHelper::GimpXToScreen(-MathUtil::RandomInt(300))-TRANSFORM(a)->size.X, PlacementHelper::GimpYToScreen(MathUtil::RandomIntInRange(830,1150))), TransformationSystem::SW);//offset
	}
	if (!modeTitleToReset || (modeTitleToReset && !MORPHING(modeTitleToReset)->active)) {
		if (BUTTON(bStart[0])->clicked) {
			choosenGameMode = Normal;
			SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return ModeMenu;
		} 
		if(BUTTON(bStart[1])->clicked){
			choosenGameMode = TilesAttack;
			SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return ModeMenu;
		}
	}
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);

	for (int i=0; i<2; i++) {
		if (i!=choosenGameMode-1) TEXT_RENDERING(eStart[i])->hide = true;
		RENDERING(bStart[i])->hide = true;
		BUTTON(bStart[i])->enabled = false;
	}

	if (modeTitleToReset) {
		theMorphingSystem.reverse(MORPHING(modeTitleToReset));
	}
    MORPHING(eStart[choosenGameMode-1])->active = true;
    modeTitleToReset = eStart[choosenGameMode-1];

    herisson->actor.speed = 4.5f;
}
