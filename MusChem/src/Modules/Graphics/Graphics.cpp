// Self include files
#include "Modules/Graphics/Graphics.h"

// Project include files
#include "GLheaders.h"
#include "Modules/Graphics/Window.h"
#include "Modules/Graphics/Constants.h"

#include "GLogger/GLogger.h"
#include "GParser/GParser.h"
#include "GList/GList.h"

#include "Systems/DebugModules.h"

#include "SOIL.h"

// System include files
#include <stdlib.h>
#include <string>


Graphics::Graphics(const char *title, const char *pathToFile)
{
    // Point window to NULL
    _p_mainWindow = NULL;

    // Initialise GLFW
    if(!glfwInit())
    {
        GLOG_ERR("Could not initialise GLFW");
        throw "GLFW init failed";
    }

    // Get monitors
    _p_monitors = glfwGetMonitors(&_numMonitors);
    _currMonitor = 0;

    // Create fullscreen window on primary monitor
    _p_mainWindow = new Window(_p_monitors[_currMonitor], title);
    if(_p_mainWindow == NULL)
    {
        GLOG_ERR("Could not create Window");
        throw "Window creation failed";
    }

    GLOG_MSG("OpenGL version: %s",
        glGetString(GL_VERSION));
    GLOG_MSG("OpenGL shading version: %s",
        glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialise GLEW
    glewExperimental = true;
    if(glewInit() != GLEW_OK)
    {
        GLOG_ERR("Could not initialise GLEW");
        throw "GLEW init failed";
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // Accept fragment if its closer to the camera than the former
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create screen buffer and texture
    glGenFramebuffers(1, &_screenBuffer);
    glGenTextures(1, &_screenTexture);
    glGenBuffers(1, &_screenVBO);

    // Create and bind VAO
    glGenVertexArrays(1, &_screenVAO);
    glGenVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);

    // Initialise timing variables
    _lastStartRender_s = 0.0f;
    _lastFrameTime_s = 0.0f;

    // Set screen buffer and texture
    glBindTexture(GL_TEXTURE_2D, _screenTexture);

    glTexImage2D
       (GL_TEXTURE_2D, 0, GL_RGB,
        CAMERA_SCREEN_WIDTH, CAMERA_SCREEN_HEIGHT, 0,
        GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, _screenBuffer);
    glFramebufferTexture2D
       (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, _screenTexture, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage
       (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        CAMERA_SCREEN_WIDTH, CAMERA_SCREEN_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer
       (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        GLOG_ERR("Could not initialise screen buffer");
        throw;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ---- PARSE FILE ----
    try
    {
        parseFile(pathToFile);
    }
    catch(...)
    {
        GLOG_ERR("Could not load Graphics data");
        throw;
    }

    GLOG_MSG("Initialised Graphics system");
}

Graphics::Graphics
   (int width_pix, int height_pix,
    const char *title, const char *pathToFile)
{
    // Point window to NULL
    _p_mainWindow = NULL;

    // Initialise GLFW
    if(!glfwInit())
    {
        GLOG_ERR("Could not initialise GLFW");
        throw "GLFW init failed";
    }

    // Get monitors
    _p_monitors = glfwGetMonitors(&_numMonitors);
    _currMonitor = 0;

    // Create fullscreen window on primary monitor
    _p_mainWindow = new Window(width_pix, height_pix, title);
    if(_p_mainWindow == NULL)
    {
        GLOG_ERR("Could not create Window");
        throw "Window creation failed";
    }

    GLOG_MSG("OpenGL version: %s",
        glGetString(GL_VERSION));
    GLOG_MSG("OpenGL shading version: %s",
        glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialise GLEW
    glewExperimental = true;
    if(glewInit() != GLEW_OK)
    {
        GLOG_ERR("Could not initialise GLEW");
        throw "GLEW init failed";
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // Accept fragment if its closer to the camera than the former
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create screen buffer and texture
    glGenFramebuffers(1, &_screenBuffer);
    glGenTextures(1, &_screenTexture);
    glGenBuffers(1, &_screenVBO);

    // Create and bind VAO
    glGenVertexArrays(1, &_screenVAO);
    glGenVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);

    // Initialise timing variables
    _lastStartRender_s = 0.0f;
    _lastFrameTime_s = 0.0f;

    // Set screen buffer and texture
    glBindTexture(GL_TEXTURE_2D, _screenTexture);

    glTexImage2D
       (GL_TEXTURE_2D, 0, GL_RGB,
        CAMERA_SCREEN_WIDTH, CAMERA_SCREEN_HEIGHT, 0,
        GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, _screenBuffer);
    glFramebufferTexture2D
       (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, _screenTexture, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage
       (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        CAMERA_SCREEN_WIDTH, CAMERA_SCREEN_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer
       (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        GLOG_ERR("Could not initialise screen buffer");
        throw;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ---- PARSE FILE ----
    try
    {
        parseFile(pathToFile);
    }
    catch(...)
    {
        GLOG_ERR("Could not load Graphics data");
        throw;
    }

    GLOG_MSG("Initialised Graphics system");
}

Graphics::~Graphics()
{
    glDeleteFramebuffers(1, &_screenBuffer);
    glDeleteVertexArrays(1, &_vaoId);
    glDeleteVertexArrays(1, &_screenVAO);

    GNode<Shader *> *node;
    foreach(node, _progList)
    {
        delete node->data;
    }

    delete _p_mainWindow;

    glfwTerminate();
}

void Graphics::beginRender()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _screenBuffer);
    glViewport(0, 0, CAMERA_SCREEN_WIDTH, CAMERA_SCREEN_HEIGHT);
    glBindVertexArray(_vaoId);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(0.5f);
}

void Graphics::endRender()
{
    // Bind window buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _p_mainWindow->getWidth(), _p_mainWindow->getHeight());
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    // Render quad with screen buffer as texture
    float vertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };

    Shader *p_shader = _progList.find("screen");
    p_shader->activate();

    glBindVertexArray(0);
    glBindVertexArray(_screenVAO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, _screenVBO);
    glBufferData
       (GL_ARRAY_BUFFER,
        6 * 5 * sizeof(float), &vertices[0],
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer
       (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glVertexAttribPointer
       (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid *) 12);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _screenTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(_p_mainWindow->getGlfwWindow());

    // Update times
    double tempRenderTime_s = glfwGetTime();

    _lastFrameTime_s = tempRenderTime_s - _lastStartRender_s;
    _lastStartRender_s = tempRenderTime_s;
}

void Graphics::parseFile(const char *pathToFile)
{
    try
    {
        GParser parser(pathToFile);
        GParser_Line line;

        while( parser.readNextTitle(line) )
        {
            if(line.key == "Shaders")
            {
                while( parser.readNextData(line) )
                {
                    if(line.key == "shader")
                    {
                        size_t delimPos = line.value.find(",");
                        std::string name = line.value.substr(0, delimPos);
                        std::string path = line.value.substr
                           (delimPos + 1, line.value.length() - delimPos);

                        addProgram(name.c_str(), path.c_str());
                    }
                }
            }
            else if(line.key == "Camera")
            {
                glm::vec2 camPos_pix;

                while( parser.readNextData(line) )
                {
                    if(line.key == "pos")
                    {
                        camPos_pix.x = atof(strtok(&line.value[0], ","));
                        camPos_pix.y = atof(strtok(NULL, ","));
                    }
                }

                _camera.update(camPos_pix);
            }
        }
    }
    catch(...)
    {
        GLOG_ERR("Could not parse file %s", pathToFile);
        throw;
    }
}
