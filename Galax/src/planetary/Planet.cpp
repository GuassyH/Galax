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
		int i = 0;
		for (auto& face : faces) {
			face.mesh.Render(camera, shader);
		}
	}

	void Planet::Delete() {
		for (auto& face : faces) {
			face.mesh.Delete();
		}

		faces.clear();

		if (transform) {
			transform->SetParent(nullptr);
			transform.reset();
		}
	}
}