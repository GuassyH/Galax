#include "UniverseManager.h"




namespace Universe {

	void UniverseManager::Init(Camera& camera) {
		atmosphereRenderer = std::make_unique<Universe::AtmosphereRenderer>();
		
		starSkybox = std::make_unique<Universe::StarSkybox>();
		starSkybox->Generate(3000, 60, 24, camera.farPlane * 0.8f);
	}

	void UniverseManager::Update(Player& player) {

		for (auto& planet : planets)
			planet->Update(player.camera);

		Universe::Planet* closestPlanet = nullptr;
		float _0_1_val = 0.0f;

		for (auto& planet : planets) {
			float distance = glm::distance(player.transform->world_position, planet->transform->world_position);
			if (distance < planet->radius * 2) {
				closestPlanet = planet.get();
				_0_1_val = distance / (planet->radius * 2.0f);
				break;
			}
		}

		player.AllignToPlanet(closestPlanet, _0_1_val);

		starSkybox->Update(player.transform.get());
	}

	void CreateBuffers(GLuint* framebuffer, GLuint* screentex, GLuint* depthtex, int w, int h) {
		if (framebuffer) glDeleteFramebuffers(1, framebuffer);
		if (screentex) glDeleteTextures(1, screentex);
		if (depthtex) glDeleteTextures(1, depthtex);

		glGenFramebuffers(1, framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);

		// Screen color texture
		glGenTextures(1, screentex);
		glBindTexture(GL_TEXTURE_2D, *screentex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *screentex, 0);

		// Depth texture
		glGenTextures(1, depthtex);
		glBindTexture(GL_TEXTURE_2D, *depthtex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthtex, 0);

		// Check framebuffer
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			GX_ERROR("Atmosphere Framebuffer incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void UniverseManager::Render(Camera& camera, Planet* sun, int w, int h) {

		if (w != last_width || h != last_height) {
			CreateBuffers(&baseFBO, &baseTexture, &baseDepth, w, h);
			CreateBuffers(&starFBO, &starTexture, &starDepth, w, h);

			last_width = w;
			last_height = h;
		}
			

		// Write to the base FBO
		glBindFramebuffer(GL_FRAMEBUFFER, starFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, w, h);

		starSkybox->Render(camera);

		
		glBindFramebuffer(GL_FRAMEBUFFER, baseFBO);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& planet : planets)
			planet->Render(camera);

		// Write to the atmosphere FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		atmosphereRenderer->Render(camera, sun, baseTexture, starTexture, baseDepth, w, h);
		
	}

	void UniverseManager::Shutdown() {
		for (auto& planet : planets) {
			planet->Delete();
		}

		if (baseFBO) glDeleteFramebuffers(1, &baseFBO);
		if (baseTexture) glDeleteTextures(1, &baseTexture);
		if (baseDepth) glDeleteTextures(1, &baseDepth);


		if (starFBO) glDeleteFramebuffers(1, &starFBO);
		if (starTexture) glDeleteTextures(1, &starTexture);
		if (starDepth) glDeleteTextures(1, &starDepth);

	}

};