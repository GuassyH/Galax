#include "Renderer.h"

#include "core/Input.h"
#include "universe/UniverseManager.h"
#include "universe/planetary/TerrainGenerator.h"




Renderer::Renderer() {

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &vertexReadbackBuffer);
	glGenBuffers(1, &craterBuffer);
	glGenBuffers(1, &noiseBuffer);

	glBindBuffer(GL_COPY_WRITE_BUFFER, vertexReadbackBuffer);
	glBufferData(GL_COPY_WRITE_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	
	// Allocate buffers and set each freeSlice vector to have one freeslice which is the entire size of the buffer

	// Allocate vertex buffer size
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_VERTICES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	FreeSlice wholeVertexSlice = { 0, MAX_VERTICES * sizeof(Vertex) };
	freeVertexSlices.push_back(wholeVertexSlice);

	// Allocate crater buffer size
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, craterBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_CRATERS * sizeof(TerrainGenerator::Crater), nullptr, GL_DYNAMIC_DRAW);
	FreeSlice wholeCraterSlice = { 0, MAX_CRATERS * sizeof(TerrainGenerator::Crater) };
	freeCraterSlices.push_back(wholeCraterSlice);

	// Allocate noise buffer size
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, noiseBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_NOISE_LAYERS * sizeof(NoiseLayer), nullptr, GL_DYNAMIC_DRAW);
	FreeSlice wholeNoiseSlice = { 0, MAX_NOISE_LAYERS * sizeof(NoiseLayer) };
	freeNoiseSlices.push_back(wholeNoiseSlice);
	
	
	atmosphereRenderer = std::make_unique<Universe::AtmosphereRenderer>();
	oceanRenderer = std::make_unique<Universe::OceanRenderer>();
	starSkybox = std::make_unique<Universe::StarSkybox>();
	starSkybox->Generate(6000, 2, 0.2f, 1000.0f);

	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}



/// KEY RULES
// Only align when creating a slice


bool Renderer::GetBufferFreeSlice(PlanetBuffer buffer, GPUslice& slice, FreeSlice& outFree, bool removeOnFind) {

	GLuint s_buffer = (buffer == PlanetBuffer::VertexBuffer ? vertexBuffer : (buffer == PlanetBuffer::CraterBuffer ? craterBuffer : noiseBuffer));
	std::vector<FreeSlice>& freeList = (buffer == PlanetBuffer::VertexBuffer ? freeVertexSlices : (buffer == PlanetBuffer::CraterBuffer ? freeCraterSlices : freeNoiseSlices));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, s_buffer);

	GLint capacity = 0;
	glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &capacity);

	GLint alignment = 0;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);


	GLsizeiptr bestSize = capacity;
	int bestFit = -1;

	for (int i = 0; i < freeList.size(); i++) {
		FreeSlice& freeSlice = freeList[i];

		// Check if the slice fits in the free slice, if it does check what the remainder is. 
		if (freeSlice.stride < slice.stride) { GX_TRACE("{} < {}", freeSlice.stride, slice.stride); continue; }

		float strideDiff = freeSlice.stride - slice.stride;

		// If the remainder is less than the "bestSize" then set bestFit to i, and set freeSlice to i
		if (strideDiff < bestSize) {
			bestFit = i;
			bestSize = strideDiff;
		}
		else {
			GX_TRACE("{} > {}", strideDiff, bestSize);
		}

	}

	if (bestFit == -1) {
		GX_ERROR("No free slice found");
		return false;
	}

	if (!removeOnFind) return true;

	FreeSlice freeSlice = freeList[bestFit];

	// Now if removeOnFind, then create a new slice where the offset is the result of Align(firstSlice.offset + the slice.stride, alignment)
	GLsizeiptr oldOffset = freeSlice.offset;
	GLsizeiptr oldStride = freeSlice.stride;

	// The stride will be equal to the remainderOffset - the freeSlice.offset to get the difference. Then set the stride to freeSlice.stride - offsetDifference
	GLsizeiptr newOffset = Align(oldOffset + slice.stride, alignment);
	GLsizeiptr newStride = oldStride - (newOffset - oldOffset);

	FreeSlice remainingFree = { newOffset, newStride };

	// Lastly remove the old free slice from the vector, and push the new free slice
	std::vector<FreeSlice> newList;
	for (int i = 0; i < freeList.size(); i++) {
		if (i == bestFit) continue;
		
		newList.push_back(freeList[i]);
	}

	newList.push_back(remainingFree);
	freeList.swap(newList);

	outFree = freeSlice;

	return true;

}

