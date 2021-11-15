#include <iostream>
#include <Grassland.h>
#include <ctime>
#include <queue>
#include <string>
#include <random>
#include <cmath>

#include <Windows.h>
#include <fstream>
#include <d3d12.h>
#include <memory>

#include <iostream>
#include <thread>
#include <chrono>
#include <io.h>
#include <fcntl.h>
#include <queue>

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace Grassland;



int main()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	GRLPtr<GRLIImage> text_img;
	GRLColor* text_img_buffer;

	if (FT_New_Face(ft, "fonts/STKAITI.TTF", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, L'ýQ', FT_LOAD_RENDER))
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	GRLCreateImage(face->glyph->bitmap.width, face->glyph->bitmap.rows, &text_img);

	text_img->GetImageBuffer(&text_img_buffer);
	for (int i = 0; i < text_img->GetWidth() * text_img->GetHeight(); i++)
	{
		float greyness = face->glyph->bitmap.buffer[i] * 1.0f / 255.0f;
		text_img_buffer[i] = GRLColor(greyness, greyness, greyness);
	}
	text_img->StoreBMP("text.bmp");
	text_img.Reset();

	if (FT_Load_Char(face, L'ÖÐ', FT_LOAD_RENDER))
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	GRLCreateImage(face->glyph->bitmap.width, face->glyph->bitmap.rows, &text_img);

	text_img->GetImageBuffer(&text_img_buffer);
	for (int i = 0; i < text_img->GetWidth() * text_img->GetHeight(); i++)
	{
		float greyness = face->glyph->bitmap.buffer[i] * 1.0f / 255.0f;
		text_img_buffer[i] = GRLColor(greyness, greyness, greyness);
	}
	text_img->StoreBMP("text2.bmp");
	text_img.Reset();

	GRLOpenGLInit(800, 600, "Grassland Project 1", false);
	//va->BindIndicesData(indices, 36, GRL_OPENGL_BUFFER_USAGE_STATIC);

	GRLOpenGLSetDepthTestState(true);
	GRLOpenGLSetVSyncState(false);

	if (GRLOpenGL2DInit())
	{
		GRLSetErrorInfo("Grassland OpenGL 2D Library Init Failed!");
	}

	//glfwSetWindowSizeCallback(Graphics::OpenGL::GetGLFWWindow(), ResizeCallBack);
	GRLPtr<GRLIImage> pImage;
	GRLCreateImage(800, 600, &pImage);
	pImage->LoadBMP("earth.bmp");
	//pImage->StoreBMP("earth_copy.bmp");
	puts("Bitmap Loaded");

	GRLPtr<GRLIOpenGLTexture> pTexture;
	GRLCreateOpenGLTextureFromImage(pImage.Get(), &pTexture);

	std::queue<double> frame_tp_Q;
	std::chrono::steady_clock::time_point start_tp = std::chrono::steady_clock::now();

	float H = 0.0, S = 1.0, V = 1.0;
	GRLOpenGLSetDepthTestState(false);
	while (!GRLOpenGLShouldClose())
	{
		H += 0.001;
		GRLColor bkcolor = Graphics::Util::HSV_to_RGB(H, S, V);
		GRLOpenGLBindFrameBuffer(0);
		GRLOpenGLSetClearColor(bkcolor);
		GRLOpenGLClear();

		//GRLOpenGL2DPutImage(0.0f, 0.0f, 1.0f, 1.0f, pTexture.Get());


		GRLOpenGL2DPutImage(0, 0, pTexture.Get());
		GRLOpenGL2DSetDrawColor(GRLColor(0.7, 0.2, 0.7, 0.7));
		GRLOpenGL2DDrawCircle(0, 0, 100);
		GRLOpenGL2DDrawTriangle(80, 60, 100, 200, 200, 100);
		GRLOpenGL2DDrawRectangle(200, 200, 300, 300);
		
		GRLOpenGLSwapBuffers();
		GRLOpenGLPollEvents();
		//puts("Running");
		static std::chrono::steady_clock::time_point lasttp = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point this_frame = std::chrono::steady_clock::now();
		auto period = this_frame - lasttp;
		double stampd = (this_frame - start_tp).count() * 1e-9;
		frame_tp_Q.push(stampd);
		while (frame_tp_Q.size() > 100)
			frame_tp_Q.pop();
		double durationd = stampd - frame_tp_Q.front();
		static int outcnt = 0;
		outcnt++;
		if (durationd > Math::epsd && outcnt == 1000)
		{
			outcnt = 0;
			std::cout << 100.0 / durationd << "fps" << std::endl;
		}

		//std::cout << std::chrono::milliseconds(1000) / period << std::endl;
		lasttp = this_frame;
	}
	;
	GRLOpenGLTerminate();//*/
}