#pragma once
#include "initWindow.h"

glm::vec3 pos(0.0, 0.0, 0.0);

void control()
{
	int rightState = glfwGetKey(window, GLFW_KEY_S);
	if (rightState == GLFW_PRESS)
	{
		pos += glm::vec3(0.005, 0.0, 0.005);
	}
	int leftState = glfwGetKey(window, GLFW_KEY_W);
	if (leftState == GLFW_PRESS)
	{
		pos -= glm::vec3(0.005, 0.0, 0.005);
	}
	int destroyState = glfwGetKey(window, GLFW_KEY_ESCAPE);
	if (destroyState == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}