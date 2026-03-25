#include "Player.h"
#include "core/Time.h"

Player::Player() {
	camera.fovDeg = 90.0f;
	camera.nearPlane = 0.3f;
	camera.farPlane = 30000.0f;
	
	transform = std::make_shared<Transform>();

	camera.transform->SetParent(transform.get());
}

Player::~Player() {
	camera.transform->SetParent(nullptr);
	transform.reset();
}

float horizontal = 0.0f;
float vertical = 0.0f;
float skywards = 0.0f;
glm::vec3 moveDir = glm::vec3(0.0f);
void Player::Move(GLFWwindow* window) {

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
		multiplier = 25.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		multiplier = 0.3f;



	glm::vec3 localMoveDir = moveDir;

	// if you are close to the ground, SDF?
	if (transform->HasParent()) {
		moveDir = camera.transform->right * horizontal + camera.transform->forward * vertical;
		moveDir = glm::length(moveDir) != 0 ? glm::normalize(moveDir) : glm::vec3(0);
		moveDir += camera.transform->up * skywards;

		localMoveDir = glm::inverse(transform->parent->world_rotation) * moveDir;
	}
	else {
		moveDir = camera.transform->right * horizontal + camera.transform->forward * vertical;
		moveDir = glm::length(moveDir) != 0 ? glm::normalize(moveDir) : glm::vec3(0);
		moveDir += camera.transform->up * skywards;

		localMoveDir = moveDir;
	}

	transform->local_position += localMoveDir * speed * multiplier * Galax::Time::get().deltaTime;

	moveDir = glm::vec3(0.0f);
	horizontal = 0.0f;
	vertical = 0.0f;
	skywards = 0.0f;
}

double last_mouseX = 0.0;
double last_mouseY = 0.0;
void Player::Look(GLFWwindow* window) {

	if (parent_planet) {
		glm::vec3 up = glm::normalize(transform->world_position - parent_planet->transform->world_position);

		// Keep forward but make it tangent
		glm::vec3 forward = camera.transform->forward;
		forward = glm::normalize(forward - up * glm::dot(forward, up));

		// Rebuild basis
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		forward = glm::normalize(glm::cross(up, right));

		glm::mat3 rot(right, up, -forward);
		this->transform->SetWorldRotation(glm::quat_cast(rot));
	}

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

	float rotX = deltaY * sensitivity * 100.0f;
	float rotY = deltaX * sensitivity * 100.0f;
	
	// Apply rotations to Euler angles
	if (parent_planet) {
		camera.transform->AddRotationAroundAxis(glm::vec3(1.0f, 0.0f, 0.0), -rotX, true); // pitch
		this->transform->AddRotationAroundAxis(glm::vec3(0.0f, 1.0f, 0.0), -rotY, true); // yaw
	}
	else {
		camera.transform->AddRotationAroundAxis(glm::vec3(1.0f, 0.0f, 0.0), -rotX, false); // pitch
		this->transform->AddRotationAroundAxis(glm::vec3(0.0f, 1.0f, 0.0f), -rotY, false); // yaw
	}

	// Clamp pitch to avoid flipping
	glm::vec3 euler = camera.transform->GetEulerAngles();
	if (euler.x > 89.0f) euler.x = 89.0f; // yaw
	if (euler.x < -89.0f) euler.x = -89.0f;
	camera.transform->SetEulerAngles(euler);

	last_mouseX = mouseX;
	last_mouseY = mouseY;
}


// Allign to planet, also makes sure you have to be closer to the planet to 
// get alligned to it than unalligned to stop edge-cases
void Player::AllignToPlanet(Universe::Planet* planet, float _0_1_val) {
	if (parent_planet == planet) 
		return;
	

	if (planet == nullptr) {
		parent_planet = nullptr;
		transform->SetParent(nullptr, true);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0);

		// Keep forward but make it tangent
		glm::vec3 forward = transform->forward;
		forward = glm::normalize(forward - up * glm::dot(forward, up));

		// Rebuild basis
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		forward = glm::normalize(glm::cross(up, right));

		glm::mat3 rot(right, up, -forward);
		this->transform->local_rotation = glm::quat_cast(rot);
		
		this->transform->UpdateMatrix();
		camera.transform->SetEulerAngles(glm::vec3(0.0));

		return;
	}

	// if parent_planet != planet
	if (_0_1_val <= 0.9f) {
		parent_planet = planet;
		transform->SetParent(parent_planet->transform.get(), true);
		transform->UpdateMatrix();
	}
	
}