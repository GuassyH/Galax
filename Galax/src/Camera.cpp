#include "Camera.h"
#include "core/Time.h"

Camera::Camera() {
	transform = std::make_shared<Transform>();
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);
}

Camera::~Camera() {

}

void Camera::UpdateMatrix(unsigned int windowWidth, unsigned int windowHeight) {
	transform->UpdateMatrix();

	proj = glm::perspective(glm::radians(fovDeg), (float)windowWidth / (float)windowHeight, nearPlane, farPlane);
	view = glm::lookAt(transform->world_position, transform->world_position + transform->forward, transform->up);
}


float horizontal = 0.0f;
float vertical = 0.0f;
float skywards = 0.0f;
glm::vec3 moveDir = glm::vec3(0.0f);
void Camera::Move(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_D))
		horizontal += 1.0f;

	if (glfwGetKey(window, GLFW_KEY_A))
		horizontal -= 1.0f;

	if (glfwGetKey(window, GLFW_KEY_W))
		vertical += 1.0f;

	if (glfwGetKey(window, GLFW_KEY_S))
		vertical -= 1.0f;


	if (glfwGetKey(window, GLFW_KEY_SPACE))
		skywards += 1.0f;

	if (glfwGetKey(window, GLFW_KEY_C))
		skywards -= 1.0f;

	float multiplier = 1.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		multiplier = 10.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		multiplier = 0.3f;

	moveDir = transform->right * horizontal + transform->forward * vertical;
	moveDir = glm::length(moveDir) != 0 ? glm::normalize(moveDir) : glm::vec3(0);
	moveDir += transform->up * skywards;

	transform->local_position += moveDir * speed * multiplier * Galax::Time::get().deltaTime;

	if (glfwGetKey(window, GLFW_KEY_E))
		// transform->AddRotationAroundAxis(transform->forward, 1.0f, false);

	if (glfwGetKey(window, GLFW_KEY_Q))
		// transform->AddRotationAroundAxis(transform->forward, -1.0f, false);

	moveDir = glm::vec3(0.0f);
	horizontal = 0.0f;
	vertical = 0.0f;
	skywards = 0.0f;
}

double last_mouseX = 0.0;
double last_mouseY = 0.0;
void Camera::Look(GLFWwindow* window) {

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) {
		// Reset last mouse position when not looking to prevent jumps
		glfwGetCursorPos(window, &last_mouseX, &last_mouseY);
		return;
	}

	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Should be clamped to window size

	float deltaX = static_cast<float>(mouseX - last_mouseX);
	float deltaY = static_cast<float>(mouseY - last_mouseY);

	float rotX = -deltaY * sensitivity * 100.0f;
	float rotY = deltaX * sensitivity * 100.0f;

	// Apply rotations to Euler angles
	transform->AddRotationAroundAxis(transform->right, rotX, false); // pitch
	transform->AddRotationAroundAxis(glm::vec3(0.0f, 1.0f, 0.0f), -rotY, false); // yaw

	// Clamp pitch to avoid flipping
	glm::vec3 euler = transform->GetEulerAngles();
	if (euler.x > 89.0f) euler.x = 89.0f; // yaw
	if (euler.x < -89.0f) euler.x = -89.0f;

	last_mouseX = mouseX;
	last_mouseY = mouseY;
}