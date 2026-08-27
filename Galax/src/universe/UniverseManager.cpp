#include "UniverseManager.h"

#include "core/Time.h"
#include "core/Input.h"

#include "rendering/Quad.h"

namespace Universe {

	void UniverseManager::Init(Renderer& renderer, Camera& camera) {
		transform = std::make_shared<Transform>();
		transform->UpdateMatrix(false);

		composite_quad = Mesh(Quad::vertices, Quad::indices);
		composite_shader = std::make_unique<FragShader>("assets/shaders/composite.frag", "assets/shaders/composite.vert");

	}

	void UniverseManager::Update(Renderer& renderer, Player& player) {
		// Recentre the universe around the player
		Recentre(player);

		for (auto& planet : planets)
			planet->Update(renderer, player.camera, isSimulating);

		Universe::Planet* closestPlanet = nullptr;
		float _0_1_val = 0.0f;

		// Check for the closest planet
		for (auto& planet : planets) {
			// Use squares since its much faster than getting the roots
			float distance_sqr = glm::distance2(player.transform->world_position, planet->transform->world_position);
			float threshold_sqr = planet->radius * planet->radius * 4.0f;
			if (distance_sqr < threshold_sqr) {
				closestPlanet = planet.get();
				_0_1_val = glm::sqrt(distance_sqr) / (planet->radius * 2.0f);
				break;
			}
		}

		if (isSimulating) {
			ResolveGravity();
			time += Galax::Time::Get().deltaTime * Galax::Time::Get().timeScale;
		}

		player.Look();
		player.Move();

		// Update to make sure its accurate for allignment
		player.transform->UpdateMatrix();
		player.camera.UpdateMatrix();

		player.AllignToPlanet(closestPlanet, _0_1_val);

		// Update to make sure its accurate for rendering (transform is updated in allign)
		player.camera.UpdateMatrix();

		renderer.starSkybox->Update(player.transform.get());
		
	}

	

	void UniverseManager::Render(Renderer& renderer, Camera& camera, Planet* sun) {
		glm::vec2 window_size = Galax::InputManager::Get().windowSize;

		// if the resolution has changed remake the framebuffers and textures
		if (window_size.x != last_width || window_size.y != last_height) {
			renderer.CreateFBOSet(&baseFBO, &baseTexture, &baseDepth);
			renderer.CreateFBOSet(&starFBO, &starTexture, &starDepth);
			renderer.CreateFBOSet(&skyboxFBO, &skyboxTexture, &skyboxDepth);

			last_width = window_size.x;
			last_height = window_size.y;
		}


		// Reverse the depth, and go from 0 -> 1, instead of -1 -> 1
		glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
		glClearDepth(0.0);

		// Render planets
		glBindFramebuffer(GL_FRAMEBUFFER, baseFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, window_size.x, window_size.y);
		
		// DRAW PLANETS
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_GREATER); // less
		for (auto& planet : planets)
			planet->Render(renderer, camera, sun);

		// DRAW OCEANS
		for (auto& planet : planets)
			if (planet->hasOcean) renderer.oceanRenderer->Render(camera, sun, planet.get(), planet->ocean_config, baseTexture, baseDepth);

