#include "Ocean.h"
#include "universe/planetary/Planet.h"

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
	}

	OceanRenderer::~OceanRenderer() {
		oceanShader.Delete();

		quad.Delete();
	}

	void OceanRenderer::Render(Camera& camera, Universe::Planet* sun, Transform* planet, OceanConfig& ocean_config, GLuint colorTex, GLuint depthTex, int w, int h) {
		oceanShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		oceanShader.SetInt("depthTexure", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, colorTex);
		oceanShader.SetInt("screenTexture", colorTex);

		oceanShader.SetFloat("oceanRadius", ocean_config.radius);
		oceanShader.SetFloat("densityFalloff", ocean_config.densityFalloff);
		oceanShader.SetVec3("centre", planet->world_position);

		oceanShader.SetVec3("camPos", camera.transform->world_position);
		oceanShader.SetVec3("sunPos", sun->transform->world_position);

		oceanShader.SetVec2("screenResolution", glm::vec2(w, h));

		oceanShader.SetVec3("camUp", camera.transform->up);
		oceanShader.SetVec3("camForward", camera.transform->forward);
		oceanShader.SetVec3("camRight", camera.transform->right);
		oceanShader.SetFloat("camFarPlane", camera.farPlane);
		oceanShader.SetFloat("camNearPlane", camera.nearPlane);

		oceanShader.SetFloat("FOVdeg", camera.fovDeg);

		oceanShader.SetMat4("viewMat", camera.GetView());
		oceanShader.SetMat4("projMat", camera.GetProj());


		quad.vao.Bind();
		glDrawElements(GL_TRIANGLES, quad.indices.size(), GL_UNSIGNED_INT, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}

}