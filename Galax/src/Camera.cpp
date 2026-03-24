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


