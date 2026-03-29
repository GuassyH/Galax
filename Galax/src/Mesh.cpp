#include "Mesh.h"

Mesh::Mesh() {
	vertices = std::vector<Vertex>();
	indices = std::vector<unsigned int>();
	transform = std::make_unique<Transform>();
	// GX_TRACE("Mesh created with no vertices or indices");
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
	this->vertices = vertices;
	this->indices = indices;
	transform = std::make_unique<Transform>();
	Calculate();
}

bool Mesh::Calculate() {
	if (vertices.empty() || indices.empty()) {
		GX_ERROR("Mesh has no vertices or indices");
		return false;
	}

	vbo.BindBufferData(sizeof(Vertex) * vertices.size(), vertices.data());

	vao.LinkAttrib(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao.LinkAttrib(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
	vao.LinkAttrib(2, 4, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, color));
	vao.LinkAttrib(3, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));

	ebo.BindBufferData(sizeof(GLuint) * indices.size(), indices.data());

	vbo.Unbind();
	vao.Unbind();
	ebo.Unbind();

	return true;
}

// Transform matrix must be updated first, with planets its done automatically
void Mesh::Render(Camera& camera) { 
	vao.Bind();
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}


void Mesh::Delete() {
	vao.Delete();
	vbo.Delete();
	ebo.Delete();

	transform->SetParent(nullptr);
	transform.reset();
}