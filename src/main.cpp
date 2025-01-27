#include <iostream>
#include <sstream>
#include <cmath>

#define GLEW_STATIC
#ifdef __APPLE__
#include <glad/glad.h>
#else
#include "GL/glew.h"
#endif
#include <SDL3/SDL.h>

#include "Md2.h"

namespace
{
	const int gStartFrame = 0;
	const int gEndFrame = 197;
	SDL_Window *gWindow = nullptr;
	SDL_GLContext gGlContext;
	int gWindowWidth = 800;
	int gWindowHeight = 600;
	bool gPause = false;
	bool gWireframe = false;
	constexpr char* APP_TITLE = "Quake 2 MD2 Model Loader - SDL3";
}

bool Init()
{
	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, APP_TITLE);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, gWindowWidth);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, gWindowHeight);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	gWindow = SDL_CreateWindowWithProperties(props);

	if (!gWindow)
	{
		std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return false;
	}

	// Create OpenGL context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	gGlContext = SDL_GL_CreateContext(gWindow);
	if (!gGlContext)
	{
		std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(gWindow);
		SDL_Quit();
		return false;
	}

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		SDL_GL_DestroyContext(gGlContext);
		SDL_DestroyWindow(gWindow);
		SDL_Quit();
		return false;
	}

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	return true;
}

void ShowFPS()
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double currentSeconds = SDL_GetTicks() / 1000.0;

	double elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = static_cast<double>(frameCount) / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		SDL_SetWindowTitle(gWindow, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}

int main()
{
	if (!Init())
	{
		std::cerr << "Failed to initialize SDL" << std::endl;
		return -1;
	}

	md2model::Md2 player((char *)"data/female.md2", (char *)"data/female.tga");

	double lastTime = SDL_GetTicks() / 1000.0;
	float angle = 0.0f;

	int renderFrame = gStartFrame;
	// Rendering loop
	float interpolation = 0.0f;
	int bufferIndex = 0;

	glm::mat4 view, projection;
	glm::vec3 camPos(0.0f, 0.0f, 0.0f);
	glm::vec3 targetPos(0.0f, 0.0f, -20.0f);
	glm::vec3 up(1.0f, 0.0f, 0.0f);

	// Create the View matrix
	view = glm::lookAt(camPos, camPos + targetPos, up);

	// Create the projection matrix
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(gWindowWidth) / static_cast<float>(gWindowHeight), 0.1f, 100.0f);
	const float speed = 5.0f;

	bool running = true;
	while (running)
	{
		ShowFPS();
		
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
			{
				running = false;
			}
			else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_SPACE)
			{
				gPause = !gPause;
			}
			else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_F1)
			{
				gWireframe = !gWireframe;
				if (gWireframe)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}			
			else if (event.type == SDL_EVENT_WINDOW_RESIZED)
			{
				gWindowWidth = event.window.data1;
				gWindowHeight = event.window.data2;
				glViewport(0, 0, gWindowWidth, gWindowHeight);
			}
		}

		float currentTime = SDL_GetTicks() / 1000.0;
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		if (!gPause)
		{
			angle += deltaTime * 50.0f;
		}

		angle = fmod(angle, 360.0f);

		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		player.Draw(renderFrame, angle, interpolation, view, projection);
		// Swap front and back buffers
		SDL_GL_SwapWindow(gWindow);

		if (interpolation >= 1.0f)
		{
			interpolation = 0.0f;
			if (renderFrame == gEndFrame)
			{
				renderFrame = gStartFrame;
				bufferIndex = 0;
			}
			else
			{
				renderFrame++;
				bufferIndex++;
			}
		}
		interpolation += speed * deltaTime;
	}

	SDL_GL_DestroyContext(gGlContext);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}
