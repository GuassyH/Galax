#include "Quad.h"



std::vector<Vertex> Quad::vertices = {
{ {-1.0f, -1.0f, 0.0f }, {}, {}, {}, { 0.0f, 0.0f }, {} },
{ {1.0f, -1.0f, 0.0f }, {}, {}, {}, { 1.0f, 0.0f }, {} },
{ {-1.0f, 1.0f, 0.0f }, {}, {}, {}, { 0.0f, 1.0f }, {} },
{ {1.0f, 1.0f, 0.0f }, {}, {}, {}, { 1.0f, 1.0f }, {} },
};

std::vector<GLuint> Quad::indices = {
	0, 1, 2,
	2, 1, 3
};