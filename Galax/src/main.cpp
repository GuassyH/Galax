#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <windows.h>

#include "Log.h"
#include "GalaxTime.h"

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

#include "planetary/Planet.h"
#include "Time.h"

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
int InitRenderer(GLFWwindow*& window, GLFWmonitor*& monitor);

int windowWidth = 1280;
int windowHeight = 720;
int monitorWidth;
int monitorHeight;

int main() {
	Log::Init();
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
	
	Universe::Planet planet;
	planet.radius = 100;
	planet.resolution = 150;
	planet.LODradii = { 10.0f, 7.5f, 5.2f, 2.0f, 0.9f, 0.5f };
	
	planet.terrainGenerator.numCraters = 200;
	planet.terrainGenerator.sizeFalloff = 5.0f;
	planet.terrainGenerator.baseSize = 0.4f;
	planet.terrainGenerator.sizeExaggeration = 5.0f;

	planet.terrainGenerator.noiseStrength = 3.0f;
	planet.terrainGenerator.noiseHeightShift = 1.0f;

	planet.Generate();
	planet.transform->local_position += glm::vec3(0.0f, 0.0f, -200.0f);

	double current_time = 0.0;

	// Update Loop
	while (!glfwWindowShouldClose(window)) {
		GalaxTime::get().update();

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Move(window);
		camera.Look(window);
		camera.UpdateMatrix(windowWidth, windowHeight);

		planet.Update(camera);
		planet.Render(camera, shader);
	
		glfwPollEvents();
		glfwSwapBuffers(window);
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
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
	// GX_TRACE("window size is {}x{}", windowWidth, windowHeight);
}
