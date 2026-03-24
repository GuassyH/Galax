#include "StarSkybox.h"

#include "core/Maths.h"

#include <glad/glad.h>
#include <vector>

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

	
	StarSkybox::StarSkybox() {
		transform = std::make_shared<Transform>();

		starShader = FragShader("assets/shaders/stars.frag", "assets/shaders/stars.vert");

		quad = Mesh(quad_verts, quad_inds);

	}
	StarSkybox::~StarSkybox() {
		transform.reset();

		starShader.Delete();
	}


	void StarSkybox::Generate(int numStars, float starSize, float sizeVariability, float starDistance) {
		m_numStars = numStars;
		m_starDistance = starDistance;
		m_starSize = starSize;

		instances.clear();
		instances.reserve(numStars);

		instanceVBO = VBO();

		for (int i = 0; i < numStars; i++) {
			StarInstance star;
			star.position = Galax::Random::PointOnUnitSphere() * starDistance;
			star.size = Galax::Random::Range(m_starSize - (sizeVariability / 2), m_starSize + (sizeVariability / 2));

			instances.push_back(star);
		}

		instanceVBO.Bind();
		instanceVBO.BindBufferData(instances.size() * sizeof(StarInstance), instances.data());

		quad.vao.Bind();
		instanceVBO.Bind();
		quad.vao.LinkAttrib(1, 3, GL_FLOAT, sizeof(StarInstance), 0);
		quad.vao.LinkAttrib(2, 1, GL_FLOAT, sizeof(StarInstance), offsetof(StarInstance, size));

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
	
		quad.vao.Unbind();
		instanceVBO.Unbind();
	}

	void StarSkybox::Update(Transform* origin) {

	}
	void StarSkybox::Render(Camera& camera) {

		glDepthMask(GL_FALSE); // don't write depth

		starShader.Use();

		starShader.SetMat4("view", camera.GetView());
		starShader.SetMat4("proj", camera.GetProj());
		starShader.SetVec3("origo", camera.transform->world_position);

		// Draw
		quad.vao.Bind();
		glDrawElementsInstanced(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0, instances.size());

		glDepthMask(GL_TRUE); // don't write depth
	}

}