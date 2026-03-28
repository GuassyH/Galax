#include "UniverseManager.h"

#include "core/Time.h"
#include "core/Input.h"

namespace Universe {

	static double G = 0.0007;


	void UniverseManager::Init(Camera& camera) {
		atmosphereRenderer = std::make_unique<Universe::AtmosphereRenderer>();
		oceanRenderer = std::make_unique<Universe::OceanRenderer>();
		
		starSkybox = std::make_unique<Universe::StarSkybox>();
		starSkybox->Generate(3000, 3, 1.5F, 1000.0f);

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

		ResolveGravity();

		player.Look();
		player.Move();

		player.transform->UpdateMatrix();
		player.camera.UpdateMatrix();

		player.AllignToPlanet(closestPlanet, _0_1_val);

		starSkybox->Update(player.transform.get());
	}

	void CreateBuffers(GLuint* framebuffer, GLuint* screentex, GLuint* depthtex) {
		if (framebuffer) glDeleteFramebuffers(1, framebuffer);
		if (screentex) glDeleteTextures(1, screentex);
		if (depthtex) glDeleteTextures(1, depthtex);

		glGenFramebuffers(1, framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);

		glm::vec2 window_size = Galax::InputManager::Get().windowSize;

		// Screen color texture
		glGenTextures(1, screentex);
		glBindTexture(GL_TEXTURE_2D, *screentex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, window_size.x, window_size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *screentex, 0);

		// Depth texture
		glGenTextures(1, depthtex);
		glBindTexture(GL_TEXTURE_2D, *depthtex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, window_size.x, window_size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthtex, 0);

		// Check framebuffer
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			GX_ERROR("Atmosphere Framebuffer incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void UniverseManager::Render(Camera& camera, Planet* sun) {
		glm::vec2 window_size = Galax::InputManager::Get().windowSize;

		// if the resolution has changed remake the framebuffers and textures
		if (window_size.x != last_width || window_size.y != last_height) {
			CreateBuffers(&baseFBO, &baseTexture, &baseDepth);
			CreateBuffers(&starFBO, &starTexture, &starDepth);


			last_width = window_size.x;
			last_height = window_size.y;
		}
		

		// Render stars
		glBindFramebuffer(GL_FRAMEBUFFER, starFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, window_size.x, window_size.y);

		// Dont test the depth, but allow writing
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		starSkybox->Render(camera);

		// Render planets
		glBindFramebuffer(GL_FRAMEBUFFER, baseFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Do test depth and do write if the depth is less, ENABLE, TRUE, LESS
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		for (auto& planet : planets)
			planet->Render(camera, sun);

		// Do test depth, do, always, write depth DONT TOUCH!!!, ENABLE, TRUE, LESS
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	
		for (auto& planet : planets)
			if (planet->hasOcean) oceanRenderer->Render(camera, sun, planet->transform.get(), planet->ocean_config, baseTexture, baseDepth);

		// Write to the atmosphere FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Do not test depth, do not write depth DONT TOUCH!!! DISABLE, FALSE
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		for (auto& planet : planets)
		if (planet->hasAtmosphere) atmosphereRenderer->Render(camera, sun, planet->transform.get(), planet->atmosphere_config, baseTexture, baseDepth, starTexture);

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

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




	void UniverseManager::ResolveGravity() {
		// Add velocity
		for (int i = 0; i < planets.size(); i++) {
			for (int j = 0; j < planets.size(); j++) {
				if (i == j)
					continue;

				Planet* thisP = planets[i].get();
				Planet* otherP = planets[j].get();

				if (!thisP || !otherP)
					continue;


				// F1 = G(m1 * m2) / d^2 = m * a 
				// a1 = (G(m1 * m2) / d^2) / m1
				// a1 = G(m2) / d^2
				float dstSq = glm::distance2(thisP->transform->world_position, otherP->transform->world_position);
				float acceleration = G * otherP->physicsBody.mass / dstSq;

				glm::vec3 direction = glm::normalize(otherP->transform->world_position - thisP->transform->world_position);

				thisP->physicsBody.velocity += direction * acceleration * Galax::Time::get().deltaTime;
			}
		}

		// Add position
		for (auto planet : planets) {
			planet->transform->local_position += planet->physicsBody.velocity * Galax::Time::get().deltaTime;
		}
	}

	void UniverseManager::SetIdealOrbitVelocity(Planet* planet, Planet* target) {
		if (!planet || !target || planet == target)
			return;


		planet->physicsBody.velocity = glm::vec3(0.0); // make sure its already still

		glm::vec3 offset = target->transform->world_position - planet->transform->world_position;
		float r = glm::length(offset);

		float orbitalSpeed = glm::sqrt(G * target->physicsBody.mass / r);

		glm::vec3 forward = glm::normalize(offset);
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

		if (glm::abs(glm::dot(forward, up)) > 0.99f)
			up = glm::vec3(1.0, 0.0, 0.0);

		glm::vec3 right = glm::normalize(glm::cross(forward, up));

		planet->physicsBody.velocity = right * orbitalSpeed;
	}




	/*
	/// Draw the path the planet will take in the next numPoints, using the current velocity and gravity. 
	/// Used for debugging and visualization, not performant.
	void DrawPredictedPath(Planet* planet, int numPoints, float timeStep) {
		std::vector<glm::vec3> points;
		glm::vec3 position = planet->transform->world_position;
		glm::vec3 velocity = planet->physicsBody.velocity;

		// Calculate the path points
		for (int i = 0; i < numPoints; i++) {
			// Calculate gravity from other planets
			glm::vec3 acceleration(0.0f);
			for (auto& otherP : UniverseManager::Get().planets) {
				if (otherP.get() == planet)
					continue;
				float dstSq = glm::distance2(position, otherP->transform->world_position);
				if (dstSq < 0.0001f) // Avoid singularity
					continue;

				acceleration += G * otherP->physicsBody.mass / dstSq;
			}
			// Update velocity and position
			velocity += acceleration * timeStep;
			position += velocity * timeStep;
			points.push_back(position);
		}

		// Do something else? Like render lines between the points or something, not sure how to do that yet
	}
	*/

};