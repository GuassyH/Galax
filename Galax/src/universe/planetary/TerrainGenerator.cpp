#include "TerrainGenerator.h"
#include <random>
#include <cmath>
#include "core/Maths.h"

TerrainGenerator::TerrainGenerator() {
	terrain_compute.Compile("assets/shaders/universe/terrain.comp");

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &craterBuff);
}


static float generate_size(float falloff, float exaggeration) {
	float x = Galax::Random::Range(0.0f, 1.0f);
	float y = glm::pow(x, falloff) * exaggeration;
	return 1 + y;
}

void TerrainGenerator::ComputeBuffers(float radius) {
	craters.clear();

	for (int i = 0; i < numCraters; i++) {
		Crater crater;
		
		crater.position = glm::vec4(Galax::Random::PointOnUnitSphere(), 0.0f) * radius;
		crater.size = generate_size(sizeFalloff, sizeExaggeration) * baseSize;
		crater.height = 1.0;

		craters.push_back(crater);
	}
}

// different thread?
void TerrainGenerator::ApplyTerrain(CubeSphere::Chunk* chunk) {

	// POSITIONS
	GLsizeiptr vertBuffSize = sizeof(Vertex) * chunk->mesh.vertices.size();
	GLsizeiptr craterBuffSize = sizeof(Crater) * numCraters;

    terrain_compute.Use();

	// Debug: ensure CPU/GPU vertex layout sizes match
	terrain_compute.SetInt("resolution", chunk->resolution);
	terrain_compute.SetFloat("radius", chunk->radius);
	terrain_compute.SetInt("numVerts", chunk->mesh.vertices.size());

	terrain_compute.SetInt("numCraters", numCraters);
	terrain_compute.SetFloat("craterDepth", craterDepth);
	terrain_compute.SetFloat("craterSteepness", craterSteepness);
	terrain_compute.SetFloat("craterWidth", craterWidth);
	terrain_compute.SetFloat("craterHeight", craterHeight);

	terrain_compute.SetFloat("rimSteepness", rimSteepness);
	terrain_compute.SetFloat("rimWidth", rimWidth);
	terrain_compute.SetFloat("smoothingK", smoothingK);

	terrain_compute.SetVec3("noiseCentre", noiseCentre);
	terrain_compute.SetInt("numLayers", numLayers);
	terrain_compute.SetFloat("noiseStrength", noiseStrength);
	terrain_compute.SetFloat("noiseBaseFrequency", noiseBaseFrequency);
	terrain_compute.SetFloat("noiseHeightShift", noiseHeightShift);
	terrain_compute.SetFloat("noiseScale", noiseScale);

	terrain_compute.SetVec4("peakColor", peakColor);
	terrain_compute.SetVec4("surfaceColor", surfaceColor);

	// Buffers std430
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vertBuffSize, chunk->mesh.vertices.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertBuff); // THIS IS ESSENTIAL

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, craterBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, craterBuffSize, craters.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, craterBuff); // THIS IS ESSENTIAL

	unsigned int workgroupSize = 256; // or 128, 512
	unsigned int numGroups = (chunk->mesh.vertices.size() + workgroupSize - 1) / workgroupSize;
	terrain_compute.Run(numGroups, 1, 1);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertBuff);

	// Ensure compute shader writes are visible to buffer mapping
	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

	// Read data back
	Vertex* cpuVerts = chunk->mesh.vertices.data();
	char* gpuData = (char*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	if (!gpuData) {
		GX_ERROR("Terrain ApplyTerrain: glMapBuffer returned null");
	} else {
		size_t stride = sizeof(Vertex);
	
		for (size_t i = 0; i < chunk->mesh.vertices.size(); ++i) {
			std::memcpy(&cpuVerts[i], gpuData + (i * stride), stride);
		}
	}


	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	chunk->mesh.Calculate();
}


TerrainGenerator::~TerrainGenerator() {
	terrain_compute.Delete();

	craters.clear();

	glDeleteBuffers(1, &vertBuff);
	glDeleteBuffers(1, &craterBuff);
}