#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H
#define GLM_ENABLE_EXPERIMENTAL


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

struct Transform {
public:
	// Local variables (set)
	glm::vec3 local_position = glm::vec3(0.0f); // (get & set)
	glm::vec3 local_scale = glm::vec3(1.0f); // (get & set)
	glm::quat local_rotation = glm::identity<glm::quat>(); // (get & set)

	// Directions
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Global variables (get)
	glm::vec3 world_position = glm::vec3(0.0f); // (get only)
	glm::vec3 world_scale = glm::vec3(0.0f);  // (get only)
	glm::quat world_rotation = glm::identity<glm::quat>(); // (get only)

	// Transform
	std::vector<Transform*> children;
	Transform* parent = nullptr;

	bool HasParent() const {
		return parent != nullptr;
	}

	bool IsDescendant(Transform* parent, Transform* child) {
		for (auto* p = parent; p; p = p->parent)
			if (p == child)
				return true;
		return false;
	}

	// THE RETRANSFORM IS WRONG
	Transform* SetParent(Transform* new_parent, bool change_translate = true) {
		// If same parent, nothing to do
		if (parent == new_parent)
			return parent;
		if (IsDescendant(new_parent, this))
			return parent;


		// Remove from old parent's children list
		if (parent) {
			auto& siblings = parent->children;
			siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
		}

		// Recalculate position and stuff
		if (parent && change_translate) {
			local_scale *= parent->world_scale;
			local_position += parent->world_position;
			local_rotation = parent->world_rotation * local_rotation;
		}

		// Assign new parent
		parent = new_parent;

		// Add to new parent's children if not null (/ setting parent to world)
		if (parent) {
			parent->children.push_back(this);

			if (change_translate) {
				// Recalculate position and stuff
				local_scale.x /= parent->world_scale.x != 0 ? parent->world_scale.x : 1.0f;
				local_scale.y /= parent->world_scale.y != 0 ? parent->world_scale.y : 1.0f;
				local_scale.z /= parent->world_scale.z != 0 ? parent->world_scale.z : 1.0f;

				local_position -= parent->world_position;
				local_rotation = glm::inverse(parent->world_rotation) * local_rotation;
			}
		}

		return parent;
	}

	Transform* SetChild(Transform* new_child) {
		if (!new_child) {
			return nullptr;
		}
		new_child->SetParent(this);
		return new_child;
	}

	glm::mat4& GetMatrix() { return modelMatrix; }
	glm::mat3 GetRotationMatrix() { return glm::mat3(glm::transpose(glm::inverse(modelMatrix))); }

	void UpdateMatrix() {
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), local_scale);
		glm::mat4 rotationMatrix = glm::mat4_cast(local_rotation);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), local_position);

		glm::mat4 localModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		if (parent) {
			modelMatrix = parent->GetMatrix() * localModelMatrix;

			world_rotation = glm::normalize(parent->world_rotation * local_rotation);
			world_position = glm::vec3(modelMatrix * glm::vec4(0, 0, 0, 1));
		}
		else {
			modelMatrix = localModelMatrix;

			world_rotation = local_rotation;
			world_position = local_position;
		}

		forward = glm::normalize(world_rotation * glm::vec3(0, 0, -1));
		up = glm::normalize(world_rotation * glm::vec3(0, 1, 0));
		right = glm::normalize(world_rotation * glm::vec3(1, 0, 0));

		for (auto child : children)
			child->UpdateMatrix();
	}

	// Get rotation in Euler angles (degrees)
	glm::vec3 GetEulerAngles(bool local = true) const {
		if (local)
			return glm::degrees(glm::eulerAngles(local_rotation));
		else
			return glm::degrees(glm::eulerAngles(world_rotation));

	}

	// Set rotation from Euler angles (degrees)
	void SetEulerAngles(const glm::vec3& euler_angles_deg) {
		glm::vec3 euler_rad = glm::radians(euler_angles_deg);
		local_rotation = glm::normalize(glm::quat(euler_rad));
	}

	// Add rotation in Euler angles (degrees), applied in local space
	void AddEulerAngles(const glm::vec3& euler_angles_deg, bool local = true) {
		glm::quat delta = glm::normalize(glm::quat(glm::radians(euler_angles_deg)));

		if (local)
			local_rotation = glm::normalize(local_rotation * delta); // local space
		else
			local_rotation = glm::normalize(delta * local_rotation); // world space
	}

	void AddRotationAroundAxis(const glm::vec3& axis, float angle_deg, bool local = true) {
		glm::quat delta = glm::angleAxis(glm::radians(angle_deg), glm::normalize(axis));

		if (local)
			local_rotation = glm::normalize(local_rotation * delta); // local space
		else
			local_rotation = glm::normalize(delta * local_rotation); // world space
	}

	void SetWorldRotation(const glm::quat& desired_world_rotation) {
		if (parent) {
			local_rotation = glm::normalize(
				glm::inverse(parent->world_rotation) * desired_world_rotation
			);
		}
		else {
			local_rotation = glm::normalize(desired_world_rotation);
		}
	}

	void SetWorldPosition(const glm::vec3& pos) {
		if (parent) {
			local_position = pos - parent->world_position; // simplified
		}
		else {
			local_position = pos;
		}
	}

	Transform() = default;

private:
	glm::mat4 modelMatrix = glm::mat4(1.0f);
};


#endif