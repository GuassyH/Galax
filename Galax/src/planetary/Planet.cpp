#include "Planet.h"


namespace Universe {

	/// Core

	Planet::Planet() {
		transform = std::make_unique<Transform>();
		faces = CubeSphere::ConstructFaces(radius, transform.get());
	}

	Planet::~Planet() {
		Delete();
	}

	void Planet::Render(Camera& camera, Shader& shader) {
		for (auto& face : faces) {
			CubeSphere::RenderChunk(face.root_chunk, camera, shader);
		}
	}

	/// Update

	void Planet::Update(Camera& camera) {
		transform->UpdateMatrix();
		UpdateAllLODs(camera.transform->world_position);
	}
	

	void Planet::UpdateLOD(CubeSphere::Chunk* chunk, glm::vec3& observer_pos) {

		/// <summary>
		///	For each node in the chunk, check the suggested LOD
		/// If the suggested LOD is the nodes LOD, then use that LOD. 
		/// If the suggested LOD is higher than the nodes LOD, then updated said node
		/// <summary>

		if (chunk->level_of_detail > LODradii.size()) {
			chunk->isLeaf = true;
			return;
		}

		if (!chunk->hasNodes)
			CubeSphere::SubdivideChunk(chunk);

		for (auto node : chunk->nodes) {
			if (!node) {
				GX_ERROR("Planet UpdateLOD: Null Chunk");
				continue;
			}

			unsigned int targetLOD = 0;
			float dist = glm::distance(node->origo + transform->world_position, observer_pos);
			
			// i need a better way to check
			for (int i = 0; i < LODradii.size(); i++) {
				float range = (LODradii[i] * radius) / (glm::sqrt(node->level_of_detail + 1));
				if (dist <= range) 
					targetLOD = i + 1;
			}


			if (node->level_of_detail == targetLOD) {
				node->isLeaf = true;
			}
			else if (targetLOD > node->level_of_detail) {
				node->isLeaf = false;
				UpdateLOD(node, observer_pos);
			}
			else if (targetLOD < node->level_of_detail) {

			}
		}

		chunk->isLeaf = false;
	}

	// Should this be face dependant?
	void Planet::UpdateAllLODs(glm::vec3 observer_pos) {

		for (auto& face : faces) {
			// Only update if required
			float range = LODradii[0] * radius;
			if (glm::distance(face.root_chunk->origo + transform->world_position, observer_pos) < range) {
				UpdateLOD(face.root_chunk, observer_pos);
			}
			else {
				face.root_chunk->isLeaf = true;
				continue;
			}
		}
	}

	/// Delete

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