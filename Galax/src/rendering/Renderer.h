#pragma once


#include <glm/glm.hpp>
#include <glad/glad.h>
#include "core/Log.h"

#include <unordered_set>
#include <map>

#include "universe/atmosphere/Atmosphere.h"
#include "universe/ocean/Ocean.h"
#include "universe/stars/StarSkybox.h"

class Renderer {
public:
	Renderer();
	~Renderer();

	void CreateFBOSet(GLuint* framebuffer, GLuint* screentex, GLuint* depthtex);

	std::unique_ptr<Universe::AtmosphereRenderer> atmosphereRenderer;
	std::unique_ptr<Universe::OceanRenderer> oceanRenderer;
	std::unique_ptr<Universe::StarSkybox> starSkybox;

	struct GPUslice {
		bool inPool = false;
		GLsizeiptr offset;
		GLsizeiptr stride;
	};

	struct FreeSlice {
		GLsizeiptr offset;
		GLsizeiptr stride;
	};

	enum PlanetBuffer {
		VertexBuffer, CraterBuffer, NoiseBuffer
	};

	size_t Align(size_t value, size_t alignment){
		return (value + alignment - 1) & ~(alignment - 1);
	}

	void FreePlanetBufferSlice(PlanetBuffer buffer, GPUslice& slice);
	bool AddToPlanetBuffer(PlanetBuffer buffer, GPUslice& slice, const void* data);
	bool GetBufferFreeSlice(PlanetBuffer buffer, GPUslice& slice, FreeSlice& outFree, bool removeOnFind = false);

	GLuint vertexReadbackBuffer;
	GLuint vertexBuffer;	std::vector<FreeSlice> freeVertexSlices;
	GLuint craterBuffer;	std::vector<FreeSlice> freeCraterSlices;
	GLuint noiseBuffer;		std::vector<FreeSlice> freeNoiseSlices;
	GLuint colorMapBuffer;
};

// 128, 8, 4 megabyte of memory per buffer
#define MAX_VERTICES floor(((128 * 1E7) / 48) + 1)
#define MAX_CRATERS floor(((8 * 1E6) / 32) + 1)
#define MAX_NOISE_LAYERS floor(((4 * 1E6) / 48) + 1)
#define MAX_COLOR_MAPS floor(((4 * 1E6) / 48) + 1)