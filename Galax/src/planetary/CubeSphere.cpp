#include "CubeSphere.h"



void ConstructPlane(CubeSphere::Face& inFace, float radius) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	for (int x = 0; x <= inFace.resolution; x++) {
		for (int y = 0; y <= inFace.resolution; y++) {
			glm::vec3 position = glm::vec3(
				(static_cast<float>(x) / static_cast<float>(inFace.resolution)) - 0.5f,
				(static_cast<float>(y) / static_cast<float>(inFace.resolution)) - 0.5f,
				0.5f);

			position = inFace.rotation * position;

			Vertex vert;
			glm::vec3 p = position * 2.0f; // map from [-0.5,0.5] → [-1,1]

			float x2 = p.x * p.x;
			float y2 = p.y * p.y;
			float z2 = p.z * p.z;

			glm::vec3 spherePos;
			spherePos.x = p.x * sqrt(1.0f - (y2 / 2.0f) - (z2 / 2.0f) + (y2 * z2 / 3.0f));
			spherePos.y = p.y * sqrt(1.0f - (z2 / 2.0f) - (x2 / 2.0f) + (z2 * x2 / 3.0f));
			spherePos.z = p.z * sqrt(1.0f - (x2 / 2.0f) - (y2 / 2.0f) + (x2 * y2 / 3.0f));

			vert.position = spherePos * radius;
			vert.normal = glm::normalize(spherePos);

			glm::vec2 coord = glm::vec2((static_cast<float>(x) / static_cast<float>(inFace.resolution)), (static_cast<float>(y) / static_cast<float>(inFace.resolution)));
			vert.texCoord = coord;

			vertices.push_back(vert);
		}
	}

	for (int x = 0; x < inFace.resolution; x++) {
		for (int y = 0; y < inFace.resolution; y++) {
			int i0 = x * (inFace.resolution + 1) + y;
			int i1 = (x + 1) * (inFace.resolution + 1) + y;
			int i2 = (x + 1) * (inFace.resolution + 1) + (y + 1);
			int i3 = x * (inFace.resolution + 1) + (y + 1);

			// First triangle
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			// Second triangle
			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}

	CubeSphere::Face newFace;
	inFace.mesh = Mesh(vertices, indices);
}

std::vector<CubeSphere::Face> CubeSphere::ConstructFaces(float radius, Transform* base_transform) {
	std::vector<Face> faces;

	glm::vec3 euler_rad;

	for (int i = 0; i < 6; i++){
		// Get correct rotation for each face
		switch (i)
		{
		case 0: // z+ face
			euler_rad = glm::radians(glm::vec3(0.0f, 0.0f, 0.0f));
			break;
		case 1: // z- face
			euler_rad = glm::radians(glm::vec3(0.0f, 180.0f, 0.0f));
			break;
		case 2: // x+ face
			euler_rad = glm::radians(glm::vec3(0.0f, 90.0f, 0.0f));
			break;
		case 3: // x- face
			euler_rad = glm::radians(glm::vec3(0.0f, 270.0f, 0.0f));
			break;
		case 4: // y+ face
			euler_rad = glm::radians(glm::vec3(90.0f, 0.0f, 0.0f));
			break;
		case 5: // y- face
			euler_rad = glm::radians(glm::vec3(270.0f, 0.0f, 0.0f));
			break;
		default: 
			break;
		}

		// Construct plane for each face with correct rotation
		Face newFace; 
		newFace.rotation = glm::normalize(glm::quat(euler_rad));
		ConstructPlane(newFace, radius);

		if (base_transform != nullptr)
			newFace.mesh.transform->SetParent(base_transform);

		// Add face to list (move because Face is non-copyable due to unique_ptrs)
		faces.push_back(std::move(newFace));
	}

	return faces;
}