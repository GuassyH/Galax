#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <glfw/glfw3.h>
#include "Player.h"

namespace GUI {

	class GUI {
	public:
		GUI() = default;
		~GUI() = default;

		void Init(GLFWwindow* window);
		void NewFrame(GLFWwindow* window);
		void Render(GLFWwindow* window, Player& player);
		void EndFrame();
	};
};