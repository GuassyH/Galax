#include "Ocean.h"

namespace Universe {

	static std::vector<Vertex> quad_verts = {
	{ {-1.0f, -1.0f, 0.0f }, {}, {}, {}, {}, { 0.0f, 0.0f }, {} },
	{ {1.0f, -1.0f, 0.0f }, {}, {}, {}, {}, { 1.0f, 0.0f }, {} },
	{ {-1.0f, 1.0f, 0.0f }, {}, {}, {}, {}, { 0.0f, 1.0f }, {} },
	{ {1.0f, 1.0f, 0.0f }, {}, {}, {}, {}, { 1.0f, 1.0f }, {} },
	};

	static std::vector<GLuint> quad_inds = {
		0, 1, 2,
		2, 1, 3
	};




	OceanRenderer::OceanRenderer() {
		oceanShader.Compile("assets/shaders/ocean.frag", "assets/shaders/ocean.vert");

		quad = Mesh(quad_verts, quad_inds);

		glGenBuffers(1, &oceanBuffer);
	}

	OceanRenderer::~OceanRenderer() {
		oceanShader.Delete();

		quad.Delete();

		glDeleteBuffers(1, &oceanBuffer);
	}

}