#include "TerrainGenerator.h"
#include <random>
#include <cmath>

TerrainGenerator::TerrainGenerator() {
	terrain_compute.Compile("assets/shaders/terrain.comp");

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &craterBuff);
}


static thread_local std::mt19937 rng(std::random_device{}());
// random float between 0 and 1
static std::uniform_real_distribution<float> random(0.0f, 1.0f);


static glm::vec3 randomPointOnUnitSphere() {
	// Random number generators

	double theta = random(rng) * 2.0 * 3.14159265;        // azimuthal angle
	double cosPhi = (random(rng) * 2.0) - 1.0;      // cos(polar angle)
	double phi = acos(cosPhi);

	double x = sin(phi) * cos(theta);
	double y = sin(phi) * sin(theta);
	double z = cosPhi;

	return glm::vec3(x, y, z);
}

static float generate_size(float falloff, float exaggeration) {
	float x = random(rng);

	float y = glm::pow(x, falloff) * exaggeration;

	return 1 + y;
}

void TerrainGenerator::ComputeBuffers(float radius) {
	craters.clear();

	for (int i = 0; i < numCraters; i++) {
		Crater crater;
		
		crater.position = glm::vec4(randomPointOnUnitSphere(), 0.0f) * radius;
		crater.size = generate_size(sizeFalloff, sizeExaggeration) * baseSize;

		craters.push_back(crater);
	}
}

// different thread?
void TerrainGenerator::ApplyTerrain(CubeSphere::Chunk* chunk) {

	// POSITIONS
	GLsizeiptr vertBuffSize = sizeof(Vertex) * chunk->mesh.vertices.size();
	GLsizeiptr craterBuffSize = sizeof(Crater) * numCraters;

	terrain_compute.Use();

	terrain_compute.SetInt("resolution", chunk->resolution);
	terrain_compute.SetFloat("radius", chunk->radius);
	terrain_compute.SetInt("numVerts", chunk->mesh.vertices.size());

	terrain_compute.SetInt("numCraters", numCraters);
	terrain_compute.SetFloat("craterDepth", craterDepth);
	terrain_compute.SetFloat("craterSteepness", craterSteepness);
	terrain_compute.SetFloat("craterWidth", craterWidth);

	terrain_compute.SetFloat("rimSteepness", rimSteepness);
	terrain_compute.SetFloat("rimWidth", rimWidth);
	terrain_compute.SetFloat("smoothingK", smoothingK);



	terrain_compute.SetVec3("noiseCentre", noiseCentre);
	terrain_compute.SetInt("numLayers", numLayers);
	terrain_compute.SetFloat("noiseStrength", noiseStrength);
	terrain_compute.SetFloat("noiseBaseFrequency", noiseBaseFrequency);
	terrain_compute.SetFloat("noiseHeightShift", noiseHeightShift);

	terrain_compute.SetVec4("peakColor", peakColor);
	terrain_compute.SetVec4("surfaceColor", surfaceColor);

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

	// Read data back
	Vertex* cpuVerts = chunk->mesh.vertices.data();
	char* gpuData = (char*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	if (!gpuData) { /* handle error */ }

	for (size_t i = 0; i < chunk->mesh.vertices.size(); ++i) {
		std::memcpy(&cpuVerts[i].position, gpuData + (i * 64) + 0, sizeof(cpuVerts[i].position));
		std::memcpy(&cpuVerts[i].normal, gpuData + (i * 64) + 16, sizeof(cpuVerts[i].normal));
		std::memcpy(&cpuVerts[i].color, gpuData + (i * 64) + 32, sizeof(cpuVerts[i].color));
		std::memcpy(&cpuVerts[i].texCoord, gpuData + (i * 64) + 48, sizeof(cpuVerts[i].texCoord));
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