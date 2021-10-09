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

uint32_t glCompileShaderFromSourceFile(const char* path, uint32_t glshadertype)
{

    FILE* file = NULL;
    fopen_s(&file, path, "r");
    if (!file)
    {
        std::cout << "Shader File Open Failed: " << path << std::endl;
        return 0;
    }
    uint32_t hShader = glCreateShader(glshadertype);
    char readBuffer[2048] = {};
    std::string shader_code = "";
    while (fread(readBuffer, 1, 2000, file))
    {
        shader_code += readBuffer;
        memset(readBuffer, 0, sizeof(readBuffer));
    }
    fclose(file);
    shader_code += '\0';

    char* shader_source = new char[shader_code.length() + 1];
    memcpy(shader_source, shader_code.c_str(), shader_code.length());
    glShaderSource(hShader, 1, &shader_source, NULL);
    glCompileShader(hShader);

    int32_t success;
    glGetShaderiv(hShader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        int32_t log_length = 0;
        char message[1024];
        glGetShaderInfoLog(hShader, 1024, &log_length, message);
        std::cout << "Shader Compilation Failed:" << std::endl;
        std::cout << message << std::endl;
    }

    delete[] shader_source;
    return hShader;
}

int main2(void)
{

    GLFWwindow* window, *window2;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(512, 512, "Hello World", NULL, NULL);
    window2 = glfwCreateWindow(512, 512, "Window2", NULL, window);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    if (!window2)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Initilize Succeed, lib version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, 512, 512);

    glfwSetFramebufferSizeCallback(window, glfwWindowResizeCallBack);
    //glfwMaximizeWindow(window);

    /* Loop until the user closes the window */



    /* Setting Vertices */
    float vertices[] = {
 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
-0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
-0.25f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };

    for (int i = 0; i < 6; i++)
    {
        float x = cosf(i * std::_Pi / 3.0f), y = sinf(i * std::_Pi / 3.0f);
        vertices[i * 8] = x * 0.5;
        vertices[i * 8 + 1] = y * 0.5;
    }

    uint32_t indices[] = {
        6, 0, 1,
        6, 1, 2,
        6, 2, 3,
        6, 3, 4,
        6, 4, 5,
        6, 5, 0
    };

    uint32_t hIndexBuffer;
    uint32_t hVertexBuffer;
    uint32_t hVertexArray;

    glGenVertexArrays(1, &hVertexArray);
    glGenBuffers(1, &hVertexBuffer);
    glGenBuffers(1, &hIndexBuffer);
    glBindVertexArray(hVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, hVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    uint32_t hTexture;
    glGenTextures(1, &hTexture);
    glBindTexture(GL_TEXTURE_2D, hTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* texData = new unsigned char[256 * 256 * 3];
    for (int x = 0; x < 256; x++)
        for (int y = 0; y < 256; y++)
        {
            texData[((y << 8) + x) * 3] = (x + y) & 255;
            texData[((y << 8) + x) * 3 + 1] = (x - y) & 255;
            texData[((y << 8) + x) * 3 + 2] = x ^ y;
        }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);
    delete[] texData;

    /* Creating Shader Program */

    uint32_t vertexShader, fragmentShader;
    vertexShader = glCompileShaderFromSourceFile("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
    fragmentShader = glCompileShaderFromSourceFile("shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);

    uint32_t shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int32_t success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success != GL_TRUE)
    {
        int32_t log_length = 0;
        char message[1024];
        glGetProgramInfoLog(shaderProgram, 1024, &log_length, message);
        std::cout << "Shader Program Link Failed:" << std::endl;
        std::cout << message << std::endl;
        glfwSetWindowShouldClose(window, true);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int32_t uniformColorLocation = glGetUniformLocation(shaderProgram, "gColor");
    int32_t gWvalueLocation = glGetUniformLocation(shaderProgram, "gWvalue");
    int32_t gMatrixLocation = glGetUniformLocation(shaderProgram, "gMatrix");
    int32_t hwScaleLocation = glGetUniformLocation(shaderProgram, "hwScale");
    glUseProgram(shaderProgram);
    glUniform4f(uniformColorLocation, 1.0, 1.0, 1.0, 1.0);

    float H = 0.0;

    std::queue<clock_t> frameQ;

    double rad = 0.0;
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(window2))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window2, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window2, true);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            glfwSwapInterval(1);


        H += 0.01;
        rad += 0.001;
        if (H > 6.0) H -= 6.0;
        float r, g, b;
        if (H <= 1.0) r = 1.0, g = H, b = 0.0;
        else if (H <= 2.0) r = 2.0 - H, g = 1.0, b = 0.0;
        else if (H <= 3.0) r = 0.0, g = 1.0, b = H - 2.0;
        else if (H <= 4.0) r = 0.0, g = 4.0 - H, b = 1.0;
        else if (H <= 5.0) r = H - 4.0, g = 0.0, b = 1.0;
        else r = 1.0, g = 0.0, b = 6.0 - H;

        float mat4[] = {
             cos(rad),  sin(rad), 0.0, 0.0,
            -sin(rad),  cos(rad), 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        };

        glfwMakeContextCurrent(window);
        glViewport(0, 0, 512, 512);
        /* Render here */
        glClearColor(0.7, 0.8, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glUniform4f(uniformColorLocation, r, g, b, 1.0);
        glUniformMatrix4fv(gMatrixLocation, 1, 0, mat4);
        glUniform1f(hwScaleLocation, g_aspect);
        glBindVertexArray(hVertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
        glBindTexture(GL_TEXTURE_2D, hTexture);
        glUniform1f(gWvalueLocation, 1.0);
        glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);


        glfwMakeContextCurrent(window2);
        glViewport(0, 0, 512, 512);

        /* Render here */
        glClearColor(0.7, 0.8, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glUniform4f(uniformColorLocation, r, g, b, 1.0);
        glUniformMatrix4fv(gMatrixLocation, 1, 0, mat4);
        glUniform1f(hwScaleLocation, g_aspect);
        glBindVertexArray(hVertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndexBuffer);
        glBindTexture(GL_TEXTURE_2D, hTexture);
        glUniform1f(gWvalueLocation, 1.0);
        glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window2);

        int tclock = clock();
        frameQ.push(tclock);
        while (tclock > frameQ.front() + CLOCKS_PER_SEC) frameQ.pop();
        std::string window_title = "Light Renderer, fps: " + std::to_string(frameQ.size());
        glfwSetWindowTitle(window, window_title.c_str());

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
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