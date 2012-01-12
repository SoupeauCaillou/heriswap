#include <GL/glfw.h>

#include "base/Vector2.h"
#include "systems/RenderingSystem.h"
#include "base/TouchInputManager.h"

#include <png.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "Game.h"

static char* loadPng(const char* assetName, int* width, int* height);
static char* loadTextfile(const char* assetName);
static bool mouse(Vector2* windowCoords);

int main() {
	if (!glfwInit())
		return 1;

	if( !glfwOpenWindow( 420,700, 0,0,0,0,0,0, GLFW_WINDOW ) )
		return 1;
	
	theRenderingSystem.setDecompressPNGImagePtr(&loadPng);
	theRenderingSystem.setLoadShaderPtr(&loadTextfile);
	theTouchInputManager.setNativeTouchStatePtr(&mouse);

	Game game;
	game.init(420, 700);
	theRenderingSystem.init();
	theTouchInputManager.init(Vector2(10, 10. * 700. / 400.), Vector2(420, 700));

	bool running = true;
	while(running) {
		game.tick(1.0f / 60.0f);
		glfwSwapBuffers();

		running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
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
	ss << "./res/raw/" << assetName;
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
	ss << "./res/raw/" << assetName;
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

static bool mouse(Vector2* windowCoords) {
	int x,y;
	glfwGetMousePos(&x, &y);
	windowCoords->X = x;
	windowCoords->Y = y;

	return glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
}
