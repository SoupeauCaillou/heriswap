#include "ModeMenuPersoStateManager.h"
#include "CallBack.cpp"

ModeMenuPersoStateManager::ModeMenuPersoStateManager(LocalizeAPI* lAPI) {
	localizeAPI = lAPI;
	ended = false;
}

ModeMenuPersoStateManager::~ModeMenuPersoStateManager() {
	theEntityManager.DeleteEntity(play);
	theEntityManager.DeleteEntity(back);
}

void ModeMenuPersoStateManager::Setup() {
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);
	// play text
	play = theEntityManager.CreateEntity();
	ADD_COMPONENT(play, Transformation);
	TRANSFORM(play)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(300));
	TRANSFORM(play)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(play, TextRendering);
	TEXT_RENDERING(play)->text = localizeAPI->text("Jouer");
	TEXT_RENDERING(play)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(play)->color = green;
	TEXT_RENDERING(play)->fontName = "typo";
	TEXT_RENDERING(play)->charHeight = PlacementHelper::GimpHeightToScreen(100);
	TEXT_RENDERING(play)->hide = true;

	// score title
	scoreTitle = theEntityManager.CreateEntity();
	ADD_COMPONENT(scoreTitle, Transformation);
	TRANSFORM(scoreTitle)->position = Vector2(PlacementHelper::GimpXToScreen(65), PlacementHelper::GimpYToScreen(476));
	TRANSFORM(scoreTitle)->z = DL_MainMenuUITxt;
	ADD_COMPONENT(scoreTitle, TextRendering);
	TEXT_RENDERING(scoreTitle)->text = localizeAPI->text("Score");
	TEXT_RENDERING(scoreTitle)->fontName = "typo";
	TEXT_RENDERING(scoreTitle)->positioning = TextRenderingComponent::LEFT;
	TEXT_RENDERING(scoreTitle)->color = green;
	TEXT_RENDERING(scoreTitle)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(scoreTitle)->hide = true;

	// play button
	playButton = theEntityManager.CreateEntity();
	ADD_COMPONENT(playButton, Transformation);
	ADD_COMPONENT(playButton, Container);
	CONTAINER(playButton)->entities.push_back(play);
	CONTAINER(playButton)->includeChildren = true;
	ADD_COMPONENT(playButton, Button);
	ADD_COMPONENT(playButton, Sound);
	SOUND(playButton)->type = SoundComponent::EFFECT;
	BUTTON(playButton)->enabled = false;

	// back button
	back = theEntityManager.CreateEntity();
	ADD_COMPONENT(back, Transformation);
	ADD_COMPONENT(back, Rendering);
	ADD_COMPONENT(back, Button);
	ADD_COMPONENT(back, Sound);
	TRANSFORM(back)->size = Vector2(PlacementHelper::GimpWidthToScreen(100), PlacementHelper::GimpHeightToScreen(75));
	TransformationSystem::setPosition(TRANSFORM(back), Vector2(PlacementHelper::GimpXToScreen(92), PlacementHelper::GimpYToScreen(82)), TransformationSystem::NW);
	TRANSFORM(back)->z = DL_MainMenuUITxt;
	RENDERING(back)->texture = theRenderingSystem.loadTextureFile("menu/back.png");
	SOUND(back)->type = SoundComponent::EFFECT;
	BUTTON(back)->enabled = false;

	// fond
	fond = theEntityManager.CreateEntity();
	ADD_COMPONENT(fond, Transformation);
	TRANSFORM(fond)->size = Vector2(PlacementHelper::GimpWidthToScreen(712), PlacementHelper::GimpHeightToScreen(1124));
	TRANSFORM(fond)->z = DL_MainMenuUIBg;
	TransformationSystem::setPosition(TRANSFORM(fond), Vector2(PlacementHelper::GimpXToScreen(44), PlacementHelper::GimpYToScreen(24)), TransformationSystem::NW);
	ADD_COMPONENT(fond, Rendering);
	RENDERING(fond)->texture = theRenderingSystem.loadTextureFile("menu/fond_menu_mode.png");
	RENDERING(fond)->color.a = 0.5;
}


void ModeMenuPersoStateManager::Enter() {
	LOGI("%s", __PRETTY_FUNCTION__);

	BUTTON(back)->enabled = true;
	BUTTON(playButton)->enabled = true;

	TEXT_RENDERING(play)->hide = false;
	TEXT_RENDERING(scoreTitle)->hide = false;
	RENDERING(back)->hide = false;
	TEXT_RENDERING(title)->hide = false;
	RENDERING(menubg)->hide = false;
	RENDERING(menufg)->hide = false;
	RENDERING(fond)->hide = false;
	TEXT_RENDERING(play)->text = ended ? localizeAPI->text("Rejouer") : localizeAPI->text("Jouer");
}

GameState ModeMenuPersoStateManager::Update(float dt) {
	//herisson
	Entity herissonActor=  herisson->actor.e;
	if (TRANSFORM(herissonActor)->position.X < PlacementHelper::ScreenWidth+TRANSFORM(herissonActor)->size.X) {
		TRANSFORM(herissonActor)->position.X += herisson->actor.speed*dt;
		switchAnim(herisson);
	} else {
		RENDERING(herissonActor)->hide = true;
	}

	if (BUTTON(playButton)->clicked) {
		SOUND(playButton)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		RENDERING(herisson->actor.e)->hide = true;
		TRANSFORM(herissonActor)->position.X = PlacementHelper::GimpXToScreen(0)-TRANSFORM(herissonActor)->size.X;
		TEXT_RENDERING(title)->hide = true;
		return ModeMenuToBlackState;
	} if (BUTTON(back)->clicked) {
		SOUND(back)->sound = theSoundSystem.loadSoundFile("audio/son_menu.ogg", false);
		return MainMenu;
	}
	return PersoModeMenu;
}

void ModeMenuPersoStateManager::Exit() {
    // nothing to do here: will be done in LateExit after Fading
}

void ModeMenuPersoStateManager::LateExit() {
	LOGI("%s", __PRETTY_FUNCTION__);
	TEXT_RENDERING(play)->hide = true;
	RENDERING(back)->hide = true;

	RENDERING(menubg)->hide = true;
	RENDERING(menufg)->hide = true;
	RENDERING(fond)->hide = true;

	BUTTON(back)->enabled = false;
	BUTTON(playButton)->enabled = false;
	TEXT_RENDERING(scoreTitle)->hide = true;
	RENDERING(herisson->actor.e)->hide = true;
}
