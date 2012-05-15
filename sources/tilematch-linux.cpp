#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/glfw.h>
//liste des keys dans /usr/include/GL/glfw.h 
#include <png.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/time.h>
#include <algorithm>
#include <sqlite3.h>
#include <sstream>
#include <cassert>
#include <vector>

#include "base/Vector2.h"
#include "base/TouchInputManager.h"
#include "base/TimeUtil.h"
#include "base/MathUtil.h"

#include "systems/RenderingSystem.h"
#include "systems/SoundSystem.h"
#include "systems/MusicSystem.h"

#include "api/linux/MusicAPILinuxOpenALImpl.h"
#include "api/linux/AssetAPILinuxImpl.h"
#include "api/linux/SoundAPILinuxOpenALImpl.h"

#include "Game.h"
#include "CallBack.cpp"

#define DT 1/60.
#define MAGICKEYTIME 0.3


static char* loadPng(const char* assetName, int* width, int* height);
static char* loadTextfile(const char* assetName);

struct LinuxLocalizeAPI: public LocalizeAPI {
	std::string text(const std::string& t) {
		return t;
	}
};

struct LinuxNativeAssetLoader: public NativeAssetLoader {
	char* decompressPngImage(const std::string& assetName, int* width, int* height) {
		return loadPng(assetName.c_str(), width, height);
	}

	char* loadShaderFile(const std::string& assetName) {
		return loadTextfile(assetName.c_str());
	}
};

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

struct TerminalPlayerNameInputUI : public PlayerNameInputUI {
	public:
		std::string show(std::vector<std::string> names) {
			__log_enabled = false;
			std::cout << "Choose your name !" << std::endl;
			int size = MathUtil::Min((int)names.size(), 10);

			for (int i=0; i<size; i++) {
				std::cout << i <<". "<<names[i] << std::endl;
			}
			std::cout << "Or enter a new name : "  << std::endl;
			std::string res;
			std::cin >> res;
			std::cin.ignore(100, '\n');
			std::istringstream iss(res);
			int value;
			iss >> value;
			//if its a valid number pick names[number]
			if (res.size()<size/10+2 && value>0 && value<size || res[0]=='0') {
				return names[value];
			//else its not a number or number>size : its the new name
			} else {
				if (res.size()>10) res.resize(10);
				std::cout << "Want to save it ? (yes or no(=everything else))" << std::endl;
				std::string a;
				getline(std::cin, a);
				if (a.c_str()[0] == 'y') storage->saveOpt("name", res);
				return res;
			}
		}
		void query(std::string& result) {
			storage->getName(result);
			if (result.size()==0) {
				result = show(storage->getName(result));
				__log_enabled = true;
			}
		}
		ScoreStorage* storage;
};


class LinuxSqliteExec: public ScoreStorage {
	public :
		void saveOpt(std::string opt, std::string name) {
			std::stringstream tmp;
			tmp << "INSERT INTO info VALUES ('" << opt << "', '" << name << "')";
			request(tmp.str(), 0, 0);
		}

		std::vector<std::string> getName(std::string& result) {
			std::vector<std::string> res;
			std::cout << "no default name - choose in the list\n";
			request("select value from info where opt LIKE 'name' ", &res, callbackNames);
			return res;
		}

		bool soundEnable(bool switchIt) {
			std::string s;
			request("select value from info where opt like 'sound'", &s, 0);
			if (switchIt) {
				if (s=="on") request("UPDATE info SET value='off' where opt='sound'",0, 0);
				else request("UPDATE info SET value='on' where opt='sound'",0, 0);
				LOGI("switched to !%s", s.c_str());
			}
			return (s == "on");
		}

		void submitScore(ScoreStorage::Score scr, int mode, int diff) {
			std::stringstream tmp;
			tmp << "INSERT INTO score VALUES ('" << scr.name <<"'," << mode<<","<<diff<<","<<scr.points<<","<<scr.time<<","<<scr.level<<")";
			request(tmp.str().c_str(), 0, 0);
		}