void Renderer::FreePlanetBufferSlice(PlanetBuffer buffer, GPUslice& slice) {
	if (!slice.inPool) return;

	GLuint s_buffer = (buffer == PlanetBuffer::VertexBuffer ? vertexBuffer : (buffer == PlanetBuffer::CraterBuffer ? craterBuffer : noiseBuffer));
	std::vector<FreeSlice>& freeList = (buffer == PlanetBuffer::VertexBuffer ? freeVertexSlices : (buffer == PlanetBuffer::CraterBuffer ? freeCraterSlices : freeNoiseSlices));

	// Set null values
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, s_buffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, slice.offset, slice.stride, nullptr);

	// if there is a free slice before or after the slice in the buffer, 
	// then make sure to make the new free slice the size of both this slice and the free adjacent slice
	GLsizeiptr leftBounds = slice.offset;
	GLsizeiptr rightBounds = slice.offset + slice.stride;

	int leftSliceI = -1;
	int rightSliceI = -1;

	// For every free space check if it is directly adjacent to the slice
	for (int i = 0; i < freeList.size(); i++) {
		if (freeList[i].offset == rightBounds) {
			rightSliceI = i;
		}
		if (freeList[i].offset + slice.stride == slice.offset) {
			leftSliceI = i;
		}

		// Both are set
		if (leftSliceI != -1 && rightSliceI != -1) {
			break;
		}
	}

	// Set the left and right bounds
	std::vector<FreeSlice> newList;
	for (int i = 0; i < freeList.size(); i++) {
		if (i == leftSliceI) {
			leftBounds = freeList[i].offset + freeList[i].stride;
			continue;
		}
		else if (i == rightSliceI) {
			rightBounds = freeList[i].offset;
			continue;
		}
		
		newList.push_back(freeList[i]);
	}


	// Create the new slice and add it to the list
	FreeSlice newSlice;
	newSlice.offset = leftBounds;
	newSlice.stride = rightBounds - leftBounds; // difference
	newList.push_back(newSlice);

	freeList.swap(newList);
}


bool Renderer::AddToPlanetBuffer(PlanetBuffer buffer, GPUslice& slice, const void* data) {
	GLuint s_buffer = (buffer == PlanetBuffer::VertexBuffer ? vertexBuffer : (buffer == PlanetBuffer::CraterBuffer ? craterBuffer : noiseBuffer));

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, s_buffer);

	GLint alignment = 0;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);

	GLint capacity = 0;
	glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &capacity);

	// Otherwise check if there is a free slice with room remove it from the list and set it as startpoint
	FreeSlice freeSlice;
	if (GetBufferFreeSlice(buffer, slice, freeSlice, true)) {
		slice.offset = freeSlice.offset;
		slice.inPool = true;
		
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, slice.offset, slice.stride, data);
	}
	else {
		slice.inPool = false;
		return false;
	}

	return true;
}

void Renderer::CreateFBOSet(GLuint* framebuffer, GLuint* screentex, GLuint* depthtex) {

	if (*framebuffer) glDeleteFramebuffers(1, framebuffer);
	if (*screentex) glDeleteTextures(1, screentex);
	if (*depthtex) glDeleteTextures(1, depthtex);

	glGenFramebuffers(1, framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);

	glm::vec2 window_size = Galax::InputManager::Get().windowSize;

	// Screen color texture
	glGenTextures(1, screentex);
	glBindTexture(GL_TEXTURE_2D, *screentex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window_size.x, window_size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *screentex, 0);

	// Depth texture
	glGenTextures(1, depthtex);
	glBindTexture(GL_TEXTURE_2D, *depthtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, window_size.x, window_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthtex, 0);

	// Check framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		GX_ERROR("Atmosphere Framebuffer incomplete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Renderer::~Renderer() {
	glDeleteBuffers(1, &vertexReadbackBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &craterBuffer);
	glDeleteBuffers(1, &noiseBuffer);
}