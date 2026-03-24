#pragma once

#include <vector>
#include <glad/glad.h>

#include "rendering/Vertex.h"
#include "Mesh.h"

class CubeSphere {
public:
	static struct Chunk {
		glm::quat rotation = glm::identity<glm::quat>();
		bool hasNodes = false; // Are there child nodes
		bool isLeaf = true; // Is this the wanted level
		int resolution = 10;
		float radius = 5.0f;
		
		int level_of_detail = 0;

		glm::vec2 minUV; // Where will the top left corner be
		glm::vec2 maxUV; // Where will the top right corner be

		glm::vec3 origo;

		Mesh mesh;
		Chunk* nodes[4] = { nullptr };
	};
	static struct Face {
		Chunk* root_chunk = nullptr;
		bool should_render = true;
	};

	static std::vector<Face> ConstructFaces(float radius, int resolution, Transform* base_transform = nullptr);
	static void RenderChunk(Chunk* chunk, Transform* sun, Camera& camera, Shader& shader);

	static void SubdivideChunk(Chunk* chunk);
	static void DestroyChunk(Chunk* inChunk); // Destroy given chunk and its children
	static void DestroyChunkNodes(Chunk* inChunk); // Destroys the given chunks' children, not the given chunk
	static void DestroyFace(Face& inFace); // Destroy all chunks within the face
};