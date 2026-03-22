#pragma once

#include "core/Log.h"

#include <iostream>

#include "rendering/VAO.h"
#include "rendering/VBO.h"
#include "rendering/EBO.h"
#include "rendering/Vertex.h"

#include <vector>
#include "Transform.h"

#include "Camera.h"
#include "shaders/Shader.h"

class Mesh {
public:
	std::unique_ptr<Transform> transform;

	VAO vao;
	VBO vbo;
	EBO ebo;

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	Mesh();
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	void Render(Camera& camera, Shader& shader);

	bool Calculate();
	void Delete();
};
