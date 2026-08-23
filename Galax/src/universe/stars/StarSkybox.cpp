#include "StarSkybox.h"

#include "core/Maths.h"

#include <glad/glad.h>
#include <vector>

#include "rendering/Quad.h"

namespace Universe {
	
	StarSkybox::StarSkybox() {
		transform = std::make_shared<Transform>();

		starShader = FragShader("assets/shaders/universe/stars.frag", "assets/shaders/universe/stars.vert");

		quad = Mesh(Quad::vertices, Quad::indices);

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
		quad.vao.LinkAttrib(3, 1, GL_FLOAT, sizeof(StarInstance), offsetof(StarInstance, size));

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(3, 1);
	
		quad.vao.Unbind();
		instanceVBO.Unbind();
	}

	void StarSkybox::Update(Transform* origin) {

	}
	void StarSkybox::Render(Camera& camera, GLuint screenTex, GLuint depthTex) {

		starShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		starShader.SetInt("baseTexture", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		starShader.SetInt("baseDepth", 1);

		starShader.SetMat4("view", camera.GetView());
		starShader.SetMat4("proj", camera.GetProj());
		starShader.SetVec3("origo", camera.transform->world_position);
		starShader.SetFloat("fov", camera.fovDeg);

		// Draw
		quad.vao.Bind();
		glDrawElementsInstanced(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0, instances.size());

	}

}