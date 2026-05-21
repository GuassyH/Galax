#include "UniverseDebug.h"
#include "UniverseManager.h"

#include "core/Time.h"

namespace Universe {

	void UniverseDebug::Init() {
		pathShader = std::make_shared<FragShader>("assets/shaders/debug/debug_path.frag", "assets/shaders/debug/debug_path.vert");
		
		pathVAO = VAO();
		pathVBO = VBO();
	}


	void UniverseDebug::Update(Player& player) {

	}

	void UniverseDebug::Render(Player& player) {
		for (auto planet : Universe::UniverseManager::Get().GetPlanets()) {
			if (planet->physicsBody.debug_path) {
				DrawPredictedPath(player, planet.get(), planet->physicsBody.debug_centre);
			}
		}
	}

	void UniverseDebug::DrawPredictedPath(Player& player, Planet* planet, Planet* centre) {
		UniverseManager& uniManager = UniverseManager::Get();

		std::vector<glm::vec3> points;
		glm::vec3 this_position = planet->transform->world_position;
		glm::vec3 this_velocity = planet->physicsBody.velocity;

		points.push_back(planet->transform->world_position);

		int numPlanets = uniManager.GetPlanets().size();
		std::vector<std::shared_ptr<Planet>>& planets = uniManager.GetPlanets();

		
		// Set initial conditions
		std::vector<glm::vec3> other_positions;
		std::vector<glm::vec3> other_velocities;
		for (int i = 0; i < numPlanets; i++) {
			other_positions.push_back(planets[i]->transform->world_position);
			other_velocities.push_back(planets[i]->physicsBody.velocity);
		}

		glm::vec3 centre_pos = centre ? centre->transform->world_position : glm::vec3(0.0);

		int this_index = 0;
		int centre_index = 0;
		for (int i = 0; i < planet->physicsBody.num_points; i++) {

			std::vector<glm::vec3> accelerations(numPlanets, glm::vec3(0.0f));

			// Compute accelerations
			for (int t = 0; t < numPlanets; t++) {
				for (int o = 0; o < numPlanets; o++) {
					if (o == t) continue;

					if (planets[o].get() == planet) 
						this_index = o;
					else if (planets[t].get() == planet)
						this_index = t;

					if (centre) {
						if (planets[o].get() == centre)
							centre_index = o;
						else if (planets[t].get() == centre)
							centre_index = t;
					}


					glm::vec3 dir = glm::normalize(other_positions[o] - other_positions[t]);
					float dstSq = glm::distance2(other_positions[o], other_positions[t]);
					float acc = uniManager.G * planets[o]->physicsBody.mass / dstSq;

					accelerations[t] += dir * acc;
				}
			}

			// Update velocities + positions
			for (int t = 0; t < numPlanets; t++) {
				other_velocities[t] += accelerations[t] * planet->physicsBody.time_step;
				other_positions[t] += other_velocities[t] * planet->physicsBody.time_step;
			}

			// If there is a centre reference then recentre
			glm::vec3 pos = centre ? (other_positions[this_index] - (other_positions[centre_index] - centre_pos)) : other_positions[this_index];

			points.push_back(pos);
		}

		// Draw
		pathVAO.Bind();
		pathVBO.Bind();
	
		pathVBO.BindBufferData(sizeof(glm::vec3) * points.size(), points.data());
		pathVAO.LinkAttrib(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

		pathVBO.Unbind();
		pathVAO.Unbind();

		pathShader->Use();
		pathVAO.Bind();

		pathShader->SetMat4("view", player.camera.GetView());
		pathShader->SetMat4("proj", player.camera.GetProj());

		glDrawArrays(GL_LINE_STRIP, 0, points.size());
	}

}