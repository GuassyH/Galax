#pragma once

#include <glad/glad.h>

class EBO {
public:
	~EBO() = default;

	GLuint ID;

	EBO() { glGenBuffers(1, &ID); }
	void Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
	void Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	void Delete() { glDeleteBuffers(1, &ID); }

	void BindBufferData(size_t size, const void* data) {
		Bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
};