#include <Grassland.h>

#include <Windows.h>
#include <d3d12.h>
#include <memory>

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>

using namespace Grassland;


int main()
{
	GRLOpenGLInit(800, 600, "Grassland Project 1", false);
	GRLOpenGLSetDepthTestState(true);
	GRLOpenGLSetVSyncState(false);

	if (GRLOpenGL2DInit())
	{
		GRLSetErrorInfo("Grassland OpenGL 2D Library Init Failed!");
		return 0;
	}
	if (GRLOpenGLTextInit(
		"fonts/consola.ttf",
		"fonts/msyh.ttc",
		0, 32))
	{
		GRLSetErrorInfo("Grassland OpenGL Text Library Init Failed!");
		return 0;
	}

	GRLPtr<GRLIImage> pImage;
	GRLCreateImage(800, 600, &pImage);
	pImage->LoadBMP("earth.bmp");
	puts("Bitmap Loaded");
	GRLPtr<GRLIOpenGLTexture> pTexture;
	GRLCreateOpenGLTextureFromImage(pImage.Get(), &pTexture);

	std::queue<double> frame_tp_Q;
	std::chrono::steady_clock::time_point start_tp = std::chrono::steady_clock::now();

	float H = 0.0, S = 1.0, V = 1.0;
	GRLOpenGLSetDepthTestState(false);
	while (!GRLOpenGLShouldClose())
	{
		H += 0.0001;
		GRLColor bkcolor = Graphics::Util::HSV_to_RGB(H, S, V, 0.7);
		GRLOpenGLBindFrameBuffer(0);
		GRLOpenGLSetClearColor(bkcolor);
		GRLOpenGLClear();

		//GRLOpenGL2DPutImage(0.0f, 0.0f, 1.0f, 1.0f, pTexture.Get());


		GRLOpenGL2DPutImage(0, 0, pTexture.Get());
		GRLOpenGL2DSetDrawColor(GRLColor(0.7, 0.2, 0.7, 0.7));
		GRLOpenGL2DDrawCircle(0, 0, 100);
		GRLOpenGL2DDrawTriangle(80, 60, 100, 200, 200, 100);
		GRLOpenGL2DDrawRectangle(200, 200, 300, 300);
		GRLOpenGLTextSetColor(bkcolor);
		GRLOpenGL2DDrawRectangle(0, 0, 800, 64);
		GRLOpenGLTextRender(0, 500, u8"Hello,World! ちとすすゆすめたかうあ��");
		
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