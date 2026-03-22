#pragma once

#include "Camera.h"
#include "shaders/FragShader.h"
#include "Mesh.h"

namespace Universe {

	class StarSkybox {
	public:
		struct Star {
			glm::vec4 direction;
			float pad0[3];
			float size;
		};

		StarSkybox();
		~StarSkybox();

		void Generate(int numStars, float starSize, float sizeVariability, float starDistance);
		void Update(Transform* origin);
		void Render(Camera& camera, int w, int h); // Instanced

		std::shared_ptr<Transform> transform;

		int m_numStars = 100;
		float m_starSize = 1.0f;
		float m_starDistance = 1000.0f;
		std::vector<Star> stars;

	private:
		GLuint starBuff = 0;
		FragShader starShader;

		Mesh quad;
	};
};