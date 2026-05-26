#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <windows.h>
#include <xmemory>

#include <KHR/khrplatform.h>
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>
#include <glad/glad.h>

#include "glm/common.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "rendering/Mesh.h"
#include "shaders/Shader.h"
#include "shaders/FragShader.h"
#include "shaders/PlanetShader.h"
#include "Camera.h"
#include "Player.h"

#include "core/Log.h"
#include "core/Time.h"
#include "core/Input.h"

#include "universe/UniverseManager.h"
#include "universe/UniverseDebug.h"
#include "gui/GUI.h"
#include "gui/PlanetEditorGUI.h"

#include "system_presets/SystemPresets.h"
#include "rendering/Renderer.h"

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);

int InitRenderer(GLFWwindow*& window, GLFWmonitor*& monitor);
void SetFullscreen(GLFWwindow* window, bool fullscreen);
void SetWindowResolution(GLFWwindow* window, int width, int height);

int windowWidth = 1920;
int windowHeight = 1080;
int monitorWidth;
int monitorHeight;


int main() {
	Galax::Log::Init();
	GX_TRACE("Application Started");

	GLFWwindow* window = nullptr;
	GLFWmonitor* monitor = nullptr;

	if (!InitRenderer(window, monitor)) return -1;
	
	bool isFullscreen = false; // SetFullscreen(window, true);

	Renderer renderer = Renderer();

	// Create Player
	Player player;
	player.camera.fovDeg = 90.0f;
	player.camera.nearPlane = 0.1f;

	player.transform->local_position = glm::vec3(5548000.0f, 0.0f, 0.0f);
	player.transform->AddRotationAroundAxis(glm::vec3(0.0f, 1.0f, 0.0f), 180);
	player.transform->UpdateMatrix();

	// Initialise the system
	GUI::GUI gui; gui.Init(window);
	Universe::UniverseManager::Get().Init(player.camera);
	Universe::UniverseDebug::Get().Init();
	
	auto sun = SystemPresets::CreateFirstSystem(renderer);

	// Update Loop
	while (!glfwWindowShouldClose(window)) {
		Galax::Time::Get().update();
		
		// Universe
		Universe::UniverseManager::Get().Update(renderer, player);
		Universe::UniverseManager::Get().Render(renderer, player.camera, sun.get());
		
		// Debug
		Universe::UniverseDebug::Get().Update(player);
		Universe::UniverseDebug::Get().Render(player);

		// UI 
		gui.NewFrame(window);
		gui.Render(renderer, window, player);
		gui.EndFrame();

		// End frame
		if (Galax::Input::IsKeyJustPressed(GLFW_KEY_F11)) {
			if (!isFullscreen) {
				isFullscreen = true;
				SetFullscreen(window, true);
			}
			else {
				isFullscreen = false;
				SetFullscreen(window, false);
			}
		}
		if (Galax::Input::IsKeyJustPressed(GLFW_KEY_ESCAPE)) 
			glfwSetWindowShouldClose(window, true);



		Galax::InputManager::Get().Clear(); // Reset inputs


		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	Universe::UniverseManager::Get().Shutdown();


	// End
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


int InitRenderer(GLFWwindow*& window, GLFWmonitor*& monitor) {
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	monitor = glfwGetPrimaryMonitor();
	window = glfwCreateWindow(windowWidth, windowHeight, "Galax - View", NULL, NULL);

	if (!window) {
		GX_ERROR("Failed to create Window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, windowWidth, windowHeight);

	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		GX_ERROR("Failed to initialize GLAD");
		return -1;
	}


	/*
	glEnable(GL_DEBUG_OUTPUT);

	glDebugMessageCallback([](
		GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam) {
			std::cout << "OpenGL: " << message << std::endl; },
			nullptr);
	*/

	// Get the monitors width and height
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	monitorWidth = mode->width;
	monitorHeight = mode->height;


	// Set Callbacks
	// Set all the callbacks: ie, when resizing send a WindowResizeEvent to the EventHandler
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (action == GLFW_PRESS)
			GX_ADD_KEY_PRESSED(key);
		else if (action == GLFW_RELEASE)
			GX_ADD_KEY_RELEASED(key);
		});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS)
			GX_ADD_MOUSE_BUTTON_PRESSED(button);
		else if (action == GLFW_RELEASE)
			GX_ADD_MOUSE_BUTTON_RELEASED(button);
		});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		if (window) Galax::InputManager::Get().mousePosition = glm::vec2(xpos, ypos);
		});
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		if (window) Galax::InputManager::Get().scrollOffset = glm::vec2(xoffset, yoffset);
		});
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		if (window) Galax::InputManager::Get().windowSize = glm::vec2(width, height);
		});


	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

// Nabbed code from other source... NOT MY FULLSCREEN CODE
void SetFullscreen(GLFWwindow* window, bool fullscreen) {
	static int windowedX, windowedY, windowedW, windowedH;

	if (fullscreen) {
		// Save windowed position/size
		glfwGetWindowPos(window, &windowedX, &windowedY);
		glfwGetWindowSize(window, &windowedW, &windowedH);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else {
		glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedW, windowedH, 0);
	}
}

void SetWindowResolution(GLFWwindow* window, int width, int height) {
	glfwSetWindowSize(window, width, height);

	float xpos = (2560 / 2.0f) - (width / 2.0f);
	float ypos = (1440 / 2.0f) - (height / 2.0f);

	glfwSetWindowPos(window, xpos, ypos);
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
}

