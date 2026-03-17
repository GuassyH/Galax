#pragma once

#include <vector>
#include <glad/glad.h>

#include "rendering/Vertex.h"
#include "Mesh.h"

class CubeSphere {
public:
	static struct Chunk {
		glm::quat rotation = glm::identity<glm::quat>();
		bool isLeaf = true; // Is this the level or are the child nodes the level
		int resolution = 20;
		float radius = 1.0f;
		
		glm::vec2 minUV; // Where will the top left corner be
		glm::vec2 maxUV; // Where will the top right corner be

		Mesh mesh;
		Chunk* nodes[4] = { nullptr };
	};
	static struct Face {
		Chunk* root_chunk = nullptr;
	};

	static std::vector<Face> ConstructFaces(float radius, Transform* base_transform = nullptr);
	static void SubdivideChunk(Chunk* chunk);
	static void RenderChunk(Chunk* chunk, Camera& camera, Shader& shader);

	static void DestroyChunk(Chunk* inChunk); // Destroy given chunk and its children
	static void DestroyChunkNodes(Chunk* inChunk); // Destroys the given chunks' children, not the given chunk
	static void DestroyFace(Face& inFace); // Destroy all chunks within the face
};