#pragma once

#include <glm/glm.hpp>

enum class NoiseType {
	Perlin,
	Voronoi
};


struct NoiseLayer {
	glm::vec3 centre = glm::vec3(0.0f); // The of the noise
	NoiseType type = NoiseType::Perlin;
	float frequency = 1.0f;			// Like zooming in and out
	float intensity = 1.0f;			// How strong the value is
	int numLayers = 1;				// Number of layers that will be calculated with attenuation
	float frequencyFactor = 1.25f;	// Each layer will be multiplied by this value, recommended > 1
	float intensityFactor = 0.75f;	// Each layer will be multiplied by this value, recommended < 1
	float heightShift = 0.0f;		// Added to the result of the noise
	float wobble = 0.0f;
	float wobble_scale = 0.0f;
	float power = 1.0f;				// In voronoi it raises to this power, thereby making it sharper. Otherwise its a pad
	float minMask = 0.0f;			// dst to centre, not surf
	float pad2;
	float pad3;
};

inline NoiseLayer NewNoiseLayer(
	glm::vec3 centre = glm::vec3(0.0f),
	NoiseType type = NoiseType::Perlin,
	float frequency = 1.0f, 
	float intensity = 1.0f, 
	int numLayers = 1, 
	float frequencyFactor = 1.25f, 
	float intensityFactor = 0.75f, 
	float heightShift = 0.0f,
	float wobble = 0.0f,
	float wobble_scale = 0.0f,
	float power = 0.0f,
	float minMask = 0.0f) {
	return NoiseLayer{ centre, type, frequency, intensity, numLayers, frequencyFactor, intensityFactor, heightShift, wobble, wobble_scale, power, minMask };
}