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

	/// Fields

	// Craters
	std::vector<Crater> craters;
	int numCraters = 0;
	float sizeFalloff = 1.0f; // min 1, heigher means less probable big craters
	float baseSize = 1.0f; // the MINIMUM size a crater will have
	float sizeExaggeration = 2.0f;

	// Noise
	glm::vec3 noiseCentre = glm::vec3(0.0f);
	int numLayers = 6;
	float noiseStrength = 1.0f;
	float noiseBaseFrequency = 0.1f; 
	float noiseHeightShift = 0.0f;

	// Colors
	glm::vec4 peakColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 surfaceColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

private:
	GLuint vertBuff = 0;
	GLuint indBuff = 0;
	GLuint craterBuff = 0;
};