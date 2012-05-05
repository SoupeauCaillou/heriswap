#include "MainMenuGameStateManager.h"

#include "base/EntityManager.h"
#include "base/TouchInputManager.h"

#include "systems/ButtonSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/MorphingSystem.h"
#include "systems/TransformationSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ContainerSystem.h"
#include "systems/SoundSystem.h"

#include "DepthLayer.h"
#include "base/PlacementHelper.h"

MainMenuGameStateManager::~MainMenuGameStateManager() {
	for (int i=0; i<4; i++) {
		theTextRenderingSystem.DestroyLocalEntity(eStart[i]);
		theEntityManager.DeleteEntity(bStart[i]);
	}
}

void MainMenuGameStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);

	//Creating text entities
	for (int i=0; i<4; i++)
		eStart[i] = theTextRenderingSystem.CreateEntity();
	//Settings
	for (int i=0; i<4; i++) {
		TRANSFORM(eStart[i])->z = DL_MainMenuUITxt;
		TEXT_RENDERING(eStart[i])->hide = true;
		TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::RIGHT;
		TEXT_RENDERING(eStart[i])->color = green;
		TEXT_RENDERING(eStart[i])->charHeight = PlacementHelper::GimpHeightToScreen(75);
		theMorphingSystem.Add(eStart[i]);
	    TypedMorphElement<Vector2>* posMorph = new TypedMorphElement<Vector2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, Vector2(PlacementHelper::GimpXToScreen(700),PlacementHelper::GimpYToScreen(100)));
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
	TEXT_RENDERING(eStart[0])->text = localizeAPI->text("Classique");
	TRANSFORM(eStart[0])->position.X = PlacementHelper::GimpXToScreen(394);
	TRANSFORM(eStart[0])->position.Y = TRANSFORM(bStart[0])->position.Y = PlacementHelper::GimpYToScreen(156);
	TEXT_RENDERING(eStart[1])->text = localizeAPI->text("Contre le temps");
	TRANSFORM(eStart[1])->position.X = PlacementHelper::GimpXToScreen(648);
	TRANSFORM(eStart[1])->position.Y = TRANSFORM(bStart[1])->position.Y = PlacementHelper::GimpYToScreen(339);
	TEXT_RENDERING(eStart[2])->text = localizeAPI->text("Contre le score");
	TRANSFORM(eStart[2])->position.X = PlacementHelper::GimpXToScreen(634);
	TRANSFORM(eStart[2])->position.Y = TRANSFORM(bStart[2])->position.Y = PlacementHelper::GimpYToScreen(522);
	TEXT_RENDERING(eStart[3])->text = localizeAPI->text("Mange tes feuilles");
	TRANSFORM(eStart[3])->position.X = PlacementHelper::GimpXToScreen(725);
	TRANSFORM(eStart[3])->position.Y = TRANSFORM(bStart[3])->position.Y = PlacementHelper::GimpYToScreen(705);

	//Adding containers
	for (int i=0; i<4; i++) {
		TypedMorphElement<Vector2>* posMorph = new TypedMorphElement<Vector2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, Vector2(PlacementHelper::GimpXToScreen(700),PlacementHelper::GimpYToScreen(100)));
		MORPHING(eStart[i])->elements.push_back(posMorph);
		ADD_COMPONENT(bStart[i], Sound);
		SOUND(bStart[i])->type = SoundComponent::EFFECT;
		ADD_COMPONENT(bStart[i], Button);
		BUTTON(bStart[i])->enabled = false;
	}

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
	theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);

	RENDERING(herisson->actor.e)->hide = false;

	for (int i=0; i<4; i++) {
		MORPHING(eStart[i])->active = false;
		RENDERING(bStart[i])->hide = false;
		TEXT_RENDERING(eStart[i])->hide = false;
		BUTTON(bStart[i])->enabled = true;
		RENDERING(menubg)->hide = false;
		RENDERING(menufg)->hide = false;
	}
	if (modeTitleToReset) {
		theMorphingSystem.reverse(MORPHING(modeTitleToReset));
		MORPHING(modeTitleToReset)->activationTime = 0;
		MORPHING(modeTitleToReset)->active = true;
	}
}

GameState MainMenuGameStateManager::Update(float dt) {
	Entity a = herisson->actor.e;//random vitesse size position decallage
	switchAnim(herisson);
	if (TRANSFORM(a)->position.X < PlacementHelper::GimpXToScreen(800)+TRANSFORM(a)->size.X) {
		TRANSFORM(a)->position.X += herisson->actor.speed/8.*dt;
	} else {
		herisson->anim.clear();
		loadHerissonTexture(MathUtil::RandomInt(8)+1, herisson);
		herisson->actor.speed = MathUtil::RandomFloatInRange(2.0f,4.0f);
		TRANSFORM(a)->size = Vector2(PlacementHelper::GimpWidthToScreen(310), PlacementHelper::GimpHeightToScreen(253))*MathUtil::RandomFloatInRange(.3f,1.f);
		TransformationSystem::setPosition(TRANSFORM(a), Vector2(PlacementHelper::GimpXToScreen(-MathUtil::RandomInt(300))-TRANSFORM(a)->size.X, PlacementHelper::GimpYToScreen(MathUtil::RandomIntInRange(830,1150))), TransformationSystem::SW);
	}
	if (!modeTitleToReset || modeTitleToReset && !MORPHING(modeTitleToReset)->active) {
		if (BUTTON(bStart[0])->clicked) {
			choosenGameMode = Normal;
			SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
			return ModeMenu;
		} else if (BUTTON(bStart[1])->clicked){
			choosenGameMode = ScoreAttack;
			SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
			return ModeMenu;
		} else if (BUTTON(bStart[2])->clicked){
			choosenGameMode = StaticTime;
			SOUND(bStart[2])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
			return ModeMenu;
		} else if (BUTTON(bStart[3])->clicked){
			choosenGameMode = TilesAttack;
			SOUND(bStart[3])->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
			return ModeMenu;
		}
	}
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);

	for (int i=0; i<4; i++) {
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
