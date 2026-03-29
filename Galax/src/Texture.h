#pragma once

#include <glad/glad.h>

class Texture {
public:
	Texture() = default;
	~Texture() = default;

	Texture(const char* m_path);
	
	void Bind();

	GLuint ID;

	int width, height, numColCh;
	const char* path;
};