#pragma once
#include <GLFW/glfw3.h>

uint32_t Wwidth, Wheight;
GLFWwindow* window;

void createWindow()
{
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(1000, 1000, "vk", nullptr, nullptr);//glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, "Window", glfwGetPrimaryMonitor(), nullptr);
	Wwidth = 1000;//glfwGetVideoMode(glfwGetPrimaryMonitor())->width;
	Wheight = 1000; //glfwGetVideoMode(glfwGetPrimaryMonitor())->height;
}
