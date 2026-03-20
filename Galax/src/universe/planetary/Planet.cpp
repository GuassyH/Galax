#include "Planet.h"

namespace Universe {

	/// Core

	void Planet::Generate() {
		Delete();
		
		transform = std::make_shared<Transform>();
		faces = CubeSphere::ConstructFaces(radius, resolution, transform.get());

		terrainGenerator.ComputeBuffers(radius);

		for (auto& face : faces) {
			terrainGenerator.ApplyTerrain(face.root_chunk);
		}
	}


	void Planet::Render(Camera& camera, Shader& shader) {
		for (auto& face : faces) {
			if (!face.should_render)
				continue;

			CubeSphere::RenderChunk(face.root_chunk, camera, shader);
		}
	}

	/// Update

	void Planet::Update(Camera& camera) {
		transform->UpdateMatrix();
		UpdateAllLODs(camera.transform->world_position);
	}
	

	void Planet::UpdateLOD(CubeSphere::Chunk* chunk, glm::vec3& observer_pos) {

		if (chunk->level_of_detail > LODradii.size()) {
			chunk->isLeaf = true;
			return;
		}

		if (!chunk->hasNodes) {
			CubeSphere::SubdivideChunk(chunk);
			for (auto node : chunk->nodes) {
				terrainGenerator.ApplyTerrain(node);
			}
		}

		chunk->isLeaf = false;

		// could be optimised
		for (auto node : chunk->nodes) {
			if (!node) {
				GX_ERROR("Planet UpdateLOD: Null node");
				continue;
			}

			unsigned int targetLOD = 0;
			float dist = glm::distance((transform->world_rotation * node->origo) + transform->world_position, observer_pos);
			float sq = glm::sqrt(static_cast<float>(node->level_of_detail) + 1.0f);

			// i need a better way to check
			for (int i = 0; i < LODradii.size(); i++) {
				float range = (LODradii[i] * radius) / sq;
				if (dist <= range) 
					targetLOD = i + 1;
			}


			if (node->level_of_detail == targetLOD) {
				node->isLeaf = true;
				CubeSphere::DestroyChunkNodes(node);
			}
			else if (targetLOD > node->level_of_detail) {
				UpdateLOD(node, observer_pos);
			}
			else { // if (targetLOD < node->level_of_detail) 
				node->isLeaf = false;
				chunk->isLeaf = true;
			}
		}
	}

	// Should this be on another thread?
	void Planet::UpdateAllLODs(glm::vec3 observer_pos) {

		glm::vec3 dir_to_planet = glm::normalize(transform->world_position - observer_pos);
		float range = LODradii[0] * radius;

		for (auto& face : faces) {
			// Only update if required, simple occlusion culling
			if (-glm::dot(dir_to_planet, transform->world_rotation * face.root_chunk->rotation * glm::vec3(0.0f, 0.0f, -1.0f)) > 0.4f) {
				face.should_render = false;
				continue;
			}

			face.should_render = true;

			if (glm::distance((transform->world_rotation * face.root_chunk->origo) + transform->world_position, observer_pos) < range) {
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