#include "TerrainGenerator.h"
#include <random>

TerrainGenerator::TerrainGenerator() {
	terrain_compute.Compile("assets/shaders/terrain.comp");
	normals_compute.Compile("assets/shaders/normals.comp");

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &indBuff);
	glGenBuffers(1, &craterBuff);
}

// random float between 0 and 1
static float random() { 

	std::random_device rd;              // seed
	std::mt19937 gen(rd());             // Mersenne Twister RNG
	std::uniform_real_distribution<> dis(0.0, 1.0);

	return dis(gen);
}

static float generate_size(float falloff) {
	float x = random();
	x = glm::clamp(x, 0.1f, 1.0f);

	float y = glm::pow(x, falloff);
	return y;
}

void TerrainGenerator::ComputeBuffers(float radius) {
	craters.clear();

	for (int i = 0; i < numCraters; i++) {
		Crater crater{};


		// randomise values
		float x = (random() * 2.0f) - 1.0f;
		float y = (random() * 2.0f) - 1.0f;
		float z = (random() * 2.0f) - 1.0f;
		
		crater.position = glm::vec4(glm::normalize(glm::vec3(x, y, z)), 0.0f) * radius;

		crater.craterDepth = -1.0f;
		crater.craterSteepness = 0.1f;
		crater.craterWidth = 5.0f;

		crater.rimSteepness = 5.0f;
		crater.rimWidth = 2.0f;

		crater.smoothingK = 0.1f;

		crater.size = generate_size(falloff);
		// crater.size = 1;
		GX_TRACE("{}", crater.size);


		craters.push_back(crater);
	}
}

void TerrainGenerator::ApplyTerrain(CubeSphere::Chunk* chunk) {

	// POSITIONS
	GLsizeiptr vertBuffSize = sizeof(Vertex) * chunk->mesh.vertices.size();
	GLsizeiptr craterBuffSize = sizeof(Crater) * numCraters;

	terrain_compute.Use();

	terrain_compute.SetInt("resolution", chunk->resolution);
	terrain_compute.SetInt("numVerts", chunk->mesh.vertices.size());
	terrain_compute.SetInt("numCraters", numCraters);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vertBuffSize, chunk->mesh.vertices.data(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertBuff); // THIS IS ESSENTIAL

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, craterBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, craterBuffSize, craters.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, craterBuff); // THIS IS ESSENTIAL

	unsigned int workgroupSize = 256; // or 128, 512
	unsigned int numGroups = (chunk->mesh.vertices.size() + workgroupSize - 1) / workgroupSize;
	terrain_compute.Run(numGroups, 1, 1);

	// NORMALS

	normals_compute.Use();
	normals_compute.SetInt("numTriangles", (chunk->resolution + 1) * (chunk->resolution + 1) * 2);
	normals_compute.SetInt("numVerts", chunk->mesh.vertices.size());

	GLsizeiptr indBuffSize = sizeof(unsigned int) * chunk->mesh.indices.size();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertBuff); // THIS IS ESSENTIAL

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indBuff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, indBuffSize, chunk->mesh.indices.data(), GL_STATIC_READ);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indBuff); // THIS IS ESSENTIAL

	normals_compute.Run(numGroups, 1, 1);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertBuff);

	// Read data back
	Vertex* cpuVerts = chunk->mesh.vertices.data();
	char* gpuData = (char*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	if (!gpuData) { /* handle error */ }

	for (size_t i = 0; i < chunk->mesh.vertices.size(); ++i) {
		std::memcpy(&cpuVerts[i].position, gpuData + (i * 48) + 0, sizeof(cpuVerts[i].position));
		std::memcpy(&cpuVerts[i].normal, gpuData + (i * 48) + 16, sizeof(cpuVerts[i].normal));
		std::memcpy(&cpuVerts[i].texCoord, gpuData + (i * 48) + 32, sizeof(cpuVerts[i].texCoord));
	}


	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	chunk->mesh.Calculate();
}


TerrainGenerator::~TerrainGenerator() {
	terrain_compute.Delete();
	normals_compute.Delete();

	glDeleteBuffers(1, &vertBuff);
	glDeleteBuffers(1, &indBuff);
	glDeleteBuffers(1, &craterBuff);
}