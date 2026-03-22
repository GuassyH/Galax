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

#include "core/Log.h"
#include "core/Time.h"

#include "universe/planetary/Planet.h"
#include "universe/stars/StarSkybox.h"
#include "universe/atmosphere/Atmosphere.h"

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
int InitRenderer(GLFWwindow*& window, GLFWmonitor*& monitor);

int windowWidth = 1920;
int windowHeight = 1080;
int monitorWidth;
int monitorHeight;

std::vector<Universe::Planet> planets;

int main() {
	Galax::Log::Init();
	GX_TRACE("Application Started");

	GLFWwindow* window = nullptr;
	GLFWmonitor* monitor = nullptr;

	if (!InitRenderer(window, monitor)) {
		return -1;
	}

	Camera camera = Camera();
	camera.fovDeg = 90.0f;
	camera.transform->local_position = glm::vec3(0.0f, 0.0f, 5.0f);

	FragShader shader("assets/shaders/default.frag", "assets/shaders/default.vert");
	
	Universe::Planet planet_char;
	planet_char.radius = 100;
	planet_char.resolution = 50;
	planet_char.LODradii = { 6.0f, 4.0f, 2.0f, 1.0f };
	
	planet_char.terrainGenerator.numCraters = 200;
	planet_char.terrainGenerator.sizeFalloff = 5.0f;
	planet_char.terrainGenerator.baseSize = 0.8f;
	planet_char.terrainGenerator.sizeExaggeration = 5.0f;

	planet_char.terrainGenerator.numLayers = 10;
	planet_char.terrainGenerator.noiseStrength = 2.0f;
	planet_char.terrainGenerator.noiseHeightShift = 0.0f;

	planet_char.terrainGenerator.surfaceColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	planet_char.terrainGenerator.peakColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	
	planet_char.Generate();
	planet_char.transform->local_position += glm::vec3(0.0f, 0.0f, -200.0f);
	planet_char.transform->UpdateMatrix();

	planets.push_back(planet_char);

	Universe::StarSkybox stars = Universe::StarSkybox();
	stars.Generate(800, 5, 2, camera.farPlane);


	Universe::AtmosphereConfig planet_char_atmo;
	planet_char_atmo.centre = planet_char.transform->world_position;
	planet_char_atmo.planetRadius = planet_char.radius;
	planet_char_atmo.atmosphereHeight = 15.0f;

	Universe::AtmosphereRenderer atmosRend = Universe::AtmosphereRenderer();

	atmosRend.atmosphere_configs.push_back(planet_char_atmo);
	atmosRend.UpdateBuffers();

	double current_time = 0.0;

	// Update Loop
	while (!glfwWindowShouldClose(window)) {
		Galax::Time::get().update();

		glBindFramebuffer(GL_FRAMEBUFFER, atmosRend.framebuffer);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, windowWidth, windowHeight);


		camera.Move(window);
		camera.Look(window);
		camera.UpdateMatrix(windowWidth, windowHeight);
		

		for (auto& planet : planets) {
			planet.Update(camera);
			planet.Render(camera, shader);
		}
		
		atmosRend.Render(camera, windowWidth, windowHeight);
		
		// stars.Update(camera.transform.get());
		// stars.Render(camera);

		glfwPollEvents();
		glfwSwapBuffers(window);

	}

	for (auto& planet : planets) {
		planet.Delete();
	}

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
