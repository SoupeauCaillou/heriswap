#include "MainMenuGameStateManager.h"

MainMenuGameStateManager::~MainMenuGameStateManager() {
	for (int i=0; i<3; i++) {
		theTextRenderingSystem.DestroyLocalEntity(eStart[i]);
		theEntityManager.DeleteEntity(bStart[i]);
	}
}

void MainMenuGameStateManager::Setup() {
	//Creating text entities
	for (int i=0; i<3; i++)
		eStart[i] = theTextRenderingSystem.CreateLocalEntity(7);
	//Settings
	for (int i=0; i<3; i++) {
		TRANSFORM(eStart[i])->z = DL_MainMenuUI;
		TRANSFORM(eStart[i])->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(300))-Vector2(0,2*i);
		TEXT_RENDERING(eStart[i])->hide = true;
		TEXT_RENDERING(eStart[i])->positioning = TextRenderingComponent::LEFT;
		theMorphingSystem.Add(eStart[i]);
	    TypedMorphElement<Vector2>* posMorph = new TypedMorphElement<Vector2>(&TRANSFORM(eStart[i])->position, TRANSFORM(eStart[i])->position, Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(50)));
	    TypedMorphElement<Color>* colorMorph = new TypedMorphElement<Color>(&TEXT_RENDERING(eStart[i])->color, TEXT_RENDERING(eStart[i])->color, Color(1.f,0.f,0.f, 1.f));
	    MORPHING(eStart[i])->elements.push_back(posMorph);
	    MORPHING(eStart[i])->elements.push_back(colorMorph);
	    MORPHING(eStart[i])->timing = 1;
	}
	TEXT_RENDERING(eStart[0])->text = "Normal";
	TEXT_RENDERING(eStart[1])->text = "Score Atk";
	TEXT_RENDERING(eStart[2])->text = "Frozen Time";

	//Adding containers
	for (int i=0; i<3; i++) {
		bStart[i] = theEntityManager.CreateEntity();
		ADD_COMPONENT(bStart[i], Transformation);
		ADD_COMPONENT(bStart[i], Sound);
		SOUND(bStart[i])->type = SoundComponent::EFFECT;
		ADD_COMPONENT(bStart[i], Button);
		ADD_COMPONENT(bStart[i], Rendering);
		RENDERING(bStart[i])->color = Color(1.0, .0, .0, .5);
		TRANSFORM(bStart[i])->z = DL_MainMenuUI-.001;
		BUTTON(bStart[i])->enabled = false;
	}
}

void MainMenuGameStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	// preload sound effect
	theSoundSystem.loadSoundFile("audio/click.wav", false);

	for (int i=0; i<3; i++) {
		MORPHING(eStart[i])->active = false;
		TRANSFORM(eStart[i])->position = Vector2(PlacementHelper::GimpXToScreen(50),PlacementHelper::GimpYToScreen(300))-Vector2(0,2*i);
		TRANSFORM(bStart[i])->size = Vector2(10,1);
		theTransformationSystem.setPosition(TRANSFORM(bStart[i]),TRANSFORM(eStart[i])->position, TransformationSystem::W);
		TEXT_RENDERING(eStart[i])->color = Color(1.f,1.f,1.f);
		RENDERING(bStart[i])->hide = false;
		TEXT_RENDERING(eStart[i])->hide = false;
		BUTTON(bStart[i])->enabled = true;
	}
}

GameState MainMenuGameStateManager::Update(float dt) {
	if (BUTTON(bStart[0])->clicked) {
		choosenGameMode = Normal;
		SOUND(bStart[0])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return ModeMenu;
	} else if (BUTTON(bStart[1])->clicked){
		choosenGameMode = ScoreAttack;
		SOUND(bStart[1])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return ModeMenu;
	} else if (BUTTON(bStart[2])->clicked){
		choosenGameMode = StaticTime;
		SOUND(bStart[2])->sound = theSoundSystem.loadSoundFile("audio/click.wav", false);
		return ModeMenu;
	}
	return MainMenu;
}

void MainMenuGameStateManager::Exit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	for (int i=0; i<3; i++) {
		if (i!=choosenGameMode-1) TEXT_RENDERING(eStart[i])->hide = true;
		RENDERING(bStart[i])->hide = true;
		BUTTON(bStart[i])->enabled = false;
	}

    MORPHING(eStart[choosenGameMode-1])->active = true;
}
