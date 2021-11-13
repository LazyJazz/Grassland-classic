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

using namespace Grassland;



GRLMat4 TransformProjection(float FOVangle, float aspect, float Near, float Far)
{
	float INVtanFOVangle = 1.0 / tan(FOVangle * 0.5);
	return GRLMat4(
		INVtanFOVangle / aspect, 0.0, 0.0, 0.0,
		0.0, INVtanFOVangle, 0.0, 0.0,
		0.0, 0.0, Far / (Far - Near), -(Near * Far) / (Far - Near),
		0.0, 0.0, 1.0, 0.0
	);
}

GRLMat4 TransformTranslate(float x, float y, float z)
{
	return GRLMat4(
		1.0, 0.0, 0.0, x,
		0.0, 1.0, 0.0, y,
		0.0, 0.0, 1.0, z,
		0.0, 0.0, 0.0, 1.0
	);
}

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
	-1.0f, -0.9f, 0.0f, 0.0f, 0.0f,
	-1.0f, 0.9f, 0.0f, 0.0f, 1.0f,
	1.0f, -0.9f, 0.0f, 1.0f, 0.0f,
	1.0f, 0.9f, 0.0f, 1.0f, 1.0f
};
uint32_t tex_indices[] = { 0,1,2,3,1,2 };

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

uint64_t WinMsgHandler(uint64_t winid, uint64_t msg, uint64_t param0, uint64_t param1)
{
	std::cout << msg << " " << param0 << " " << param1 << std::endl;
	switch (msg)
	{
	case GRL_WM_SIZE:
		g_Width = param0;
		g_Height = param1;
		g_WinSizeChanged = true;
		break;
	}
	return 0;
}

int main()
{
	auto aplusb = [](int a, int b) {return a + b; };
	std::cout << aplusb(1,2) << std::endl;
	GRLOpenGLInit(800, 600, "Grassland Project 1", false);

	//glfwSetKeyCallback(GRLOpenGLGetWindow(), KeyCallback);
	//glfwSetCursorPosCallback(GRLOpenGLGetWindow(), CursorPosCallback);
	GRLOpenGLSetWindowProc(WinMsgHandler);

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

	uint32_t hFrameBuffer;

	glGenFramebuffers(1, &hFrameBuffer);

	//GRLGLCall(glNamedFramebufferTexture(hFrameBuffer, GL_COLOR_ATTACHMENT0, texture->GetHandle(), 0));
	//GRLGLCall(glNamedFramebufferTexture(hFrameBuffer, GL_COLOR_ATTACHMENT1, texture2->GetHandle(), 0));
	//GRLGLCall(glNamedFramebufferTexture(hFrameBuffer, GL_DEPTH_ATTACHMENT, depthmap->GetHandle(), 0));
	//glNamedFramebufferTexture(hFrameBuffer, GL_DEPTH_ATTACHMENT, depthmap->GetHandle(), 0);
	uint32_t drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetHandle(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture2->GetHandle(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap->GetHandle(), 0);
	glDrawBuffers(2, (GLenum*)drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	va->ActiveVerticesLayout(0, 3, 6, 0);
	va->ActiveVerticesLayout(1, 3, 6, 3);
	vatex->ActiveVerticesLayout(0, 3, 5, 0);
	vatex->ActiveVerticesLayout(1, 2, 5, 3);
	vatex->BindVerticesData(tex_vertices, 20, GRL_OPENGL_BUFFER_USAGE_STATIC);
	va->BindVerticesData(vertices, 8 * 2 * 3, GRL_OPENGL_BUFFER_USAGE_STREAM);
	vatex->BindIndicesData(tex_indices, 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
	va->BindIndicesData(indices, 36, GRL_OPENGL_BUFFER_USAGE_STATIC);


	float dpitch = 0.0, dyaw = 0.0, droll = 0.0;

	dpitch = Grassland::Math::radian(0.1);
	dyaw = Grassland::Math::radian(0.21);
	droll = Grassland::Math::radian(0.32);


	Grassland::Math::Mat3x3
		pitch(
			cos(dpitch), -sin(dpitch), 0.0,
			sin(dpitch), cos(dpitch), 0.0,
			0.0, 0.0, 1.0
		)
		, yaw(
			1.0, 0.0, 0.0,
			0.0, cos(dyaw), -sin(dyaw),
			0.0, sin(dyaw), cos(dyaw)
		), roll(
			cos(droll), 0.0, -sin(droll),
			0.0, 1.0, 0.0,
			sin(droll), 0.0, cos(droll)
		), mat_block(1.0);
	glEnable(GL_DEPTH_TEST);
	//glfwSwapInterval(1);
	glfwSwapInterval(1);
	std::random_device rand_dev;
	std::uniform_int_distribution<> distr_int(1, 6);

	pOutImgProgram->SetInt("texture1", 1);
	pOutImgProgram->SetInt("texture0", 0);

	//glfwSetWindowSizeCallback(Graphics::OpenGL::GetGLFWWindow(), ResizeCallBack);

	GRLCameraNormal camera(GRLRadian(60.0f), 800.0f / 600.0f, 1.0f, 100.0f);
	camera.LookAt(GRLVec3(0.0, 0.0, -5.0), GRLVec3(0.0, 0.0, 0.0));

	while (!glfwWindowShouldClose(Graphics::OpenGL::GetGLFWWindow()))
	{
		int state = glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_E);
		H += 0.001;

		//glClear();

		GRLColor color = 
			//GRLColor(0.6, 0.7, 0.8);
			Graphics::Util::HSV_to_RGB(H, S, V);

		GRLOpenGLBindFrameBuffer(framebuffer.Get());
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.6, 0.7, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pProgram->Use();
		pProgram->SetMat4("gMatrix", camera.GetShaderMatrix());
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
		va->BindVerticesData(vertices, 8 * 2 * 3, GRL_OPENGL_BUFFER_USAGE_STATIC);

		va->BindIndicesData(indices, 6 * 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
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
		texture2->BindTexture(0);
		texture->BindTexture(1);

		vatex->Render();
		glfwSwapBuffers(Graphics::OpenGL::GetGLFWWindow());
		glfwPollEvents();
		if (g_WinSizeChanged)
		{
			framebuffer->Resize(g_Width, g_Height);
			camera.SetFOV(GRLRadian(60.0f), (float)g_Width / (float)g_Height, 1.0f, 100.0f);
			g_WinSizeChanged = false;
		}

		float x = 0.0, y = 0.0, z = 0.0;
		float roll = 0.0;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_W)) z += 0.1;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_S)) z -= 0.1;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_A)) x -= 0.1;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_D)) x += 0.1;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_R)) y += 0.1;
		if (glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_F)) y -= 0.1;
		if (glfwGetMouseButton(Graphics::OpenGL::GetGLFWWindow(), GLFW_MOUSE_BUTTON_LEFT)) roll -= GRLRadian(0.1);
		if (glfwGetMouseButton(Graphics::OpenGL::GetGLFWWindow(), GLFW_MOUSE_BUTTON_RIGHT)) roll += GRLRadian(0.1);

		camera.Translate(x, y, z);
		camera.Rotation(0.0, 0.0, roll);
	}
	;
	GRLOpenGLTerminate();//*/
}