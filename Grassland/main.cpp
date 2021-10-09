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

    uint32_t hIndexBuffer;
    uint32_t hVertexBuffer;
    uint32_t hVertexArray;

    glGenBuffers(1, &hVertexBuffer);
    glGenBuffers(1, &hIndexBuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &hVertexArray);
    glBindVertexArray(hVertexArray);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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
    

    glEnable(GL_DEPTH_TEST);

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
            1.0,
            20.0
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
        glClearColor(0.7, 0.8, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glBindBuffer(GL_ARRAY_BUFFER, hVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
        glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);
        /* Swap front and back buffers */
        glfwSwapBuffers(window.GetGLFWWindowHandle());

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}