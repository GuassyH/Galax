#include "TerrainGenerator.h"
#include <random>
#include <cmath>
#include "core/Maths.h"

#include "universe/UniverseManager.h"

TerrainGenerator::TerrainGenerator() {
	terrain_compute.Compile("assets/shaders/universe/terrain.comp");
}


static float generate_size(float falloff, float exaggeration) {
	float x = Galax::Random::Range(0.0f, 1.0f);
	float y = glm::pow(x, falloff) * exaggeration;
	return 1 + y;
}

void TerrainGenerator::ComputeBuffers(Renderer& renderer, float radius) {
	craters.clear();

	for (int i = 0; i < numCraters; i++) {
		Crater crater;
		
		crater.position = glm::vec4(Galax::Random::PointOnUnitSphere(), 0.0f) * radius;
		crater.size = generate_size(sizeFalloff, sizeExaggeration) * baseSize;
		crater.height = 1.0;

		craters.push_back(crater);
	}

	if (!craterSlice.inPool) {
		craterSlice.stride = craters.size() * sizeof(Crater);

		renderer.AddToPlanetBuffer(Renderer::PlanetBuffer::Crater, craterSlice, craters.data());
	}

	if (!noiseSlice.inPool) {
		noiseSlice.stride = noiseLayers.size() * sizeof(NoiseLayer);

		renderer.AddToPlanetBuffer(Renderer::PlanetBuffer::Noise, noiseSlice, noiseLayers.data());
	}
}

// different thread?
void TerrainGenerator::ApplyTerrain(Renderer& renderer, CubeSphere::Chunk* chunk) {
	Universe::UniverseManager& universeManager = Universe::UniverseManager::Get();

	// Add chunk vertex data to the big buffer
	if (!chunk->vertexSlice.inPool) {
		chunk->vertexSlice.stride = sizeof(Vertex) * chunk->mesh.vertices.size();

		renderer.AddToPlanetBuffer(Renderer::PlanetBuffer::Vertex, chunk->vertexSlice, chunk->mesh.vertices.data());
	}

	if (!chunk->vertexSlice.inPool || !craterSlice.inPool || !noiseSlice.inPool) {
		GX_TRACE("TERRAIN FAIL");
		if (!chunk->vertexSlice.inPool) {
			GX_TRACE("{}", "Vertex");
		}
		if (!craterSlice.inPool) {
			GX_TRACE("{}", "Crater");
		}
		if (!noiseSlice.inPool) {
			GX_TRACE("{}", "Noise");
		}

		return;
	}


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

	terrain_compute.SetInt("numNoiseLayers", noiseLayers.size());

	// Bind only the parts needed from each buffer (since they hold all data from all planets)
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, renderer.vertexBuffer, chunk->vertexSlice.offset, chunk->vertexSlice.stride); // THIS IS ESSENTIAL
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, renderer.craterBuffer, craterSlice.offset, craterSlice.stride); // THIS IS ESSENTIAL
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, renderer.noiseBuffer, noiseSlice.offset, noiseSlice.stride); // THIS IS ESSENTIAL
	
	// Run compute shader
	unsigned int workgroupSize = 256; // or 128, 512
	unsigned int numGroups = (chunk->mesh.vertices.size() + workgroupSize - 1) / workgroupSize;
	terrain_compute.Run(numGroups, 1, 1);

	// Allow finish
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// Bind read and write buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer.vertexBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, renderer.vertexReadbackBuffer);

	// Copy read buffer (updated verts) to the readback buffer
	glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_WRITE_BUFFER, chunk->vertexSlice.offset, chunk->vertexSlice.offset, chunk->vertexSlice.stride);

	// Bind the write buffer and map the buffer range to data
	glBindBuffer(GL_COPY_WRITE_BUFFER, renderer.vertexReadbackBuffer);
	void* data = glMapBufferRange(GL_COPY_WRITE_BUFFER, chunk->vertexSlice.offset, chunk->vertexSlice.stride, GL_MAP_READ_BIT);

	// copy the data 
	memcpy(chunk->mesh.vertices.data(), data, chunk->vertexSlice.stride);
	glUnmapBuffer(GL_COPY_WRITE_BUFFER);


	chunk->mesh.Calculate();
}


TerrainGenerator::~TerrainGenerator() {
	terrain_compute.Delete();

	craters.clear();
}