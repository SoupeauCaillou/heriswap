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

MainMenuGameStateManager::~MainMenuGameStateManager() {
	for (int i=0; i<2; i++) {
		theTextRenderingSystem.DeleteEntity(eStart[i]);
		theEntityManager.DeleteEntity(bStart[i]);
	}
	theTextRenderingSystem.DeleteEntity(eSuccess);
	theEntityManager.DeleteEntity(bSuccess);
}

void MainMenuGameStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	//Creating text entities
	for (int i=0; i<2; i++) {
		eStart[i] = theTextRenderingSystem.CreateEntity();

		TRANSFORM(eStart[i])->z = DL_MainMenuUITxt;
		TEXT_RENDERING(eStart[i])->hide = true;
		TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(eStart[i])->color = green;
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		theMorphingSystem.Add(eStart[i]);
	    TypedMorphElement<float>* sizeMorph = new TypedMorphElement<float>(&TEXT_RENDERING(eStart[i])->charHeight, TEXT_RENDERING(eStart[i])->charHeight, PlacementHelper::GimpHeightToScreen(54));
	    MORPHING(eStart[i])->elements.push_back(sizeMorph);
	    MORPHING(eStart[i])->timing = 0.2;

	    bStart[i] = theEntityManager.CreateEntity();
	    ADD_COMPONENT(bStart[i], Transformation);
	    TRANSFORM(bStart[i])->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
	    TRANSFORM(bStart[i])->position.X = 0;
	    TRANSFORM(bStart[i])->z = DL_MainMenuUIBg;
	    ADD_COMPONENT(bStart[i], Rendering);
	    RENDERING(bStart[i])->texture = theRenderingSystem.loadTextureFile("menu/fond_bouton.png");
	    RENDERING(bStart[i])->color.a = 0.5;
	}
	TEXT_RENDERING(eStart[0])->text = localizeAPI->text("mode_1", "Classique");
	TRANSFORM(eStart[0])->position.X = PlacementHelper::GimpXToScreen(394);
	TRANSFORM(eStart[0])->position.Y = TRANSFORM(bStart[0])->position.Y = PlacementHelper::GimpYToScreen(156);
	TEXT_RENDERING(eStart[1])->text = localizeAPI->text("mode_2", "Mange tes feuilles");
	TRANSFORM(eStart[1])->position.X = PlacementHelper::GimpXToScreen(700);
	TRANSFORM(eStart[1])->position.Y = TRANSFORM(bStart[1])->position.Y = PlacementHelper::GimpYToScreen(156+183);

	//Containers properties
	for (int i=0; i<2; i++) {
		TypedMorphElement<Vector2>* posMorph = new TypedMorphElement<Vector2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, Vector2(PlacementHelper::GimpXToScreen(700),PlacementHelper::GimpYToScreen(100)));
		MORPHING(eStart[i])->elements.push_back(posMorph);
		ADD_COMPONENT(bStart[i], Sound);
		ADD_COMPONENT(bStart[i], Button);
		BUTTON(bStart[i])->enabled = false;
	}


	eSuccess = theTextRenderingSystem.CreateEntity();
	TRANSFORM(eSuccess)->z = DL_MainMenuUITxt;
	TEXT_RENDERING(eSuccess)->hide = true;
	TEXT_RENDERING(eSuccess)->positioning = TextRenderingComponent::RIGHT;
	TEXT_RENDERING(eSuccess)->color = green;
	TEXT_RENDERING(eSuccess)->charHeight = PlacementHelper::GimpHeightToScreen(75);
	TEXT_RENDERING(eSuccess)->text = localizeAPI->text("success", "Succes");
    bSuccess = theEntityManager.CreateEntity();
    ADD_COMPONENT(bSuccess, Transformation);
    TRANSFORM(bSuccess)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(147));
    TRANSFORM(bSuccess)->position.X = 0;
    TRANSFORM(bSuccess)->z = DL_MainMenuUIBg;
    ADD_COMPONENT(bSuccess, Rendering);
    RENDERING(bSuccess)->texture = theRenderingSystem.loadTextureFile("menu/fond_bouton.png");
    RENDERING(bSuccess)->color.a = 0.5;
	ADD_COMPONENT(bSuccess, Sound);
	ADD_COMPONENT(bSuccess, Button);
	BUTTON(bSuccess)->enabled = false;
	TRANSFORM(eSuccess)->position.X = PlacementHelper::GimpXToScreen(394);
	TRANSFORM(eSuccess)->position.Y = TRANSFORM(bSuccess)->position.Y = PlacementHelper::GimpYToScreen(156+2*183);


	menubg = theEntityManager.CreateEntity();
	ADD_COMPONENT(menubg, Transformation);
	TRANSFORM(menubg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menubg), Vector2(0, PlacementHelper::GimpYToScreen(542)), TransformationSystem::N);
	TRANSFORM(menubg)->z = DL_MainMenuBg;
	ADD_COMPONENT(menubg, Rendering);
	RENDERING(menubg)->texture = theRenderingSystem.loadTextureFile("menu/2emeplan.png");
	RENDERING(menubg)->hide = true;

	menufg = theEntityManager.CreateEntity();
	ADD_COMPONENT(menufg, Transformation);
	TRANSFORM(menufg)->size = Vector2(PlacementHelper::ScreenWidth, PlacementHelper::GimpHeightToScreen(570));
	TransformationSystem::setPosition(TRANSFORM(menufg), Vector2(0, PlacementHelper::GimpYToScreen(1280)), TransformationSystem::S);
	TRANSFORM(menufg)->z = DL_MainMenuFg;
	ADD_COMPONENT(menufg, Rendering);
	RENDERING(menufg)->texture = theRenderingSystem.loadTextureFile("menu/1erplan.png");
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
	RENDERING(bSuccess)->hide = false;
	TEXT_RENDERING(eSuccess)->hide = false;
	BUTTON(bSuccess)->enabled = true;

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
	if (!modeTitleToReset || modeTitleToReset && !MORPHING(modeTitleToReset)->active) {
		if (BUTTON(bStart[0])->clicked) {
			choosenGameMode = Normal;
			SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return ModeMenu;
		} else if (BUTTON(bStart[1])->clicked){
			choosenGameMode = TilesAttack;
			SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg");
			return ModeMenu;
		}
		#ifdef ANDROID
		if (BUTTON(bSuccess)->clicked){
			successAPI->openfeintSuccess();
		}
		#endif
	}
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);

	TEXT_RENDERING(eSuccess)->hide = true;
	RENDERING(bSuccess)->hide = true;
	BUTTON(bSuccess)->enabled = false;

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
