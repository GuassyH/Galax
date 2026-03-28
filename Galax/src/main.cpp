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

#include "Mesh.h"
#include "shaders/Shader.h"
#include "shaders/FragShader.h"
#include "Camera.h"
#include "Player.h"

#include "core/Log.h"
#include "core/Time.h"
#include "core/Input.h"

#include "universe/UniverseManager.h"

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

	if (!InitRenderer(window, monitor)) {
		return -1;
	}

	Player player;
	player.camera.fovDeg = 90.0f;
	player.camera.nearPlane = 1.0f;
	player.camera.farPlane = 100000.0f;

	player.transform->local_position = glm::vec3(44000.0f, 0.0f, 0.0f);
	player.transform->AddRotationAroundAxis(glm::vec3(0.0f, 1.0f, 0.0f), 180);

	FragShader shader("assets/shaders/universe/planet.frag", "assets/shaders/universe/planet.vert");
	FragShader unlit("assets/shaders/default_unlit.frag", "assets/shaders/default_unlit.vert");
	
	// Charley
	std::shared_ptr<Universe::Planet> planet_char = std::make_shared<Universe::Planet>();
	planet_char->planetShader = shader;
	planet_char->radius = 500;
	planet_char->resolution = 64;
	planet_char->LODradii = { 6.0f, 3.0f, 1.0f, 0.8f };

	planet_char->terrainGenerator.numCraters = 0;
	planet_char->terrainGenerator.sizeFalloff = 3.0f;
	planet_char->terrainGenerator.baseSize = 10.0f;
	planet_char->terrainGenerator.sizeExaggeration = 2.0f;
	planet_char->terrainGenerator.smoothingK = 0.1f;
	planet_char->terrainGenerator.craterHeight = 1.0f;

	planet_char->terrainGenerator.numLayers = 8;
	planet_char->terrainGenerator.noiseStrength = 40.0f;
	planet_char->terrainGenerator.noiseHeightShift = -2.0f;
	planet_char->terrainGenerator.noiseScale = 0.1f;
	planet_char->terrainGenerator.surfaceColor = glm::vec4(0.396f, 0.58f, 0.306f, 1.0f);
	planet_char->terrainGenerator.peakColor = glm::vec4(0.569f, 0.498f, 0.286f, 1.0f);

	planet_char->physicsBody.mass = 1000000000;
	planet_char->mpr = 10.0; // x minutes for one rot

	planet_char->Generate();
	planet_char->transform->local_position = glm::vec3(45000.0f, 0.0f, 0.0f);
	planet_char->transform->UpdateMatrix();

	planet_char->atmosphere_config.planetRadius = planet_char->radius;
	planet_char->atmosphere_config.densityFalloff = 8.0f;
	planet_char->atmosphere_config.atmosphereHeight = 200.0f;
	planet_char->atmosphere_config.scatteringCoefficient = 200.0f;
	planet_char->atmosphere_config.wavelengths = glm::vec3(700.0f, 550.0f, 440.0f);
	planet_char->atmosphere_config.scatteringStrength = 0.5f;
	planet_char->atmosphere_config.intensity = 0.9f;
	planet_char->atmosphere_config.UpdateScatteringCoefficients();
	planet_char->hasAtmosphere = true;

	planet_char->ocean_config.radius = planet_char->radius;
	planet_char->ocean_config.densityFalloff = 1.0f;
	planet_char->ocean_config.oceanColor = glm::vec4(0.0, 0.1, 0.3, 1.0);
	planet_char->hasOcean = true;

	// Sun
	std::shared_ptr<Universe::Planet> sun = std::make_shared<Universe::Planet>();
	sun->planetShader = unlit;
	sun->radius = 1000;
	sun->resolution = 50;
	sun->LODradii = { };

	sun->terrainGenerator.numCraters = 0;

	sun->terrainGenerator.numLayers = 2;
	sun->terrainGenerator.noiseStrength = 10.0f;
	sun->terrainGenerator.noiseHeightShift = 0.0f;
	sun->terrainGenerator.noiseBaseFrequency = 0.025f;

	sun->terrainGenerator.surfaceColor = glm::vec4(1.0f, 0.8f, 0.3f, 1.0f);
	sun->terrainGenerator.peakColor = glm::vec4(1.0f, 0.9f, 0.6f, 1.0f);

	sun->Generate();
	sun->transform->local_position = glm::vec3(0.0, 0.0f, 0.0f);
	sun->transform->UpdateMatrix();


	// NEED TO FIX, if planet_char is first, the subdivisions break somehow
	Universe::UniverseManager::Get().Init(player.camera);
	Universe::UniverseManager::Get().PushPlanet(sun);
	Universe::UniverseManager::Get().PushPlanet(planet_char);


	bool isFullscreen = true;

	SetFullscreen(window, true);
	player.transform->UpdateMatrix();
	
	// Update Loop
	while (!glfwWindowShouldClose(window)) {

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

		Galax::Time::get().update();
		
		Universe::UniverseManager::Get().Update(player);
		Universe::UniverseManager::Get().Render(player.camera, sun.get());
		

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

