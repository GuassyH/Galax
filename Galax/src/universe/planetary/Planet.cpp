#include "Planet.h"
#include "core/Time.h"
#include "core/Input.h"

namespace Universe {

	/// Core

	void Planet::Generate(Renderer& renderer) {
		// Delete just incase you are regenerating
		Delete();

		transform = std::make_shared<Transform>();
		faces = CubeSphere::ConstructFaces(radius, resolution, transform.get());

		terrainGenerator.ComputeBuffers(renderer, radius);

		// For each face apply the terrain through the compute shader
		for (auto& face : faces) {
			terrainGenerator.ApplyTerrain(renderer, face.root_chunk);
		}
	}


	void Planet::Render(Renderer& renderer, Camera& camera, Planet* sun) {


	
		for (auto& face : faces) {
			if (!face.should_render)
				continue;

		
			CubeSphere::RenderChunk(face.root_chunk, sun->transform.get(), camera, renderer, &shader);
		}

	}

	/// Update

	void Planet::Update(Renderer& renderer, Camera& camera, bool isSimulating) {
		if (isSimulating) {
			if (mpr != 0.0f) {
				float spr = mpr * 60.0f;
				float angle_deg = (360.0f * Galax::Time::Get().deltaTime * Galax::Time::Get().timeScale) / spr;
				transform->AddRotationAroundAxis(rotation_axis, angle_deg, false);
			}
		}

		transform->UpdateMatrix();

		if (!LODradii.empty())
			UpdateAllLODs(renderer, camera.transform->world_position);
	}


	void Planet::RemoveNodesFromVSSBO(Renderer& renderer, CubeSphere::Chunk* chunk) {
		for (auto child : chunk->nodes) {
			if (!child) continue;

			if (child->hasNodes)
				RemoveNodesFromVSSBO(renderer, child);

			renderer.FreePlanetBufferSlice(Renderer::VertexBuffer, child->vertexSlice);
		}
	}

	void Planet::UpdateLOD(Renderer& renderer, CubeSphere::Chunk* chunk, glm::vec3& observer_pos) {
		// If there isnt a level of detail specified in LODradii just make this the leaf
		if (chunk->level_of_detail > LODradii.size()) {
			chunk->isLeaf = true;
			return;
		}

		// If there are no nodes (child chunks), subdivide and apply terrain
		if (!chunk->hasNodes) {
			CubeSphere::SubdivideChunk(chunk);

			for (auto node : chunk->nodes) {
				terrainGenerator.ApplyTerrain(renderer, node);
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
			float dist_sqr = glm::distance2((transform->world_rotation * node->origo) + transform->world_position, observer_pos);
			float falloff = static_cast<float>(node->level_of_detail) + 1.0f; // arbitrary value for distance calcs

			// i need a better way to check
			for (int i = 0; i < LODradii.size(); i++) {
				float range_sqr = ((LODradii[i] * radius) / falloff) * ((LODradii[i] * radius) / falloff);
				if (dist_sqr <= range_sqr)
					targetLOD = i + 1;
			}


			if (node->level_of_detail == targetLOD) {
				node->isLeaf = true;
				RemoveNodesFromVSSBO(renderer, node);
				CubeSphere::DestroyChunkNodes(node);
			}
			else if (targetLOD > node->level_of_detail) {
				UpdateLOD(renderer, node, observer_pos);
			}
			else { // if (targetLOD < node->level_of_detail) 
				node->isLeaf = false;
				chunk->isLeaf = true;
			}
		}
	}

	// Should this be on another thread?
	void Planet::UpdateAllLODs(Renderer& renderer, glm::vec3 observer_pos) {
		glm::vec3 dir_to_planet = glm::normalize(transform->world_position - observer_pos);
		float range = LODradii[0] * radius;

		for (auto& face : faces) {
			// Only update if required, simple occlusion culling needs to be better, maybe checking distance from normal to camera pos?
			if (-glm::dot(dir_to_planet, transform->world_rotation * face.root_chunk->rotation * glm::vec3(0.0f, 0.0f, -1.0f)) > 0.4f) {
				face.should_render = false;
				continue;
			}

			face.should_render = true;

			// If you are within the maximum distance THEN check updateLOD
			if (glm::distance2((transform->world_rotation * face.root_chunk->origo) + transform->world_position, observer_pos) < range * range) {
				UpdateLOD(renderer, face.root_chunk, observer_pos);
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