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

// Amazing, infinite far plane
glm::mat4 MakeInfReversedZProj(float fovY, float aspect, float zNear)
{
	float f = 1.0f / tan(fovY * 0.5f);

	glm::mat4 result(0.0f);

	result[0][0] = f / aspect;
	result[1][1] = f;

	result[2][2] = 0.0f;
	result[2][3] = -1.0f;

	result[3][2] = zNear;

	return result;
}

void Camera::UpdateMatrix() {
	glm::vec2 window_size = Galax::InputManager::Get().windowSize;

	if (window_size.y != 0) {
		proj = MakeInfReversedZProj(glm::radians(fovDeg), (float)window_size.x / (float)window_size.y, nearPlane);
		view = glm::lookAt(transform->world_position, transform->world_position + transform->forward, transform->up);
	}
}


