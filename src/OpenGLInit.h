#pragma once 

#define GLEW_STATIC
#ifdef __APPLE__
#include <glad/glad.h>
#else
#include "GL/glew.h"
#endif
#include "GLFW/glfw3.h"

class OpenGLInit
{
    public:
        ~OpenGLInit();
        void static glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
        void static glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
        void showFPS();
        bool initOpenGL();

        static bool gPause;
        static bool gDebug;
        const char* APP_TITLE = "md2 Loader by Raydelto Hernandez v0.1";
        static int gWindowWidth;
        static int gWindowHeight;
        
        double gElapsedSeconds;
        GLFWwindow* GetWindow() { return gWindow; }
    private:
        GLFWwindow* gWindow = NULL;
        static bool gWireframe;
};

