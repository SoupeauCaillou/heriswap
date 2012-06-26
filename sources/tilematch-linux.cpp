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
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glfw.h>
//liste des keys dans /usr/include/GL/glfw.h 
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/time.h>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <vector>

#include <base/Vector2.h>
#include <base/TouchInputManager.h>
#include <base/TimeUtil.h>
#include <base/MathUtil.h>

#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/MusicSystem.h"

#include "api/linux/MusicAPILinuxOpenALImpl.h"
#include "api/linux/AssetAPILinuxImpl.h"
#include "api/linux/SoundAPILinuxOpenALImpl.h"
#include "api/linux/LocalizeAPILinuxImpl.h"
#include "api/linux/NameInputAPILinuxImpl.h"
#include "api/linux/ExitAPILinuxImpl.h"

#include "api/linux/StorageAPILinuxImpl.h"

#include "systems/TextRenderingSystem.h"
#include "systems/ButtonSystem.h"
#include "systems/TransformationSystem.h"
#include "base/PlacementHelper.h"
#include "DepthLayer.h"

#include "Game.h"

#include <locale.h>
#include <libintl.h>

#define DT 1/60.
#define MAGICKEYTIME 0.3



class MouseNativeTouchState: public NativeTouchState {
	public:
		bool isTouching(Vector2* windowCoords) const {
			int x,y;
			glfwGetMousePos(&x, &y);
			windowCoords->X = x;
			windowCoords->Y = y;

			return glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
		}
};

Entity globalFTW = 0;
void GLFWCALL myCharCallback( int c, int action ) {
	if (globalFTW == 0)
		return;
		
	if (!TEXT_RENDERING(globalFTW)->hide) {
		if (action == GLFW_PRESS && (isalnum(c) || c == ' ')) {
			if (TEXT_RENDERING(globalFTW)->text.length() > 20) 
				return;
			// filter out all unsupported keystrokes
			TEXT_RENDERING(globalFTW)->text.push_back((char)c);
		}
	}
}

