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

#include "universe/UniverseManager.h"

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
int InitRenderer(GLFWwindow*& window, GLFWmonitor*& monitor);

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
	player.transform->local_position = glm::vec3(200.0f, 0.0f, -10000.0f);
	player.transform->AddRotationAroundAxis(glm::vec3(0.0f, 1.0f, 0.0f), 180);

	FragShader shader("assets/shaders/planet.frag", "assets/shaders/planet.vert");
	FragShader unlit("assets/shaders/default_unlit.frag", "assets/shaders/default_unlit.vert");
	
	// Charley
	Universe::Planet planet_char = Universe::Planet();
	planet_char.planetShader = shader;
	planet_char.radius = 100;
	planet_char.resolution = 50;
	planet_char.LODradii = { 6.0f, 4.0f, 2.0f, 1.0f };
	
	planet_char.terrainGenerator.numCraters = 100;
	planet_char.terrainGenerator.sizeFalloff = 3.0f;
	planet_char.terrainGenerator.baseSize = 1.0f;
	planet_char.terrainGenerator.sizeExaggeration = 4.0f;
	planet_char.terrainGenerator.smoothingK = 0.1f;

	planet_char.terrainGenerator.numLayers = 10;
	planet_char.terrainGenerator.noiseStrength = 2.0f;
	planet_char.terrainGenerator.noiseHeightShift = 0.0f;

	planet_char.terrainGenerator.surfaceColor = glm::vec4(0.396f, 0.58f, 0.306f, 1.0f);
	planet_char.terrainGenerator.peakColor = glm::vec4(0.569f, 0.498f, 0.286f, 1.0f);
	
	planet_char.Generate();
	planet_char.transform->local_position = glm::vec3(0.0f, 0.0f, -10000.0f);
	planet_char.transform->UpdateMatrix();


	Universe::Planet char_moon = Universe::Planet();
	char_moon.planetShader = shader;
	char_moon.radius = 20;
	char_moon.resolution = 50;
	char_moon.LODradii = { 6.0f, 4.0f, 2.0f, 1.0f };

	char_moon.terrainGenerator.numCraters = 20;
	char_moon.terrainGenerator.sizeFalloff = 5.0f;
	char_moon.terrainGenerator.baseSize = 0.8f;
	char_moon.terrainGenerator.sizeExaggeration = 5.0f;
	char_moon.terrainGenerator.smoothingK = 0.1f;

	char_moon.terrainGenerator.numLayers = 8;
	char_moon.terrainGenerator.noiseStrength = 2.0f;
	char_moon.terrainGenerator.noiseHeightShift = 0.0f;

	char_moon.Generate();
	char_moon.transform->local_position = glm::vec3(500.0f, 0.0f, -9700.0f);
	char_moon.transform->UpdateMatrix();


	Universe::AtmosphereConfig planet_char_atmo;
	planet_char_atmo.centre = planet_char.transform->world_position;
	planet_char_atmo.planetRadius = planet_char.radius;
	planet_char_atmo.densityFalloff = 5.0f;
	planet_char_atmo.atmosphereHeight = 40.0f;
	planet_char_atmo.scatteringCoefficient = 250.0f;
	planet_char_atmo.wavelengths = glm::vec3(700.0f, 550.0f, 440.0f);
	planet_char_atmo.scatteringStrength = 0.75f;
	planet_char_atmo.intensity = 0.8f;


	// Sun
	Universe::Planet sun = Universe::Planet();
	sun.planetShader = unlit;
	sun.radius = 600;
	sun.resolution = 50;
	sun.LODradii = { };

	sun.terrainGenerator.numCraters = 0;

	sun.terrainGenerator.numLayers = 2;
	sun.terrainGenerator.noiseStrength = 2.0f;
	sun.terrainGenerator.noiseHeightShift = 0.0f;
	sun.terrainGenerator.noiseBaseFrequency = 0.025f;

	sun.terrainGenerator.surfaceColor = glm::vec4(1.0f, 0.8f, 0.3f, 1.0f);
	sun.terrainGenerator.peakColor = glm::vec4(1.0f, 0.9f, 0.6f, 1.0f);

	sun.Generate();
	sun.transform->local_position = glm::vec3(0.0f, 0.0f, 0.0f);
	sun.transform->UpdateMatrix();


	// NEED TO FIX, if planet_char is first, the subdivisions break somehow
	Universe::UniverseManager::Get().Init(player.camera);
	Universe::UniverseManager::Get().planets.push_back(sun);
	Universe::UniverseManager::Get().planets.push_back(char_moon);
	Universe::UniverseManager::Get().planets.push_back(planet_char);

	Universe::UniverseManager::Get().atmosphereRenderer->atmosphere_configs.push_back(planet_char_atmo);
	Universe::UniverseManager::Get().atmosphereRenderer->UpdateBuffers();

	double current_time = 0.0;

	// Update Loop
	while (!glfwWindowShouldClose(window)) {
		Galax::Time::get().update();

		player.transform->UpdateMatrix();
		player.camera.UpdateMatrix(windowWidth, windowHeight);
		player.Move(window);
		player.Look(window);
		
		Universe::UniverseManager::Get().Update(player);
		Universe::UniverseManager::Get().Render(player.camera, sun, windowWidth, windowHeight);
		
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

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
	// GX_TRACE("window size is {}x{}", windowWidth, windowHeight);
}
