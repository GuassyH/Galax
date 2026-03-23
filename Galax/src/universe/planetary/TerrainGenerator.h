#pragma once

#include "shaders/ComputeShader.h"
#include "CubeSphere.h"


class TerrainGenerator {
public:
	struct Crater {
		glm::vec4 position = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); // just use xyz
		float size = 1.0f;
		float height = 1.0f;
		float pad1[2];
	};

	TerrainGenerator();
	~TerrainGenerator();

	void ComputeBuffers(float radius);
	void ApplyTerrain(CubeSphere::Chunk* chunk);

	ComputeShader terrain_compute;
	/// Fields

	// Craters
	std::vector<Crater> craters;
	int numCraters = 0; // Default 0
	float sizeFalloff = 1.0f; // Default  1, min 1, heigher means less probable big craters
	float baseSize = 1.0f; // Default 1, the MINIMUM size a crater will have
	float sizeExaggeration = 2.0f; // Default 2

	float craterDepth = 1.5f; // Default 1.5
	float craterSteepness = 2.5f; // Default 2.5
	float craterWidth = 4.0f; // Default 4

	float rimSteepness = 2.3f; // Default 2.3
	float rimWidth = 4.2f; // Default 4.2
	float smoothingK = 0.1f; // Default 0.1

	// Noise
	glm::vec3 noiseCentre = glm::vec3(0.0f);
	int numLayers = 6; // Default 6
	float noiseStrength = 1.0f; // Default 1
	float noiseBaseFrequency = 0.1f;  // Default 0.1
	float noiseHeightShift = 0.0f; // Default 0

	// Colors
	glm::vec4 peakColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 surfaceColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);

private:
	GLuint vertBuff = 0;
	GLuint craterBuff = 0;
};