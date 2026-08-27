#include "CubeSphere.h"

#include <memory>
#include <utility>
/// Generation

void ConstructChunk(CubeSphere::Chunk* inChunk, Transform* base_transform) {

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	for (int x = 0; x < inChunk->resolution + 1; x++) {
		for (int y = 0; y < inChunk->resolution + 1; y++) {
			
			float u = glm::mix(inChunk->minUV.x, inChunk->maxUV.x,
				(float)x / inChunk->resolution);

			float v = glm::mix(inChunk->minUV.y, inChunk->maxUV.y,
				(float)y / inChunk->resolution);

			glm::vec3 position = glm::vec3(u - 0.5f, v - 0.5f, 0.5f);

			position = inChunk->rotation * position;

			Vertex vert;
			glm::vec3 p = position * 2.0f; // map from [-0.5,0.5] → [-1,1]

			float x2 = p.x * p.x;
			float y2 = p.y * p.y;
			float z2 = p.z * p.z;

			glm::vec3 spherePos;
			spherePos.x = p.x * sqrt(1.0f - (y2 / 2.0f) - (z2 / 2.0f) + (y2 * z2 / 3.0f));
			spherePos.y = p.y * sqrt(1.0f - (z2 / 2.0f) - (x2 / 2.0f) + (z2 * x2 / 3.0f));
			spherePos.z = p.z * sqrt(1.0f - (x2 / 2.0f) - (y2 / 2.0f) + (x2 * y2 / 3.0f));


			vert.position = spherePos * inChunk->radius;
			vert.normal = glm::normalize(spherePos);

			glm::vec2 coord = glm::vec2((static_cast<float>(x) / static_cast<float>(inChunk->resolution)), (static_cast<float>(y) / static_cast<float>(inChunk->resolution)));
			vert.params.x = coord.x;
			vert.params.y = coord.y;

			vertices.push_back(vert);
		}
	}
	
	float u = glm::mix(inChunk->minUV.x, inChunk->maxUV.x, 0.5f);
	float v = glm::mix(inChunk->minUV.y, inChunk->maxUV.y, 0.5f);

	glm::vec3 position = inChunk->rotation * glm::vec3(u - 0.5f, v - 0.5f, 0.5f);
	inChunk->origo = glm::normalize(position) * inChunk->radius;

	// Generate indices
	for (int x = 0; x < inChunk->resolution; x++) {
		for (int y = 0; y < inChunk->resolution; y++) {
			int i0 = x * (inChunk->resolution + 1) + y;
			int i1 = (x + 1) * (inChunk->resolution + 1) + y;
			int i2 = (x + 1) * (inChunk->resolution + 1) + (y + 1);
			int i3 = x * (inChunk->resolution + 1) + (y + 1);

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

	inChunk->mesh = Mesh(vertices, indices);

	if (base_transform != nullptr) {
		inChunk->mesh.transform->SetParent(base_transform, false);
		inChunk->mesh.transform->UpdateMatrix();
	}
}

void CubeSphere::SubdivideChunk(CubeSphere::Chunk* chunk) {
	chunk->nodes.clear();
	chunk->nodes.reserve(4);

	glm::vec2 mid = (chunk->minUV + chunk->maxUV) * 0.5f;

	// Top Left
	chunk->nodes.push_back(std::make_unique<CubeSphere::Chunk>());
	auto tl = chunk->nodes[0].get();
	tl->isLeaf = true;
	tl->minUV = chunk->minUV;
	tl->maxUV = mid;
	tl->radius = chunk->radius;
	tl->rotation = chunk->rotation;
	tl->resolution = chunk->resolution;
	tl->level_of_detail = chunk->level_of_detail + 1;



	// Top Right
	chunk->nodes.push_back(std::make_unique<CubeSphere::Chunk>());
	auto tr = chunk->nodes[1].get();
	tr->isLeaf = true;
	tr->minUV = glm::vec2(mid.x, chunk->minUV.y);
	tr->maxUV = glm::vec2(chunk->maxUV.x, mid.y);
	tr->radius = chunk->radius;
	tr->rotation = chunk->rotation;
	tr->resolution = chunk->resolution;
	tr->level_of_detail = chunk->level_of_detail + 1;

	// Bottom Left
	chunk->nodes.push_back(std::make_unique<CubeSphere::Chunk>());
	auto bl = chunk->nodes[2].get();
	bl->isLeaf = true;
	bl->minUV = glm::vec2(chunk->minUV.x, mid.y);
	bl->maxUV = glm::vec2(mid.x, chunk->maxUV.y);
	bl->radius = chunk->radius;
	bl->rotation = chunk->rotation;
	bl->resolution = chunk->resolution;
	bl->level_of_detail = chunk->level_of_detail + 1;

	// Bottom Right
	chunk->nodes.push_back(std::make_unique<CubeSphere::Chunk>());
	auto br = chunk->nodes[3].get();
	br->isLeaf = true;
	br->minUV = mid;
	br->maxUV = chunk->maxUV;
	br->radius = chunk->radius;
	br->rotation = chunk->rotation;
	br->resolution = chunk->resolution;
	br->level_of_detail = chunk->level_of_detail + 1;


	// Construct
	ConstructChunk(tl, chunk->mesh.transform.get());
	ConstructChunk(bl, chunk->mesh.transform.get());
	ConstructChunk(tr, chunk->mesh.transform.get());
	ConstructChunk(br, chunk->mesh.transform.get());

	// Ensure transforms are updated so child meshes have correct world matrices/positions
	// This is required because subdivision can happen after the parent's UpdateMatrix call
	// and newly created children won't have up-to-date world_position until the next frame.
	if (chunk->mesh.transform)
		chunk->mesh.transform->UpdateMatrix();

	chunk->isLeaf = false;
	chunk->hasNodes = true;
}


void CubeSphere::RenderChunk(Chunk* chunk, Transform* sun, Camera& camera, Renderer& renderer, PlanetShader* shader) {
	if (!chunk || !sun || !shader) {
		GX_INFO("CubeSphere::Chunk null pointer check");
		return;
	}

	if (chunk->isLeaf) {
		shader->Use();

		shader->SetMat4("model", chunk->mesh.transform->GetMatrix());
		shader->SetMat4("view", camera.GetView());
		shader->SetMat4("proj", camera.GetProj());
		shader->SetVec3("camPos", camera.transform->world_position);

		shader->SetVec3("sunDir", glm::normalize(chunk->mesh.transform->world_position - sun->world_position));
		shader->SetVec3("centre", chunk->mesh.transform->world_position);
		shader->SetFloat("radius", chunk->radius);
		shader->SetBool("lit", shader->lit);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer.colorMapBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ColorMap) * shader->colorMaps.size(), shader->colorMaps.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderer.colorMapBuffer); // THIS IS ESSENTIAL

		shader->SetInt("numColorMaps", static_cast<int>(shader->colorMaps.size()));

		chunk->mesh.Render();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	}
	else if (chunk->hasNodes) {
		for (auto& node : chunk->nodes) {
			if(node)
				RenderChunk(node.get(), sun, camera, renderer, shader);
		}
	}
}

