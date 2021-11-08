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


int main()
{
	auto aplusb = [](int a, int b) {return a + b; };
	std::cout << aplusb(1,2) << std::endl;
	GRLOpenGLInit(800, 600, "Grassland Project 1", false);
	float H = 0.0, S = 1.0, V = 1.0;

	uint32_t hVertexBuffer;
	uint32_t hElementBuffer;
	uint32_t hVertexArrayObject;

	glGenBuffers(1, &hVertexBuffer);
	glGenBuffers(1, &hElementBuffer);
	glGenVertexArrays(1, &hVertexArrayObject);

	glBindVertexArray(hVertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, hVertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hElementBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	glBindVertexArray(0);

	GRLPtr<GRLIOpenGLProgram> pProgram;

	GRLCreateProgramFromSourceFile(
		"shaders/VertexShader.glsl",
		"shaders/FragmentShader.glsl",
		nullptr,
		&pProgram
	);

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
		0b000, 0b011, 0b010,
		0b100, 0b111, 0b101,
		0b100, 0b111, 0b110,
		0b000, 0b101, 0b001,
		0b000, 0b101, 0b100,
		0b010, 0b111, 0b011,
		0b010, 0b111, 0b110,
		0b000, 0b110, 0b010,
		0b000, 0b110, 0b100,
		0b001, 0b111, 0b011,
		0b001, 0b111, 0b101
	};

	Graphics::OpenGL::VertexArray vertex_array;
	vertex_array.BindIndicesData(indices, 36, GRL_OPENGL_BUFFER_USAGE_STREAM);
	vertex_array.ActiveVerticesLayout(0, 3, 6, 0);
	vertex_array.ActiveVerticesLayout(1, 3, 6, 3);
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
	while (!glfwWindowShouldClose(Graphics::OpenGL::GetGLFWWindow()))
	{
		int state = glfwGetKey(Graphics::OpenGL::GetGLFWWindow(), GLFW_KEY_E);
		H += 0.001;

		//glClear();

		GRLColor color = 
			//GRLColor(0.6, 0.7, 0.8);
			Graphics::Util::HSV_to_RGB(H, S, V);

		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pProgram->Use();
		pProgram->SetMat4("gMatrix", TransformProjection(
			Grassland::Math::radian(60.0),
			4.0f/3.0f,
			2.0,
			10.0
		) * TransformTranslate(0.0, 0.0, 5.0));
		mat_block *= pitch * yaw * roll;
		for (int i = 0; i < 8; i++)
		{
			vertices[i][0] = mat_block * block[i];
			vertices[i][1] = block[i] * 0.5 + 0.5;
		}
		vertex_array.BindVerticesData(&vertices[0][0][0], 8*2*3, GRL_OPENGL_BUFFER_USAGE_DYNAMIC);

		vertex_array.Render();

		glfwSwapBuffers(Graphics::OpenGL::GetGLFWWindow());
		glfwPollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	GRLOpenGLTerminate();//*/
}
//float g_aspect = 800.0 / 600.0;
//int32_t gWidth = 800, gHeight = 600;
//int32_t gWidthNow = 800, gHeightNow = 600;
//
//void glfwWindowResizeCallBack(GLFWwindow* window, int32_t width, int32_t height)
//{
//    gWidth = width;
//    gHeight = height;
//    glViewport(0, 0, width, height);
//    g_aspect = (float)width / height;
//}
//
//int main()
//{
//    GRG::Bitmap img(800, 600);
//    img.Clear(GRG::BitmapPixel(0xFF * 0.7, 0xFF * 0.8, 0xFF * 0.9));
//    img.SaveBitmapToFile("img.bmp");
//    Grassland::Graphics::GL::Window window;
//    Grassland::Graphics::GL::Initialize(800, 600, "Grassland", &window);
//
//
//    glfwSetFramebufferSizeCallback(window.GetGLFWWindowHandle(), glfwWindowResizeCallBack);
//
//    Grassland::Math::Mat4x4 translate(
//        1.0, 0.0, 0.0, 0.0,
//        0.0, 1.0, 0.0, 0.0,
//        0.0, 0.0, 1.0, -5.0,
//        0.0, 0.0, 0.0, 1.0
//    );
//
//    Grassland::Math::Vec3 block[8] =
//    {
//        Grassland::Math::Vec3(-1.0, -1.0, -1.0),
//        Grassland::Math::Vec3(-1.0, -1.0,  1.0),
//        Grassland::Math::Vec3(-1.0,  1.0, -1.0),
//        Grassland::Math::Vec3(-1.0,  1.0,  1.0),
//        Grassland::Math::Vec3( 1.0, -1.0, -1.0),
//        Grassland::Math::Vec3( 1.0, -1.0,  1.0),
//        Grassland::Math::Vec3( 1.0,  1.0, -1.0),
//        Grassland::Math::Vec3( 1.0,  1.0,  1.0)
//    };
//    Grassland::Math::Vec3 vertices[8][2] = {};
//
//    uint32_t indices[] = {
//        0b000, 0b011, 0b001,
//        0b000, 0b011, 0b010,
//        0b100, 0b111, 0b101,
//        0b100, 0b111, 0b110,
//        0b000, 0b101, 0b001,
//        0b000, 0b101, 0b100,
//        0b010, 0b111, 0b011,
//        0b010, 0b111, 0b110,
//        0b000, 0b110, 0b010,
//        0b000, 0b110, 0b100,
//        0b001, 0b111, 0b011,
//        0b001, 0b111, 0b101
//    };
//
//    float texSquare[] = {
//        -1.0, -0.9, 0.0, 0.0, 0.0,
//        -1.0,  0.9, 0.0, 0.0, 1.0,
//         1.0, -0.9, 0.0, 1.0, 0.0,
//         1.0,  0.9, 0.0, 1.0, 1.0
//    };
//
//    uint32_t texIndices[] = {
//        0, 1, 2,
//        1, 2, 3
//    };
//    uint32_t hIndexBufferTex;
//    uint32_t hVertexBufferTex;
//    uint32_t hVertexArrayTex;
//
//    glGenVertexArrays(1, &hVertexArrayTex);
//    glBindVertexArray(hVertexArrayTex);
//    glGenBuffers(1, &hVertexBufferTex);
//    glGenBuffers(1, &hIndexBufferTex);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBufferTex);
//    glBindBuffer(GL_ARRAY_BUFFER, hVertexBufferTex);
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texIndices), texIndices, GL_STATIC_DRAW);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(texSquare), texSquare, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)12);
//    glBindVertexArray(0);
//
//
//    uint32_t hIndexBuffer;
//    uint32_t hVertexBuffer;
//    uint32_t hVertexArray;
//
//    glGenVertexArrays(1, &hVertexArray);
//    glBindVertexArray(hVertexArray);
//    glGenBuffers(1, &hVertexBuffer);
//    glGenBuffers(1, &hIndexBuffer);
//
//
//    glBindBuffer(GL_ARRAY_BUFFER, hVertexBuffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
//    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//    glEnableVertexAttribArray(0);
//    glEnableVertexAttribArray(1);
//    glBindVertexArray(0);
//
//    GRG::GL::Shader vertexShader, fragmentShader;
//    GRG::GL::Program shaderProgram;
//    vertexShader.CompileFromFile("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
//    fragmentShader.CompileFromFile("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
//    shaderProgram.Init();
//    shaderProgram.Attach(vertexShader);
//    shaderProgram.Attach(fragmentShader);
//    shaderProgram.Link();
//    vertexShader.Release();
//    fragmentShader.Release();
//
//    GRG::GL::Shader vertexShaderTex, fragmentShaderTex;
//    GRG::GL::Program shaderProgramTex;
//    vertexShaderTex.CompileFromFile("shaders/VertexShaderTex.glsl", GL_VERTEX_SHADER);
//    fragmentShaderTex.CompileFromFile("shaders/FragmentShaderTex.glsl", GL_FRAGMENT_SHADER);
//    shaderProgramTex.Init();
//    shaderProgramTex.Attach(vertexShaderTex);
//    shaderProgramTex.Attach(fragmentShaderTex);
//    shaderProgramTex.Link();
//    vertexShaderTex.Release();
//    fragmentShaderTex.Release();
//
//    GRG::GL::FrameBuffer frame_buffer;
//    frame_buffer.Init(800, 600);
//    //glReadBuffer(GL_NONE);
//
//    
//
//
//    float dpitch = 0.0, dyaw = 0.0, droll = 0.0;
//
//    dpitch = Grassland::Math::radian(0.1);
//    dyaw = Grassland::Math::radian(0.21);
//    droll = Grassland::Math::radian(0.32);
//
//
//    Grassland::Math::Mat3x3
//        pitch(
//            cos(dpitch), -sin(dpitch), 0.0,
//            sin(dpitch), cos(dpitch), 0.0,
//            0.0, 0.0, 1.0
//        )
//        , yaw(
//            1.0, 0.0, 0.0,
//            0.0, cos(dyaw), -sin(dyaw),
//            0.0, sin(dyaw), cos(dyaw)
//        ), roll(
//            cos(droll), 0.0, -sin(droll),
//            0.0, 1.0, 0.0,
//            sin(droll), 0.0, cos(droll)
//        ), mat_block(1.0);
//
//    glfwSwapInterval(1);
//    GRG::GL::SetBlendState(1);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    while (!window.ShouldClose())
//    {
//        if (glfwGetKey(window.GetGLFWWindowHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
//            glfwSetWindowShouldClose(window.GetGLFWWindowHandle(), true);
//
//        //if (glfwGetKey(window.GetGLFWWindowHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)
//
//        Grassland::Math::Mat4x4 UniformMat = Grassland::Graphics::TransformProjection(
//            Grassland::Math::radian(60.0),
//            g_aspect,
//            2.0,
//            10.0
//        ) * translate.inverse();
//        glUseProgram(0);
//
//
//        shaderProgram.SetMat4("gMatrix", UniformMat);
//        shaderProgram.Use();
//
//        mat_block *= pitch * yaw * roll;
//        for (int i = 0; i < 8; i++)
//        {
//            vertices[i][0] = mat_block*block[i];
//            vertices[i][1] = block[i] * 0.5 + 0.5;
//        }
//
//        if (gWidth != gWidthNow || gHeight != gHeightNow)
//        {
//            /*frame_buffer.Release();
//            frame_buffer.Init(gWidth, gHeight); //*/
//            frame_buffer.Resize(gWidth, gHeight);
//
//            gWidthNow = gWidth;
//            gHeightNow = gHeight;
//            img.Resize(gWidth, gHeight);
//        }
//
//        /* Render here */
//        GRG::GL::UseScreenFrame();
//        frame_buffer.Use();
//        GRG::GL::SetClearColor(0.7, 0.6, 0.5, 0.0);
//        GRG::GL::ClearColorBuffer();
//        GRG::GL::ClearDepthBuffer();
//
//        glBindVertexArray(hVertexArray);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
//        glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);
//
//        glReadPixels(0, 0, gWidthNow, gHeightNow, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, img.GetColorPtr());
//        img.SaveBitmapToFile("screen_save.bmp");
//
//        GRG::GL::UseScreenFrame();
//        shaderProgramTex.SetMat4("gMatrix", GRM::Mat4(1.0));
//        shaderProgramTex.Use();
//        GRG::GL::SetDepthTestState(false);
//        GRG::GL::SetClearColor(0.7, 0.8, 1.0, 1.0);
//        GRG::GL::ClearColorBuffer();
//        glBindVertexArray(hVertexArrayTex);
//
//        glBindTexture(GL_TEXTURE_2D, frame_buffer.GetColorTextureHandle());
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//*/
//
//        /* Swap front and back buffers */
//        //glfwSwapBuffers(window.GetGLFWWindowHandle());
//        window.Present();
//        //return 0;
//
//        /* Poll for and process events */
//        glfwPollEvents();
//    }
//    glfwTerminate();
//}