int main(int argc, char** argv) {
	if (!glfwInit())
		return 1;

	Vector2 reso16_9(394, 700);
	Vector2 reso16_10(430, 700);

	glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );
	
	Vector2* reso = &reso16_10; // (argc == 1) ? &reso16_10 : &reso16_9;
	if( !glfwOpenWindow( reso->X,reso->Y, 8,8,8,8,8,8, GLFW_WINDOW ) )
		return 1;

	glfwSetWindowTitle("Heriswap");
	
	glewInit();

	// pose de l'origine du temps ici t = 0
	TimeUtil::init();
	uint8_t* state = 0;
	int size = 0;
	#if 0
	if (argc > 1 && !strcmp(argv[1], "-restore")) {
		FILE* file = fopen("dump.bin", "r+b");
		if (file) {
			std::cout << "Restoring game state from file" << std::endl;
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fseek(file, 0, SEEK_SET);
			state = new uint8_t[size];
			fread(state, size, 1, file);
			fclose(file);
		}
	}
	#endif

    StorageAPILinuxImpl* storage = new StorageAPILinuxImpl();
    storage->init();

	LocalizeAPILinuxImpl* loc = new LocalizeAPILinuxImpl();
	
	NameInputAPILinuxImpl* nameInput = new NameInputAPILinuxImpl();
	
	Game game(new AssetAPILinuxImpl(), storage, nameInput, new SuccessAPI(), loc, new AdAPI(), new ExitAPILinuxImpl());

	theRenderingSystem.opengles2 = true;
	//theSoundSystem.init();
	theTouchInputManager.setNativeTouchStatePtr(new MouseNativeTouchState());
	MusicAPILinuxOpenALImpl* openal = new MusicAPILinuxOpenALImpl();
    theMusicSystem.musicAPI = openal;
    theMusicSystem.assetAPI = new AssetAPILinuxImpl();
    theRenderingSystem.assetAPI = new AssetAPILinuxImpl();
    SoundAPILinuxOpenALImpl* soundAPI = new SoundAPILinuxOpenALImpl();
    theSoundSystem.soundAPI = soundAPI;
    openal->init();
    theMusicSystem.init();
    soundAPI->init();
    
    game.sacInit(reso->X,reso->Y);
    
    setlocale( LC_ALL, "" );
	loc->init();
	
	game.init(state, size);
	
	Color green = Color(3.0/255.0, 99.0/255, 71.0/255);
	// name input entities
	nameInput->title = theEntityManager.CreateEntity();
	ADD_COMPONENT(nameInput->title, Transformation);
	TRANSFORM(nameInput->title)->position = Vector2(0, PlacementHelper::GimpYToScreen(275));
	TRANSFORM(nameInput->title)->z = DL_HelpText;
	ADD_COMPONENT(nameInput->title, TextRendering);
	TEXT_RENDERING(nameInput->title)->text = loc->text("enter_name", "Enter your name:");
	TEXT_RENDERING(nameInput->title)->fontName = "typo";
	TEXT_RENDERING(nameInput->title)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(nameInput->title)->color = green;
	TEXT_RENDERING(nameInput->title)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(nameInput->title)->hide = true;
	
	globalFTW = nameInput->nameEdit = theEntityManager.CreateEntity();
	ADD_COMPONENT(nameInput->nameEdit, Transformation);
	TRANSFORM(nameInput->nameEdit)->position = Vector2(0, PlacementHelper::GimpYToScreen(390));
	TRANSFORM(nameInput->nameEdit)->z = DL_HelpText;
	ADD_COMPONENT(nameInput->nameEdit, TextRendering);
	TEXT_RENDERING(nameInput->nameEdit)->fontName = "typo";
	TEXT_RENDERING(nameInput->nameEdit)->positioning = TextRenderingComponent::CENTER;
	TEXT_RENDERING(nameInput->nameEdit)->color = green;
	TEXT_RENDERING(nameInput->nameEdit)->charHeight = PlacementHelper::GimpHeightToScreen(54);
	TEXT_RENDERING(nameInput->nameEdit)->hide = true;
	
	nameInput->background = theEntityManager.CreateEntity();
	ADD_COMPONENT(nameInput->background, Transformation);
	TRANSFORM(nameInput->background)->size = Vector2(PlacementHelper::GimpWidthToScreen(708), PlacementHelper::GimpHeightToScreen(256));
	TRANSFORM(nameInput->background)->position = Vector2(0, PlacementHelper::GimpYToScreen(320));
	TRANSFORM(nameInput->background)->z = DL_HelpTextBg;
	ADD_COMPONENT(nameInput->background, Rendering);
	RENDERING(nameInput->background)->hide = true;
	RENDERING(nameInput->background)->texture = theRenderingSystem.loadTextureFile("fond_bouton");
	RENDERING(nameInput->background)->color.a = 1;

	bool running = true;
	float timer = 0;
	float dtAccumuled=0, dt = 0, time = 0;

	time = TimeUtil::getTime();
	
	glfwSetCharCallback(myCharCallback);


	int frames = 0;
	float nextfps = time + 5;
	while(running) {

		do {
			dt = TimeUtil::getTime() - time;
			if (dt < DT) {
				struct timespec ts;
				ts.tv_sec = 0;
				ts.tv_nsec = (DT - dt) * 1000000000LL;
				nanosleep(&ts, 0);
			}
		} while (dt < DT);

		if (dt > 1./20) {
			dt = 1./20.;
		}
		dtAccumuled += dt;
		time = TimeUtil::getTime();
		while (dtAccumuled >= DT){
			dtAccumuled -= DT;
			game.tick(DT);
			running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
			bool focus = glfwGetWindowParam(GLFW_ACTIVE);
			if (focus) {
				theMusicSystem.toggleMute(theSoundSystem.mute);
			} else {
				// theMusicSystem.toggleMute(true);
			}
			//pause ?
			if (glfwGetKey( GLFW_KEY_SPACE )) {// || !focus) {
				game.togglePause(true);
			}
			//magic key?
			if ((glfwGetKey( GLFW_KEY_ENTER ) || glfwGetKey( GLFW_KEY_KP_ENTER) ) && timer<=0) {
				if (!TEXT_RENDERING(nameInput->nameEdit)->hide) {
					nameInput->textIsReady = true;
				}
				// game.toggleShowCombi(false);
				// timer = MAGICKEYTIME;
			}
			if (glfwGetKey( GLFW_KEY_BACKSPACE)) {
				// game.backPressed();
				if (!TEXT_RENDERING(nameInput->nameEdit)->hide) {
					std::string& text = TEXT_RENDERING(nameInput->nameEdit)->text;
					if (text.length() > 0) {
						text.resize(text.length() - 1);
					}
				}
			}
			if (glfwGetKey( GLFW_KEY_LSHIFT)) {
				uint8_t* state = 0;
				int size = game.saveState(&state);
				if (size) {
					LOGI("ptr: %p %d", state, size);
					FILE* file = fopen("dump.bin", "w+b");
					fwrite(state, size, 1, file);
					fclose(file);
				}
				running = false;
				break;
			}
			timer -= DT;
			frames++;
			if (time > nextfps) {
				//std::cout << "FPS: " << (frames / 5) << std::endl;
				nextfps = time + 5;
				frames = 0;
			}
		}

		theRenderingSystem.render();
		glfwSwapBuffers();
	}

	Vector2 x(Vector2::Zero);

	glfwTerminate();

	return 0;
}
