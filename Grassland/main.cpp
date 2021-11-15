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

/*这一段是用于正方体模型的*/
GRLVec3 block[8] =
{
	Grassland::Math::Vec3(-1.0, -1.0, -1.0),
	Grassland::Math::Vec3(-1.0, -1.0,  1.0),
	Grassland::Math::Vec3(-1.0,  1.0, -1.0),
	Grassland::Math::Vec3(-1.0,  1.0,  1.0),
	Grassland::Math::Vec3(1.0, -1.0, -1.0),
	Grassland::Math::Vec3(1.0, -1.0,  1.0),
	Grassland::Math::Vec3(1.0,  1.0, -1.0),
	Grassland::Math::Vec3(1.0,  1.0,  1.0)
};
GRLVec3 vertices[8][2] = {};
uint32_t indices[] = {
	0b000, 0b011, 0b001,
	0b000, 0b010, 0b011,
	0b100, 0b101, 0b111,
	0b100, 0b111, 0b110,
	0b000, 0b001, 0b101,
	0b000, 0b101, 0b100,
	0b010, 0b111, 0b011,
	0b010, 0b110, 0b111,
	0b000, 0b110, 0b010,
	0b000, 0b100, 0b110,
	0b001, 0b011, 0b111,
	0b001, 0b111, 0b101
};


float tex_vertices[] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f
};
uint32_t tex_indices[] = { 0,2,1,1,2,3 };

int g_Width = 800, g_Height = 600;
bool g_WinSizeChanged = false;

void ResizeCallBack(GLFWwindow* window, int width, int height)
{
	g_Width = width;
	g_Height = height;
	g_WinSizeChanged = true;
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	std::cout << "mouse move: " << xpos << " " << ypos << std::endl;
}

