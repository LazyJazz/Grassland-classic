#include <iostream>
#include <Grassland.h>
#include <ctime>
#include <queue>
#include <string>
#include <random>
#include <cmath>

#include <iostream>

float g_aspect = 800.0 / 600.0;

void glfwWindowResizeCallBack(GLFWwindow* window, int32_t width, int32_t height)
{
    glViewport(0, 0, width, height);
    g_aspect = (float)width / height;
}

int main()
{
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

    uint32_t hFrameBuffer;
    uint32_t hFrameBufferTex, hFrameBufferDepth;
    glGenFramebuffers(1, &hFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);

    glGenTextures(1, &hFrameBufferTex);
    glGenTextures(1, &hFrameBufferDepth);

    glBindTexture(GL_TEXTURE_2D, hFrameBufferTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hFrameBufferTex, 0);

    glBindTexture(GL_TEXTURE_2D, hFrameBufferDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, hFrameBufferDepth, 0);

    glEnable(GL_DEPTH_TEST);
    //glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << " FrameBuffer Not Complete" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    


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
    while (!glfwWindowShouldClose(window.GetGLFWWindowHandle()))
    {
        if (glfwGetKey(window.GetGLFWWindowHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window.GetGLFWWindowHandle(), true);

        //if (glfwGetKey(window.GetGLFWWindowHandle(), GLFW_KEY_SPACE) == GLFW_PRESS)

        Grassland::Math::Mat4x4 UniformMat = Grassland::Graphics::TransformProjection(
            Grassland::Math::radian(60.0),
            g_aspect,
            3.0,
            7.0
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


        /* Render here */
        glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);
        glViewport(0, 0, 800, 600);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.6, 0.5, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(hVertexArray);


        //glBindBuffer(GL_ARRAY_BUFFER, hVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
        glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        shaderProgramTex.SetMat4("gMatrix", GRM::Mat4(1.0));
        shaderProgramTex.Use();
        glViewport(0, 0, 800, 600);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.7, 0.8, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glBindVertexArray(hVertexArrayTex);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBufferTex);
        //glBindBuffer(GL_ARRAY_BUFFER, hVertexBufferTex);

        glBindTexture(GL_TEXTURE_2D, hFrameBufferDepth);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window.GetGLFWWindowHandle());

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}