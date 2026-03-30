#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <glfw/glfw3.h>

class GUI {
public:
	GUI() = default;
	~GUI() = default;

	void Init(GLFWwindow* window);
	void NewFrame(GLFWwindow* window);
	void EndFrame();
};