		// DRAW ATMOSPHERES
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_NEVER); // always
		for (auto& planet : planets)
			if (planet->hasAtmosphere) renderer.atmosphereRenderer->Render(camera, sun, planet.get(), planet->atmosphere_config, baseTexture, baseDepth);

		// DRAW STARS
		glBindFramebuffer(GL_FRAMEBUFFER, skyboxFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_GREATER);

		renderer.starSkybox->RenderSkybox(camera, baseTexture, baseDepth);

		// DRAW STARS
		glBindFramebuffer(GL_FRAMEBUFFER, starFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_GREATER);

		renderer.starSkybox->RenderStars(camera, baseTexture, baseDepth);

		// COMPOSITE
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDepthMask(GL_TRUE);

		composite_shader->Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, baseTexture);
		composite_shader->SetInt("baseTexture", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, baseDepth);
		composite_shader->SetInt("baseDepth", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, starTexture);
		composite_shader->SetInt("starTexture", 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, skyboxTexture);
		composite_shader->SetInt("skyboxTexture", 3);

		composite_quad.Render();
	}

	void UniverseManager::Shutdown(Renderer& renderer) {
		for (auto& planet : planets) {
			planet->Delete(renderer);
		}

		if (baseFBO) glDeleteFramebuffers(1, &baseFBO);
		if (baseTexture) glDeleteTextures(1, &baseTexture);
		if (baseDepth) glDeleteTextures(1, &baseDepth);


		if (starFBO) glDeleteFramebuffers(1, &starFBO);
		if (starTexture) glDeleteTextures(1, &starTexture);
		if (starDepth) glDeleteTextures(1, &starDepth);


		if (skyboxFBO) glDeleteFramebuffers(1, &skyboxFBO);
		if (skyboxTexture) glDeleteTextures(1, &skyboxTexture);
		if (skyboxDepth) glDeleteTextures(1, &skyboxDepth);

		composite_quad.Delete();
		composite_shader->Delete();
	}



	void UniverseManager::Recentre(Player& player) {
		float player_dst = glm::distance(glm::vec3(0.0f), player.transform->world_position);

		if (player_dst > recentre_dist) {

			// Set the position of the universes transform
			transform->local_position -= player.transform->world_position;

			// Only set players position if it has no parent, since if it does it will automatically recentre
			if (!player.transform->HasParent()) {
				player.transform->local_position -= player.transform->world_position;
			}

			// DONT update children since they WILL be updated anyways in void Update()
			transform->UpdateMatrix(false);
		}
	}



	// The sun is not affecting the middle body?
	void UniverseManager::ResolveGravity() {
		// Add velocity
		std::vector<glm::vec3> accelerations(planets.size(), glm::vec3(0.0f));

		for (int i = 0; i < planets.size(); i++) {
			for (int j = 0; j < planets.size(); j++) {
				if (i == j)
					continue;

				Planet* thisP = planets[i].get();
				Planet* otherP = planets[j].get();

				if (!thisP || !otherP)
					continue;

				// GX_TRACE("\'{}\' affecting \'{}\'", thisP->name, otherP->name);

				// F1 = G(m1 * m2) / d^2 = m * a 
				// a1 = (G(m1 * m2) / d^2) / m1
				// a1 = G(m2) / d^2
				float dstSq = glm::distance2(thisP->transform->world_position, otherP->transform->world_position);
				float acceleration = G * otherP->physicsBody.mass / dstSq;
				glm::vec3 direction = glm::normalize(otherP->transform->world_position - thisP->transform->world_position);

				accelerations[i] += direction * acceleration;
			}
		}

		// Add position
		for (int i = 0; i < planets.size(); i++) {
			planets[i]->physicsBody.velocity += accelerations[i] * Galax::Time::Get().deltaTime * Galax::Time::Get().timeScale;
			planets[i]->transform->local_position += planets[i]->physicsBody.velocity * Galax::Time::Get().deltaTime * Galax::Time::Get().timeScale;
		}
	}


	void UniverseManager::SetIdealOrbitVelocity(Planet* planet, Planet* target) {
		if (!planet || !target || planet == target)
			return;

		glm::vec3 planetPos = planet->transform->world_position;
		glm::vec3 targetPos = target->transform->world_position;

		// Direction from planet -> target
		glm::vec3 radialDir = glm::normalize(targetPos - planetPos);

		// --- 1. Compute net gravitational acceleration at planet ---
		glm::vec3 netAcceleration(0.0f);

		for (auto& other : planets) {
			if (!other || other.get() == planet)
				continue;

			glm::vec3 dir = other->transform->world_position - planetPos;
			float distSq = glm::dot(dir, dir);

			if (distSq < 0.0001f) continue;

			float accel = G * other->physicsBody.mass / distSq;
			netAcceleration += glm::normalize(dir) * accel;
		}

		float accelMag = glm::length(netAcceleration);
		if (accelMag < 0.00001f)
			return;

		// --- 2. Distance to target (defines orbit radius) ---
		float r = glm::distance(planetPos, targetPos);

		// --- 3. Compute orbital speed based on total acceleration ---
		float orbitalSpeed = glm::sqrt(accelMag * r);

		// --- 4. Find tangent direction (perpendicular to acceleration) ---
		glm::vec3 accelDir = glm::normalize(netAcceleration);

		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		if (glm::abs(glm::dot(accelDir, up)) > 0.99f)
			up = glm::vec3(1.0f, 0.0f, 0.0f);

		glm::vec3 tangent = glm::normalize(glm::cross(accelDir, up));

		// --- 5. Set velocity relative to target motion ---
		planet->physicsBody.velocity = target->physicsBody.velocity + tangent * orbitalSpeed;
	}

};