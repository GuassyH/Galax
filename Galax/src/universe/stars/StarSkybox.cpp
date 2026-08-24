#include "StarSkybox.h"

#include "core/Maths.h"
#include "stb/stb_image.h"

#include <glad/glad.h>
#include <vector>

#include "rendering/Quad.h"

namespace Universe {
	
	std::vector<std::string> cubemap_faces{
		"assets/textures/GalaxyBackdrop2/right.png",
		"assets/textures/GalaxyBackdrop2/left.png",
		"assets/textures/GalaxyBackdrop2/top.png",
		"assets/textures/GalaxyBackdrop2/bottom.png",
		"assets/textures/GalaxyBackdrop2/front.png",
		"assets/textures/GalaxyBackdrop2/back.png",
	};

	StarSkybox::StarSkybox() {
		transform = std::make_shared<Transform>();

		starShader = FragShader("assets/shaders/universe/stars.frag", "assets/shaders/universe/stars.vert");
		skyboxShader = FragShader("assets/shaders/universe/skybox.frag", "assets/shaders/universe/skybox.vert");


		glGenTextures(1, &skyboxCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < cubemap_faces.size(); i++)
		{
			unsigned char* data = stbi_load(cubemap_faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				GLenum format;

				if (nrChannels == 1)
					format = GL_RED;
				else if (nrChannels == 3)
					format = GL_RGB;
				else if (nrChannels == 4)
					format = GL_RGBA;
				else
					format = GL_RGB;

				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					format,
					width,
					height,
					0,
					format,
					GL_UNSIGNED_BYTE,
					data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << cubemap_faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		quad = Mesh(Quad::vertices, Quad::indices);
		skyboxCube = Mesh(Cube::vertices, Cube::indices);

	}
	StarSkybox::~StarSkybox() {
		transform.reset();
		glDeleteTextures(1, &skyboxCubemap);
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

	void StarSkybox::RenderSkybox(Camera& camera, GLuint screenTex, GLuint depthTex) {
		skyboxShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubemap);
		skyboxShader.SetInt("skybox", 0);


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		skyboxShader.SetInt("baseTexture", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthTex);
		skyboxShader.SetInt("baseDepth", 2);

		skyboxShader.SetMat4("view", camera.GetView());
		skyboxShader.SetMat4("proj", camera.GetProj());

		skyboxCube.Render();
	}


	void StarSkybox::RenderStars(Camera& camera, GLuint screenTex, GLuint depthTex) {


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