#include <iostream>
#include <Grassland.h>
#include <ctime>
#include <queue>
#include <string>
#include <random>
#include <cmath>

#include <iostream>

float g_aspect = 800.0 / 600.0;
int32_t gWidth = 800, gHeight = 600;
int32_t gWidthNow = 800, gHeightNow = 600;

void glfwWindowResizeCallBack(GLFWwindow* window, int32_t width, int32_t height)
{
    gWidth = width;
    gHeight = height;
    glViewport(0, 0, width, height);
    g_aspect = (float)width / height;
}

int main()
{
    GRG::Bitmap img(640, 480);
    img.Clear(GRG::BitmapPixel(0xFF * 0.7, 0xFF * 0.8, 0xFF * 0.9));
    img.SaveBitmapToFile("img.bmp");
    Grassland::Graphics::GL::Window window;
    Grassland::Graphics::GL::Initialize(800, 600, "Grassland", &window);


    glfwSetFramebufferSizeCallback(window.GetGLFWWindowHandle(), glfwWindowResizeCallBack);

    Grassland::Math::Mat4x4 translate(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, -5.0,
        0.0, 0.0, 0.0, 1.0
    );

    Grassland::Math::Vec3 block[8] =
    {
        Grassland::Math::Vec3(-1.0, -1.0, -1.0),
        Grassland::Math::Vec3(-1.0, -1.0,  1.0),
        Grassland::Math::Vec3(-1.0,  1.0, -1.0),
        Grassland::Math::Vec3(-1.0,  1.0,  1.0),
        Grassland::Math::Vec3( 1.0, -1.0, -1.0),
        Grassland::Math::Vec3( 1.0, -1.0,  1.0),
        Grassland::Math::Vec3( 1.0,  1.0, -1.0),
        Grassland::Math::Vec3( 1.0,  1.0,  1.0)
    };
    Grassland::Math::Vec3 vertices[8][2] = {};

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

    float texSquare[] = {
        -1.0, -0.9, 0.0, 0.0, 0.0,
        -1.0,  0.9, 0.0, 0.0, 1.0,
         1.0, -0.9, 0.0, 1.0, 0.0,
         1.0,  0.9, 0.0, 1.0, 1.0
    };

    uint32_t texIndices[] = {
        0, 1, 2,
        1, 2, 3
    };
    uint32_t hIndexBufferTex;
    uint32_t hVertexBufferTex;
    uint32_t hVertexArrayTex;

    glGenVertexArrays(1, &hVertexArrayTex);
    glBindVertexArray(hVertexArrayTex);
    glGenBuffers(1, &hVertexBufferTex);
    glGenBuffers(1, &hIndexBufferTex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBufferTex);
    glBindBuffer(GL_ARRAY_BUFFER, hVertexBufferTex);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texIndices), texIndices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texSquare), texSquare, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)12);
    glBindVertexArray(0);


    uint32_t hIndexBuffer;
    uint32_t hVertexBuffer;
    uint32_t hVertexArray;

    glGenVertexArrays(1, &hVertexArray);
    glBindVertexArray(hVertexArray);
    glGenBuffers(1, &hVertexBuffer);
    glGenBuffers(1, &hIndexBuffer);


    glBindBuffer(GL_ARRAY_BUFFER, hVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    GRG::GL::Shader vertexShader, fragmentShader;
    GRG::GL::Program shaderProgram;
    vertexShader.CompileFromFile("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
    fragmentShader.CompileFromFile("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
    shaderProgram.Init();
    shaderProgram.Attach(vertexShader);
    shaderProgram.Attach(fragmentShader);
    shaderProgram.Link();
    vertexShader.Release();
    fragmentShader.Release();

    GRG::GL::Shader vertexShaderTex, fragmentShaderTex;
    GRG::GL::Program shaderProgramTex;
    vertexShaderTex.CompileFromFile("shaders/VertexShaderTex.glsl", GL_VERTEX_SHADER);
    fragmentShaderTex.CompileFromFile("shaders/FragmentShaderTex.glsl", GL_FRAGMENT_SHADER);
    shaderProgramTex.Init();
    shaderProgramTex.Attach(vertexShaderTex);
    shaderProgramTex.Attach(fragmentShaderTex);
    shaderProgramTex.Link();
    vertexShaderTex.Release();
    fragmentShaderTex.Release();

    GRG::GL::FrameBuffer frame_buffer;
    frame_buffer.Init(200, 150);
    //glReadBuffer(GL_NONE);

    


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

    glfwSwapInterval(1);
    GRG::GL::SetBlendState(1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!window.ShouldClose())
    {
        if (glfwGetKey(window.GetGLFWWindowHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window.GetGLFWWindowHandle(), true);

        //if (glfwGetKey(window.GetGLFWWindowHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)

        Grassland::Math::Mat4x4 UniformMat = Grassland::Graphics::TransformProjection(
            Grassland::Math::radian(60.0),
            g_aspect,
            2.0,
            10.0
        ) * translate.inverse();
        glUseProgram(0);


        shaderProgram.SetMat4("gMatrix", UniformMat);
        shaderProgram.Use();

        mat_block *= pitch * yaw * roll;
        for (int i = 0; i < 8; i++)
        {
            vertices[i][0] = mat_block*block[i];
            vertices[i][1] = block[i] * 0.5 + 0.5;
        }

        if (gWidth != gWidthNow || gHeight != gHeightNow)
        {
            /*frame_buffer.Release();
            frame_buffer.Init(gWidth, gHeight); //*/
            frame_buffer.Resize(gWidth * 2, gHeight * 2);

            gWidthNow = gWidth;
            gHeightNow = gHeight;
        }

        /* Render here */
        GRG::GL::UseScreenFrame();
        frame_buffer.Use();
        GRG::GL::SetClearColor(0.7, 0.6, 0.5, 0.0);
        GRG::GL::ClearColorBuffer();
        GRG::GL::ClearDepthBuffer();

        glBindVertexArray(hVertexArray);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
        glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);

        GRG::GL::UseScreenFrame();
        shaderProgramTex.SetMat4("gMatrix", GRM::Mat4(1.0));
        shaderProgramTex.Use();
        GRG::GL::SetDepthTestState(false);
        GRG::GL::SetClearColor(0.7, 0.8, 1.0, 1.0);
        GRG::GL::ClearColorBuffer();
        glBindVertexArray(hVertexArrayTex);

        glBindTexture(GL_TEXTURE_2D, frame_buffer.GetColorTextureHandle());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//*/

        /* Swap front and back buffers */
        //glfwSwapBuffers(window.GetGLFWWindowHandle());
        window.Present();

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
}