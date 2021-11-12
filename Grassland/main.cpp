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

int main()
{
	auto aplusb = [](int a, int b) {return a + b; };
	std::cout << aplusb(1,2) << std::endl;
	GRLOpenGLInit(800, 600, "Grassland Project 1", false);


	uint32_t hFrameBuffer, hColorTex, hColorTex2, hDepthTex;

	glGenFramebuffers(1, &hFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);

	glGenTextures(1, &hColorTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hColorTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,
		800, 600, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hColorTex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &hColorTex2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, hColorTex2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F,
		800, 600, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hColorTex2, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glGenTextures(1, &hDepthTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, hDepthTex);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hDepthTex, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	uint32_t drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, (GLenum*)drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float H = 0.0, S = 1.0, V = 1.0;


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

	std::cout << "texture1: " << pOutImgProgram->GetUniformLocation("texture1") << std::endl;
	std::cout << "texture0: " << pOutImgProgram->GetUniformLocation("texture0") << std::endl;
	pOutImgProgram->SetInt("texture1", 1);
	pOutImgProgram->SetInt("texture0", 0);

	while (!glfwWindowShouldClose(Graphics::OpenGL::GetGLFWWindow()))
	{
		int state = glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_E);
		H += 0.001;

		//glClear();

		GRLColor color = 
			//GRLColor(0.6, 0.7, 0.8);
			Graphics::Util::HSV_to_RGB(H, S, V);

		glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);
		glViewport(0, 0, 800, 600);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.6, 0.7, 0.8, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pProgram->Use();
		pProgram->SetMat4("gMatrix", TransformProjection(
			Grassland::Math::radian(60.0),
			4.0f / 3.0f,
			2.0,
			10.0
		) * TransformTranslate(0.0, 0.0, 5.0));
		mat_block *= pitch * yaw * roll;
		for (int i = 0; i < 8; i++)
		{
			vertices[i][0] = mat_block * block[i];
			vertices[i][1] = block[i] * 0.5 + 0.5;
		}
		va->BindVerticesData(vertices, 8 * 2 * 3, GRL_OPENGL_BUFFER_USAGE_STATIC);

		va->BindIndicesData(indices, 6 * 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
		va->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 800, 600);
		//glDisable(GL_DEPTH_TEST);
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pOutImgProgram->Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hColorTex2);
		//glUniform1i(pOutImgProgram->GetUniformLocation("texture0"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, hColorTex);
		//glUniform1i(pOutImgProgram->GetUniformLocation("texture1"), 1);
		//glActiveTexture(GL_TEXTURE0);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, hColorTex2);

		vatex->Render();
		glfwSwapBuffers(Graphics::OpenGL::GetGLFWWindow());
		glfwPollEvents();
	}
	;
	GRLOpenGLTerminate();//*/
}