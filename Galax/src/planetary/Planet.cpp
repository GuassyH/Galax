#include "Planet.h"


namespace Universe {

	Planet::Planet() {
		transform = std::make_unique<Transform>();
		faces = CubeSphere::ConstructFaces(20, transform.get());
	}

	Planet::~Planet() {
		Delete();
	}

	void Planet::Update() {
		transform->UpdateMatrix();
	}

	void Planet::Render(Camera& camera, Shader& shader) {
		for (auto& face : faces) {
			CubeSphere::RenderChunk(face.root_chunk, camera, shader);
		}
	}

	void Planet::Delete() {
		for (auto& face : faces) {
			CubeSphere::DestroyFace(face);
		}

		faces.clear();

		if (transform) {
			transform->SetParent(nullptr);
			transform.reset();
		}
	}
}