		bool request(std::string s, void* res, int (*callbackP)(void*,int,char**,char**)) {
			sqlite3 *db;
			char *zErrMsg = 0;
			int rc = sqlite3_open("tilematch.db", &db);
			if( rc ){
				LOGI("Can't open database tilematch.db: %s\n", sqlite3_errmsg(db));
				sqlite3_close(db);
				return false;
			}
			//si on veut notre callback personnel(script component)
			if (callbackP && res) rc = sqlite3_exec(db, s.c_str(), callbackP, res, &zErrMsg);
			//sinon on prend celui par défaut
			else rc = sqlite3_exec(db, s.c_str(), callback, res, &zErrMsg);

			if( rc!=SQLITE_OK ){
				LOGI("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			sqlite3_close(db);
			return true;
		}

		bool initTable() {
			bool r = request("", 0, 0);
			if (r) {
				LOGI("initializing database...");
				request("create table score(name varchar2(11) default 'Anonymous', mode number(1) default '0', difficulty number(1) default '1', points number(7) default '0', time number(5) default '0', level number(3) default '1')", 0, 0);
				request("create table info(opt varchar2(8), value varchar2(11), constraint f1 primary key(opt,value))", 0, 0);
				std::string s;
				request("select value from info where opt like 'sound'", &s, 0);
				if (s.length()==0) request("insert into info values('sound', 'on')", 0, 0);
				s = "";
				request("select value from info where opt like 'helpActive'", &s, 0);
				if (s.length()==0) request("insert into info values('helpActive', '1')", 0, 0);
				s = "";
				request("select value from info where opt like 'gameb4Ads'", &s, 0);
				if (s.length()==0) request("insert into info values('gameb4Ads', '2')", 0, 0);
				else request("UPDATE info SET value='2' where opt='gameb4Ads'",0, 0);
				
			}
			return r;
		}
};

int main(int argc, char** argv) {
	if (!glfwInit())
		return 1;

	Vector2 reso16_9(394, 700);
	Vector2 reso16_10(437, 700);

	Vector2* reso = (argc == 1) ? &reso16_10 : &reso16_9;
	if( !glfwOpenWindow( reso->X,reso->Y, 8,8,8,8,8,8, GLFW_WINDOW ) )
		return 1;

	glewInit();


	// pose de l'origine du temps ici t = 0
	TimeUtil::init();
	uint8_t* state = 0;
	int size = 0;
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
	// vérification de la table des scores
	LinuxSqliteExec* sqliteExec = new LinuxSqliteExec();
	TerminalPlayerNameInputUI* term = new TerminalPlayerNameInputUI();
	if (!sqliteExec->initTable()) {
		LOGI("probleme d'initialisation sql");
		return 0;
	}
	term->storage=sqliteExec;

	Game game(new LinuxNativeAssetLoader(), sqliteExec, term, new SuccessAPI(), new LinuxLocalizeAPI());

	//theSoundSystem.init();
	theRenderingSystem.setNativeAssetLoader(new LinuxNativeAssetLoader());
	theTouchInputManager.setNativeTouchStatePtr(new MouseNativeTouchState());
    theMusicSystem.musicAPI = new MusicAPILinuxOpenALImpl();
    theMusicSystem.assetAPI = new AssetAPILinuxImpl();
    theSoundSystem.soundAPI = new SoundAPILinuxOpenALImpl();
    theMusicSystem.init();
    theSoundSystem.init();
    
    game.sacInit(reso->X,reso->Y);
	game.init(state, size);

	bool running = true;
	float timer = 0;
	float dtAccumuled=0, dt = 0, time = 0;

	time = TimeUtil::getTime();

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
			//pause ?
			if (glfwGetKey( GLFW_KEY_SPACE ))
				game.togglePause(true);
			//magic key?
			if ((glfwGetKey( GLFW_KEY_ENTER ) || glfwGetKey( GLFW_KEY_KP_ENTER) ) && timer<=0) {
				game.toggleShowCombi(false);
				timer = MAGICKEYTIME;
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

static char* loadPng(const char* assetName, int* width, int* height)
{
	std::cout << __FUNCTION__ << " : " << assetName << std::endl;
	png_byte* PNG_image_buffer;
	std::stringstream ss;
	ss << "./assets/" << assetName;
	FILE *PNG_file = fopen(ss.str().c_str(), "rb");
	if (PNG_file == NULL) {
		std::cout << ss.str() << " not found" << std::endl;
		return 0;
	}

	GLubyte PNG_header[8];

	fread(PNG_header, 1, 8, PNG_file);
	if (png_sig_cmp(PNG_header, 0, 8) != 0) {
		std::cout << "ERROR: " << ss.str() << " is not a PNG." << std::endl;
		return 0;
	}

	png_structp PNG_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (PNG_reader == NULL)
	{
		std::cout << "ERROR: Can't start reading %s." << ss.str() << std::endl;
		fclose(PNG_file);
		return 0;
	}

	png_infop PNG_info = png_create_info_struct(PNG_reader);
	if (PNG_info == NULL)
	{
		std::cout << "ERROR: Can't get info for " << ss.str() << std::endl;
		png_destroy_read_struct(&PNG_reader, NULL, NULL);
		fclose(PNG_file);
		return 0;
	}

	png_infop PNG_end_info = png_create_info_struct(PNG_reader);
	if (PNG_end_info == NULL)
	{
		std::cout << "ERROR: Can't get end info for " << ss.str() << std::endl;
		png_destroy_read_struct(&PNG_reader, &PNG_info, NULL);
		fclose(PNG_file);
		return 0;
	}

	if (setjmp(png_jmpbuf(PNG_reader)))
	{
		std::cout << "ERROR: Can't load " << ss.str() << std::endl;
		png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
		fclose(PNG_file);
		return 0;
	}

	png_init_io(PNG_reader, PNG_file);
	png_set_sig_bytes(PNG_reader, 8);

	png_read_info(PNG_reader, PNG_info);

	*width = png_get_image_width(PNG_reader, PNG_info);
	*height = png_get_image_height(PNG_reader, PNG_info);

	png_uint_32 bit_depth, color_type;
	bit_depth = png_get_bit_depth(PNG_reader, PNG_info);
	color_type = png_get_color_type(PNG_reader, PNG_info);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(PNG_reader);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		png_set_expand_gray_1_2_4_to_8(PNG_reader);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(PNG_reader);
	}

	if (png_get_valid(PNG_reader, PNG_info, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(PNG_reader);
	}
	else
	{
		png_set_filler(PNG_reader, 0xff, PNG_FILLER_AFTER);
	}

	if (bit_depth == 16)
	{
		png_set_strip_16(PNG_reader);
	}

	png_read_update_info(PNG_reader, PNG_info);

	PNG_image_buffer = (png_byte*)malloc(4 * (*width) * (*height));
	png_byte** PNG_rows = (png_byte**)malloc(*height * sizeof(png_byte*));

	unsigned int row;
	for (row = 0; row < *height; ++row) {
		PNG_rows[*height - 1 - row] = PNG_image_buffer + (row * 4 * *width);
	}

	png_read_image(PNG_reader, PNG_rows);

	free(PNG_rows);

	png_destroy_read_struct(&PNG_reader, &PNG_info, &PNG_end_info);
	fclose(PNG_file);

	return (char*)PNG_image_buffer;
}

static char* loadTextfile(const char* assetName)
{
	std::cout << __FUNCTION__ << " : " << assetName << std::endl;
	std::stringstream ss;
	ss << "./assets/" << assetName;
	FILE *file = fopen(ss.str().c_str(), "r");
	if (file == NULL) {
		std::cout << ss.str() << " not found" << std::endl;
		return 0;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);

	char* output = new char[size * sizeof(char) + 1];
	fread(output, 1, size, file);
	output[size] = '\0';
	return output;
}
