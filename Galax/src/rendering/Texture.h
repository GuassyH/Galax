#pragma once

#include <glad/glad.h>

class Texture {
public:
	Texture() = default;
	~Texture() = default;

	Texture(const char* m_path, GLenum wrap_type = GL_REPEAT, GLenum filter_type = GL_LINEAR);
	
	void Bind();

	GLuint ID;

	int width, height, numColCh;
	const char* path;
};