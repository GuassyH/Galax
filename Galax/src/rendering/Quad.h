#pragma once

#include <iostream>
#include <glad/glad.h>
#include "Vertex.h"

class Quad {
public:
	static std::vector<Vertex> vertices;
	static std::vector<GLuint> indices;
};

