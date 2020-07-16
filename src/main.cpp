#include "OpenGLInit.h"
#include <iostream>
#include "Md2.h"

namespace
{
const int startFrame = 0;
const int endFrame = 197;
} 

int main()
{
	OpenGLInit openGLinit;
	if (!openGLinit.initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	md2model::Md2 player((char *)"data/female.md2", (char *)"data/female.tga");

	double lastTime = glfwGetTime();
	float angle = 0.0f;

	int renderFrame = startFrame;
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
	projection = glm::perspective(glm::radians(45.0f), (float)OpenGLInit::gWindowWidth / (float)OpenGLInit::gWindowHeight, 0.1f, 100.0f);
    const float velocity = 5.0f;

	while (!glfwWindowShouldClose(openGLinit.GetWindow()))
	{
		openGLinit.showFPS();
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

		// Update the cube position and orientation.  Rotate first then translate
		if (!OpenGLInit::gPause)
		{
			angle += deltaTime * 50.0f;
		}

		if (angle >= 360.0f)
			angle = 0.0f;

		// Poll for and process events
		glfwPollEvents();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		player.Draw(renderFrame, angle, interpolation, view, projection);
		// Swap front and back buffers
		glfwSwapBuffers(openGLinit.GetWindow());
		
		if (interpolation >= 1.0f)
		{
			interpolation = 0.0f;
			if (renderFrame == endFrame)
			{
				renderFrame = startFrame;
				bufferIndex = 0;
			}
			else
			{
				renderFrame++;
				bufferIndex++;
			}
		}
		interpolation += velocity * deltaTime;
	}
	return 0;
}
