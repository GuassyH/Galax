#pragma once

#include <glad/glad.h>

class VAO {
public:
	~VAO() = default;

	GLuint ID;

	VAO() { glGenVertexArrays(1, &ID); }
	void Bind() { glBindVertexArray(ID); }
	void Unbind() { glBindVertexArray(0); }
	void Delete() { glDeleteVertexArrays(1, &ID); }

	void LinkAttrib(unsigned int location, unsigned int numElements, unsigned int type, size_t stride, unsigned int offset) {
		Bind();
		glVertexAttribPointer(location, numElements, type, GL_FALSE, stride, (void*)(offset));
		glEnableVertexAttribArray(location);
	}

};