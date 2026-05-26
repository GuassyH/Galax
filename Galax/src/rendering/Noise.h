#pragma once

#include <glm/glm.hpp>

enum class NoiseType {
	Perlin,
	Voronoi
};


struct NoiseLayer {
	glm::vec3 centre = glm::vec3(0.0f); // The of the noise
	float pad0;
	NoiseType type = NoiseType::Perlin;
	float frequency = 1.0f;			// Like zooming in and out
	float intensity = 1.0f;			// How strong the value is
	int numLayers = 1;				// Number of layers that will be calculated with attenuation
	float frequencyFactor = 1.25f;	// Each layer will be multiplied by this value, recommended > 1
	float intensityFactor = 0.75f;	// Each layer will be multiplied by this value, recommended < 1
	float heightShift = 0.0f;		// Added to the result of the noise
	float pad1;
};