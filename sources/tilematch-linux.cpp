
#include <GL/glfw.h>
#include <png.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/time.h>
#include <algorithm>

#include "base/Vector2.h"
#include "base/TouchInputManager.h"

#include "systems/RenderingSystem.h"

#include "Game.h"

#include "states/ScoreBoardStateManager.h"

#define DT 1/60.
#define MAGICKEYTIME 0.1


static char* loadPng(const char* assetName, int* width, int* height);
static char* loadTextfile(const char* assetName);

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

static struct timeval startup_time;

float timeconverter(struct timeval tv) {
	return (tv.tv_sec + tv.tv_usec / 1000000.0f);
}

float gettime() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	timersub(&tv, &startup_time, &tv);
	return timeconverter(tv);
}

	class FileScoreStorage: public ScoreStorage {
	std::vector<ScoreEntry> loadFromStorage() {
		std::vector<ScoreEntry> result;
		FILE* file = fopen("scores.txt", "r");
		if (file) {
			ScoreEntry entry;
			char name[128];
			int i=0;
			while(i<10 && fscanf(file, "%d:%s\n", &entry.points, name)!=EOF) {
				entry.name = name;
				result.push_back(entry);

				i++;
			}
			fclose(file);
		} else {
			std::cout << "impossible de lire les scores" << std::endl;
		}

		std::sort(result.begin(), result.end(), ScoreStorage::ScoreEntryComp);
		return result;
	}

	void saveToStorage(const std::vector<ScoreEntry>& entries) {
		std::ofstream out("scores.txt", std::ios::out);
		for(int i=0;i<entries.size(); i++) {
			out << entries[i].points << ':' << entries[i].name << std::endl;
		}
		out.close();
	}
};

int main() {
	if (!glfwInit())
		return 1;

	if( !glfwOpenWindow( 420,700, 8,8,8,8,8,1, GLFW_WINDOW ) )
		return 1;

	gettimeofday(&startup_time,NULL);
	theRenderingSystem.setNativeAssetLoader(new LinuxNativeAssetLoader());
	theTouchInputManager.setNativeTouchStatePtr(new MouseNativeTouchState());

	Game game;
	game.init(new FileScoreStorage(), 420, 700);
	theTouchInputManager.init(Vector2(10, 10. * 700. / 400.), Vector2(420, 700));

	bool running = true;
	float timer = 0;
	float dtAccumuled=0, dt = 0, time = 0;

	time = gettime();

	int frames = 0;
	float nextfps = time + 5;
	while(running) {

		do {
			dt = gettime() - time;
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
		time = gettime();
		while (dtAccumuled >= DT){
			game.tick(DT);
			glfwSwapBuffers();
			running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
			//pause ?
			if (glfwGetKey( GLFW_KEY_SPACE ))
				game.togglePause(true);
			//magic key?
			if (glfwGetKey( GLFW_KEY_ENTER ) && timer<=0) {
				game.toggleShowCombi(false);
				timer = MAGICKEYTIME;
			}
			timer -= DT;
			dtAccumuled -= DT;
			frames++;
			if (time > nextfps) {
				std::cout << "FPS: " << (frames / 5) << std::endl;
				nextfps = time + 5;
				frames = 0;
			}
		}
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

	char* output = (char*) malloc(size * sizeof(char) + 1);
	fread(output, 1, size, file);
	output[size] = '\0';
	return output;
}
