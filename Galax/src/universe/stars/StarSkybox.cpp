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

		glGenBuffers(1, &starBuff);
	}
	StarSkybox::~StarSkybox() {
		transform.reset();

		starShader.Delete();

		glDeleteBuffers(1, &starBuff);
	}


	void StarSkybox::Generate(int numStars, float starSize, float sizeVariability, float starDistance) {
		m_numStars = numStars;
		m_starDistance = starDistance;
		m_starSize = starSize;

		stars.clear();
		stars.reserve(numStars);

		for (int i = 0; i < numStars; i++) {
			Star star;
			star.direction = glm::vec4(Galax::Random::PointOnUnitSphere(), 0.0f);
			star.size = Galax::Random::Range(m_starSize - (sizeVariability / 2), m_starSize + (sizeVariability / 2));

			stars.push_back(star);
		}

		GLsizeiptr starBuffSize = sizeof(Star) * stars.size();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, starBuff);
		glBufferData(GL_SHADER_STORAGE_BUFFER, starBuffSize, stars.data(), GL_STATIC_DRAW);
	}

	void StarSkybox::Update(Transform* origin) {

	}
	void StarSkybox::Render(Camera& camera, int w, int h) {

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE); // don't write depth

		starShader.Use();

		// Buffers std430
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, starBuff); // THIS IS ESSENTIAL


		starShader.SetInt("numStars", m_numStars);

		starShader.SetVec3("camForward", camera.transform->forward);
		starShader.SetVec3("camRight", camera.transform->right);
		starShader.SetVec3("camUp", camera.transform->up);

		starShader.SetFloat("FOVdeg", camera.fovDeg);

		starShader.SetInt("windowWidth", w);
		starShader.SetInt("windowHeight", h);

		// Draw
		quad.vao.Bind();
		glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE); // don't write depth

	}

}