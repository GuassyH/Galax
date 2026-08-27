#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>

#include "rendering/Vertex.h"
#include "rendering/Mesh.h"
#include "rendering/Renderer.h"
#include "shaders/PlanetShader.h"

class CubeSphere {
public:
	static struct Chunk {
		Renderer::GPUslice vertexSlice;

		glm::quat rotation = glm::identity<glm::quat>();
		bool hasNodes = false; // Are there child nodes
		bool isLeaf = true; // Is this the wanted level
		bool hasTerrain = false; // Is the terrain applied
		GLsync computeFence = 0;
		
		int resolution = 10;
		float radius = 5.0f;
		
		int level_of_detail = 0;

		glm::vec2 minUV; // Where will the top left corner be
		glm::vec2 maxUV; // Where will the top right corner be

		glm::vec3 origo;

		Mesh mesh;
		std::vector<std::unique_ptr<CubeSphere::Chunk>> nodes;
	};

	static struct Face {
		std::unique_ptr<CubeSphere::Chunk> root_chunk = nullptr;
		bool should_render = true;
	};

	static std::vector<Face> ConstructFaces(float radius, int resolution, Transform* base_transform = nullptr);
	static void RenderChunk(Chunk* chunk, Transform* sun, Camera& camera, Renderer& renderer, PlanetShader* shader);

	static void SubdivideChunk(CubeSphere::Chunk* chunk);
	static void DestroyChunk(CubeSphere::Chunk* inChunk); // Destroy given chunk and its children
	static void DestroyChunkNodes(CubeSphere::Chunk* inChunk); // Destroys the given chunks' children, not the given chunk
	static void DestroyFace(Face& inFace); // Destroy all chunks within the face

	// static void ForEachNode(Renderer& renderer, Chunk* parent, std::function<void(Renderer&, Chunk*)> func);
};