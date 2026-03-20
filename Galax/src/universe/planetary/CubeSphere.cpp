#include "CubeSphere.h"

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

			glm::vec3 position = glm::vec3(
				u - 0.5f,
				v - 0.5f,
				0.5f
			);

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
			vert.texCoord = coord;

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
	}
}

void CubeSphere::SubdivideChunk(CubeSphere::Chunk* chunk) {

	glm::vec2 mid = (chunk->minUV + chunk->maxUV) * 0.5f;

	// Top Left
	chunk->nodes[0] = new CubeSphere::Chunk;
	auto tl = chunk->nodes[0];
	tl->isLeaf = true;
	tl->minUV = chunk->minUV;
	tl->maxUV = mid;
	tl->radius = chunk->radius;
	tl->rotation = chunk->rotation;
	tl->resolution = chunk->resolution;
	tl->level_of_detail = chunk->level_of_detail + 1;

	// Top Right
	chunk->nodes[1] = new CubeSphere::Chunk;
	auto tr = chunk->nodes[1];
	tr->isLeaf = true;
	tr->minUV = glm::vec2(mid.x, chunk->minUV.y);
	tr->maxUV = glm::vec2(chunk->maxUV.x, mid.y);
	tr->radius = chunk->radius;
	tr->rotation = chunk->rotation;
	tr->resolution = chunk->resolution;
	tr->level_of_detail = chunk->level_of_detail + 1;

	// Bottom Left
	chunk->nodes[2] = new CubeSphere::Chunk;
	auto bl = chunk->nodes[2];
	bl->isLeaf = true;
	bl->minUV = glm::vec2(chunk->minUV.x, mid.y);
	bl->maxUV = glm::vec2(mid.x, chunk->maxUV.y);
	bl->radius = chunk->radius;
	bl->rotation = chunk->rotation;
	bl->resolution = chunk->resolution;
	bl->level_of_detail = chunk->level_of_detail + 1;

	// Bottom Right
	chunk->nodes[3] = new CubeSphere::Chunk;
	auto br = chunk->nodes[3];
	br->isLeaf = true;
	br->minUV = mid;
	br->maxUV = chunk->maxUV;
	br->radius = chunk->radius;
	br->rotation = chunk->rotation;
	br->resolution = chunk->resolution;
	br->level_of_detail = chunk->level_of_detail + 1;


	// Construct
	ConstructChunk(tl, chunk->mesh.transform.get());
	ConstructChunk(tr, chunk->mesh.transform.get());
	ConstructChunk(bl, chunk->mesh.transform.get());
	ConstructChunk(br, chunk->mesh.transform.get());

	chunk->isLeaf = false;
	chunk->hasNodes = true;
}


void CubeSphere::RenderChunk(Chunk* chunk, Camera& camera, Shader& shader) {
	if (!chunk)
		return;

	if (chunk->isLeaf) {
		chunk->mesh.Render(camera, shader);
	}
	else if (chunk->hasNodes) {
		for (auto node : chunk->nodes) {
			if(node)
				RenderChunk(node, camera, shader);
		}
	}
}

// Create all Faces
std::vector<CubeSphere::Face> CubeSphere::ConstructFaces(float radius, int resolution, Transform* base_transform) {
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
		newFace.root_chunk = new Chunk;
		newFace.root_chunk->rotation = glm::normalize(glm::quat(euler_rad));
		newFace.root_chunk->isLeaf = true;
		newFace.root_chunk->minUV = { 0.0f, 0.0f };
		newFace.root_chunk->maxUV = { 1.0f, 1.0f };
		newFace.root_chunk->radius = radius;
		newFace.root_chunk->resolution = resolution;

		ConstructChunk(newFace.root_chunk, base_transform);

		// Add face to list (move because Face is non-copyable due to unique_ptrs)
		faces.push_back(std::move(newFace));
	}

	return faces;
}


/// Deletion

void CubeSphere::DestroyChunk(Chunk* inChunk) {
	if (!inChunk)
		return;

	for (auto node : inChunk->nodes) {
		DestroyChunk(node);
	}
	
	inChunk->mesh.transform->SetParent(nullptr);
	inChunk->mesh.Delete();
	delete inChunk;
	inChunk = nullptr;
}

void CubeSphere::DestroyChunkNodes(Chunk* inChunk) {
	if (!inChunk)
		return;

	if (!inChunk->hasNodes)
		return;

	for (auto node : inChunk->nodes) {
		DestroyChunk(node);
	}

	inChunk->nodes[0] = nullptr;
	inChunk->nodes[1] = nullptr;
	inChunk->nodes[2] = nullptr;
	inChunk->nodes[3] = nullptr;

	inChunk->hasNodes = false;
}

void CubeSphere::DestroyFace(Face& inFace) {
	DestroyChunk(inFace.root_chunk);
}