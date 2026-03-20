#pragma once

#include "shaders/ComputeShader.h"
#include "CubeSphere.h"


class TerrainGenerator {
public:
	struct Crater {
		glm::vec4 position; // just use xyz

		float craterSteepness;
		float craterWidth;
		float craterDepth;
		float smoothingK; // 16

		float rimSteepness;
		float rimWidth;
		float size;
		float pad1 = 0.0f;
	};

	TerrainGenerator();
	~TerrainGenerator();

	void ComputeBuffers(float radius);
	void ApplyTerrain(CubeSphere::Chunk* chunk);

	ComputeShader terrain_compute;
	ComputeShader normals_compute;

	// fields
	std::vector<Crater> craters;
	int numCraters = 0;
	float falloff = 1.0f; // min 1, heigher means less probable big craters

private:
	GLuint vertBuff = 0;
	GLuint indBuff = 0;
	GLuint craterBuff = 0;
};