std::vector<CubeSphere::Face>
CubeSphere::ConstructFaces(float radius, int resolution, Transform* base_transform) {

	std::vector<Face> faces;
	faces.reserve(6);

	glm::vec3 euler_rad;

	for (int i = 0; i < 6; i++) {

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

		Face newFace;

		newFace.root_chunk = std::make_unique<CubeSphere::Chunk>();

		newFace.root_chunk->rotation = glm::normalize(glm::quat(euler_rad));

		newFace.root_chunk->isLeaf = true;
		newFace.root_chunk->minUV = { 0.0f, 0.0f };
		newFace.root_chunk->maxUV = { 1.0f, 1.0f };
		newFace.root_chunk->radius = radius;
		newFace.root_chunk->resolution = resolution;


		ConstructChunk(newFace.root_chunk.get(), base_transform);

		faces.push_back(std::move(newFace));
	}

	return faces;
}


/// Deletion

void CubeSphere::DestroyChunk(CubeSphere::Chunk* inChunk) {
	if (!inChunk)
		return;

	for (auto& node : inChunk->nodes) {
		DestroyChunk(node.get());
	}
	
	inChunk->mesh.transform->SetParent(nullptr);
	inChunk->mesh.Delete();

	inChunk = nullptr;
}

void CubeSphere::DestroyChunkNodes(CubeSphere::Chunk* inChunk) {
	if (!inChunk)
		return;

	if (!inChunk->hasNodes)
		return;

	for (auto& node : inChunk->nodes) {
		DestroyChunk(node.get());
	}

	inChunk->nodes.clear();

	inChunk->hasNodes = false;
}

void CubeSphere::DestroyFace(Face& inFace) {
	DestroyChunk(inFace.root_chunk.get());
}


/*
void CubeSphere::ForEachNode(Renderer& renderer, Chunk* parent, std::function<void(Renderer&, Chunk*)> func) {
	if (!func || !parent) return;

	for (auto node : parent->nodes) {
		if (!node) continue;
		
		if (node->hasNodes)
			ForEachNode(renderer, parent, func);

		func(renderer, node);
	}
}
*/
