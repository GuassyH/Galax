#include "Camera.h"
#include "core/Time.h"
#include "core/Input.h"

Camera::Camera() {
	transform = std::make_shared<Transform>();
	view = glm::mat4(1.0f);
	proj = glm::mat4(1.0f);
}

Camera::~Camera() {

}

void Camera::UpdateMatrix() {
	glm::vec2 window_size = Galax::InputManager::Get().windowSize;

	if (window_size.y != 0) {
		proj = glm::perspective(glm::radians(fovDeg), (float)window_size.x / (float)window_size.y, nearPlane, farPlane);
		view = glm::lookAt(transform->world_position, transform->world_position + transform->forward, transform->up);
	}
}