void KeyCallback(GLFWwindow* window, int gl_key, int scannode, int action, int mods)
{
	std::cout << "key act: " << gl_key << " " << scannode << " " << action << " " << mods << std::endl;
	if (gl_key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (gl_key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

bool g_MouseNormal = true;
bool g_block_mouse = true;

uint64_t WinMsgHandler(uint64_t winid, uint64_t msg, uint64_t param0, uint64_t param1)
{
	//std::cout << msg << " " << param0 << " " << param1 << std::endl;
	switch (msg)
	{
	case GRL_WM_SIZE:
		g_Width = param0;
		g_Height = param1;
		g_WinSizeChanged = true;
		break;
	case GRL_WM_KEYDOWN:
		if (param0 == GRL_KEY_LSHIFT)
		{
			glfwSetInputMode(GRLOpenGLGetWindow(), GLFW_CURSOR, g_MouseNormal ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
			g_MouseNormal = !g_MouseNormal;
			g_block_mouse = true;
		}
		break;
	}
	return 0;
}

class MeshBuffer
{
public:
	GRLVec3* vbuffer;
	uint32_t* ibuffer;
	int32_t vcnt;
	int32_t icnt;
}mesh_buffer;

int main()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "fonts/STKAITI.TTF", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, L'齉', FT_LOAD_RENDER))
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	GRLPtr<GRLIImage> text_img;
	GRLCreateImage(face->glyph->bitmap.width, face->glyph->bitmap.rows, &text_img);

	GRLColor* text_img_buffer;
	text_img->GetImageBuffer(&text_img_buffer);
	for (int i = 0; i < text_img->GetWidth() * text_img->GetHeight(); i++)
	{
		float greyness = face->glyph->bitmap.buffer[i] * 1.0f / 255.0f;
		text_img_buffer[i] = GRLColor(greyness, greyness, greyness);
	}
	text_img->StoreBMP("text.bmp");
	text_img.Reset();

	GRLOpenGLInit(800, 600, "Grassland Project 1", false);
	mesh_buffer.vbuffer = new GRLVec3[21*21*21*8*2];
	mesh_buffer.ibuffer = new uint32_t[21*21*21*36];

	//glfwSetKeyCallback(GRLOpenGLGetWindow(), KeyCallback);
	//glfwSetCursorPosCallback(GRLOpenGLGetWindow(), CursorPosCallback);
	GRLOpenGLSetWindowProc(WinMsgHandler);

	/*int max_size;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
	std::cout << max_size << std::endl;
	GRLOpenGLTerminate();
	return 0;//*/

	//Graphics::OpenGL::Texture texture(800, 600, GRL_OPENGL_TEXTURE_FORMAT_RGB);
	//Graphics::OpenGL::Texture texture2(800, 600, GRL_OPENGL_TEXTURE_FORMAT_RGB);
	//Graphics::OpenGL::Texture depthmap(800, 600, GRL_OPENGL_TEXTURE_FORMAT_DEPTH);

	GRLPtr<GRLIOpenGLTexture> texture, texture2, depthmap;
	GRLCreateOpenGLTexture(800, 600, GRL_OPENGL_TEXTURE_FORMAT_RGBA, nullptr, &texture);
	GRLCreateOpenGLTexture(800, 600, GRL_OPENGL_TEXTURE_FORMAT_RGB, nullptr, &texture2);
	GRLCreateOpenGLTexture(800, 600, GRL_OPENGL_TEXTURE_FORMAT_DEPTH, nullptr, &depthmap);

	GRLPtr<GRLIImage> img;
	GRLCreateImage(800, 600, &img);

	GRLPtr<GRLIOpenGLFrameBuffer> framebuffer;
	GRLCreateOpenGLFrameBuffer(800, 600, &framebuffer);

	//GRLGLCall(glNamedFramebufferTexture(hFrameBuffer, GL_COLOR_ATTACHMENT0, texture->GetHandle(), 0));
	//GRLGLCall(glNamedFramebufferTexture(hFrameBuffer, GL_COLOR_ATTACHMENT1, texture2->GetHandle(), 0));
	//GRLGLCall(glNamedFramebufferTexture(hFrameBuffer, GL_DEPTH_ATTACHMENT, depthmap->GetHandle(), 0));
	//glNamedFramebufferTexture(hFrameBuffer, GL_DEPTH_ATTACHMENT, depthmap->GetHandle(), 0);
	framebuffer->BindTexture(texture.Get(), GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR0);
	framebuffer->BindTexture(texture2.Get(), GRL_OPENGL_FRAMEBUFFER_SLOT_COLOR1);
	framebuffer->BindTexture(depthmap.Get(), GRL_OPENGL_FRAMEBUFFER_SLOT_DEPTH);

	float H = 0.0, S = 1.0, V = 1.0;
	const int coe = (int)'-';

	GRLPtr<GRLIOpenGLProgram> pProgram, pOutImgProgram;

	GRLCreateOpenGLProgramFromSourceFile(
		"shaders/VertexShader.glsl",
		"shaders/FragmentShader.glsl",
		nullptr,
		&pProgram
	);
	GRLCreateOpenGLProgramFromSourceFile(
		"shaders/VertexShaderTex.glsl",
		"shaders/FragmentShaderTex.glsl",
		nullptr,
		&pOutImgProgram
	);


	GRLPtr<GRLIOpenGLVertexArray> va, vatex;
	GRLCreateOpenGLVertexArray(&va);
	GRLCreateOpenGLVertexArray(&vatex);
	va->ActiveVerticesLayout(0, 3, GRL_TYPE_FLOAT, 6 * 4, 0);
	va->ActiveVerticesLayout(1, 3, GRL_TYPE_FLOAT, 6 * 4, 12);
	vatex->ActiveVerticesLayout(0, 3, GRL_TYPE_FLOAT, 20, 0);
	vatex->ActiveVerticesLayout(1, 2, GRL_TYPE_FLOAT, 20, 12);
	vatex->BindVerticesData(tex_vertices, 20, GRL_OPENGL_BUFFER_USAGE_STATIC);
	//va->BindVerticesData(vertices, 8 * 2 * 3, GRL_OPENGL_BUFFER_USAGE_STREAM);
	vatex->BindIndicesData(tex_indices, 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
	//va->BindIndicesData(indices, 36, GRL_OPENGL_BUFFER_USAGE_STATIC);


	Grassland::Math::Mat3x3 mat_block(1.0);
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(0);

	pOutImgProgram->SetInt("texture1", 1);
	pOutImgProgram->SetInt("texture0", 0);

	//glfwSetWindowSizeCallback(Graphics::OpenGL::GetGLFWWindow(), ResizeCallBack);

	GRLCameraNormal camera(GRLRadian(60.0f), 800.0f / 600.0f, 1.0f, 100.0f);
	camera.LookAt(GRLVec3(0.0, 0.0, -5.0), GRLVec3(0.0, 0.0, 0.0));

	float pitch = 0.0, yaw = 0.0, roll = 0.0;
	GRLVec4 origin = GRLVec4(0.0, 0.0, -5.0, 1.0);

	GRLMat4 RMat(1.0), MMat(1.0);

	GRLMat4 projMat = GRLTransformProjection(GRLRadian(60.0f), (float)g_Width / (float)g_Height, 1.0f, 300.0f);

	double lastx = 0;
	double lasty = 0;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	 
	std::queue<double> frame_tp_Q;

	std::chrono::steady_clock::time_point start_tp = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(Graphics::OpenGL::GetGLFWWindow()))
	{
		H += 0.001;

		GRLColor color = 
			//GRLColor(0.6, 0.7, 0.8);
			Graphics::Util::HSV_to_RGB(H, S, V);

		GRLOpenGLBindFrameBuffer(framebuffer.Get());
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.6, 0.7, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pProgram->Use();
		pProgram->SetMat4("gMatrix", projMat * (MMat * RMat).inverse());
		MMat = GRLTransformTranslate(origin[0], origin[1], origin[2]);
		
		//pProgram->SetMat4("gMatrix", camera.GetShaderMatrix());
		/*pProgram->SetMat4("gMatrix", GRLTransformProjection(
			GRLRadian(60.0f),
			(float)g_Width / (float)g_Height,
			2.0f,
			100.0f
		) * GRLTransformLookAt(GRLVec3(5.0f, 1.0f, -5.0f), GRLVec3(0.0,0.0,0.0)).inverse());*/
		mat_block *= GRLMat3(GRLTransformRotation(GRLRadian(0.1f), GRLRadian(0.0f), GRLRadian(0.0f)));
		for (int i = 0; i < 8; i++)
		{
			vertices[i][0] = mat_block * block[i];
			vertices[i][1] = block[i] * 0.5 + 0.5;
		}
		mesh_buffer.icnt = 0;
		mesh_buffer.vcnt = 0;
		for (int x = -10; x <= 10; x++)
			for (int y = -10; y <= 10; y++)
				for (int z = -10; z <= 10; z++)
				{
					float fx = x * 10.0, fy = y * 10.0, fz = z * 10.0;
					for (int i = 0; i < 36; i++)
					{
						mesh_buffer.ibuffer[mesh_buffer.icnt++] = indices[i] + mesh_buffer.vcnt;
					}
					for (int i = 0; i < 8; i++)
					{
						mesh_buffer.vbuffer[mesh_buffer.vcnt * 2] = vertices[i][0] + GRLVec3(fx, fy, fz);
						mesh_buffer.vbuffer[mesh_buffer.vcnt * 2 + 1] = vertices[i][1];
						mesh_buffer.vcnt++;
					}
						
				}
		va->BindVerticesData(mesh_buffer.vbuffer, 8 * 2 * 3 * 21 * 21 * 21, GRL_OPENGL_BUFFER_USAGE_DYNAMIC);
		va->BindIndicesData(mesh_buffer.ibuffer, 36 * 21 * 21 * 21, GRL_OPENGL_BUFFER_USAGE_DYNAMIC);

		//va->BindIndicesData(indices, 6 * 6, GRL_OPENGL_BUFFER_USAGE_DYNAMIC);
		va->Render();

		static int last_press = 0;
		if (glfwGetKey(Grassland::Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			if (!last_press)
			{
				texture->GetImage(img.Get());
				img->StoreBMP("texture.bmp");
			}
			last_press = 1;
		}
		else last_press = 0;

		//depthmap->GetImage(img.Get());
		//img->StoreBMP("texture.bmp");

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, g_Width, g_Height);

		GRLOpenGLBindFrameBuffer(nullptr);

		//glDisable(GL_DEPTH_TEST);
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pOutImgProgram->Use();
		texture2->BindTexture(1);
		texture->BindTexture(0);
		//depthmap->BindTexture(1);
		

		vatex->Render();
		glfwSwapBuffers(Graphics::OpenGL::GetGLFWWindow());
		glfwPollEvents();
		if (g_WinSizeChanged)
		{
			framebuffer->Resize(g_Width, g_Height);
			camera.SetFOV(GRLRadian(60.0f), (float)g_Width / (float)g_Height, 1.0f, 100.0f);
			projMat = GRLTransformProjection(GRLRadian(60.0f), (float)g_Width / (float)g_Height, 1.0f, 300.0f);
			g_WinSizeChanged = false;
		}

		static std::chrono::steady_clock::time_point lasttp = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point this_frame = std::chrono::steady_clock::now();
		auto period = this_frame - lasttp;
		double stampd = (this_frame - start_tp).count() * 1e-9;
		frame_tp_Q.push(stampd);
		while (frame_tp_Q.size() > 100)
			frame_tp_Q.pop();
		double durationd = stampd - frame_tp_Q.front();
		if (durationd > Math::epsd)
		{
			std::cout <<  100.0 / durationd  << "fps" << std::endl;
		}

		//std::cout << std::chrono::milliseconds(1000) / period << std::endl;
		lasttp = this_frame;

		float dx = 0.0, dy = 0.0, dz = 0.0;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_W)) dz += 30;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_S)) dz -= 30;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_A)) dx -= 30;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_D)) dx += 30;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_R)) dy += 30;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_F)) dy -= 30;
		double xpos, ypos;
		glfwGetCursorPos(GRLOpenGLGetWindow(), &xpos, &ypos);
		//std::cout << period.count() << " " << std::chrono::nanoseconds(1000000000).count() << std::endl;
		dx *= period.count()* 1e-9;
		dy *= period.count() * 1e-9;
		dz *= period.count() * 1e-9;

		if (!g_block_mouse && !g_MouseNormal)
		{
			//std::cout << xpos << " " << ypos << std::endl;
			pitch += (ypos - lasty) * GRLRadian(0.1f);
			yaw -= (xpos - lastx) * GRLRadian(0.1f);
			if (pitch > GRLRadian(90.0f)) pitch = GRLRadian(90.0f);
			if (pitch < GRLRadian(-90.0f)) pitch = GRLRadian(-90.0f);
			while (yaw > GRLRadian(180.0f)) yaw -= GRLRadian(360.0f);
			while (yaw < GRLRadian(-180.0f)) yaw += GRLRadian(360.0f);
		}
		lastx = xpos;
		lasty = ypos;
		g_block_mouse = false;

		RMat = GRLTransformRotation(pitch, yaw, roll);
		origin = origin + RMat * GRLVec4(dx, dy, dz, 0.0);
	}
	;
	GRLOpenGLTerminate();//*/
}