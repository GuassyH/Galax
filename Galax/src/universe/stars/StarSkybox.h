#pragma once

#include "Camera.h"
#include "shaders/FragShader.h"
#include "rendering/Mesh.h"

namespace Universe {

	class StarSkybox {
	public:
		struct StarInstance {
			glm::vec3 position;
			float size;
		};

		StarSkybox();
		~StarSkybox();

		void Generate(int numStars, float starSize, float sizeVariability, float starDistance);
		void Update(Transform* origin);
		void RenderStars(Camera& camera, GLuint screenTex, GLuint depthTex); // Instanced
		void RenderSkybox(Camera& camera, GLuint screenTex, GLuint depthTex); // Instanced

		std::shared_ptr<Transform> transform;

		int m_numStars = 100;
		float m_starSize = 1.0f;
		float m_starDistance = 1000.0f;
		std::vector<StarInstance> instances;

	private:
		FragShader starShader;
		FragShader skyboxShader;


		VBO instanceVBO;
		Mesh quad;
		Mesh skyboxCube;

		GLuint skyboxCubemap;
	};
};