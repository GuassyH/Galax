#pragma once

#include <glad/glad.h>

class VBO {
public:
	~VBO() = default;

	GLuint ID;

	VBO() { glGenBuffers(1, &ID); }
	void Bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }
	void Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void Delete() { glDeleteBuffers(1, &ID); }

	void BindBufferData(size_t size, const void* data) {
		Bind();
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

};