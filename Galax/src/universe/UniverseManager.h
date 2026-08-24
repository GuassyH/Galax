#pragma once

#include "Camera.h"
#include "planetary/Planet.h"
#include "stars/StarSkybox.h"
#include "Player.h"

#include "rendering/Renderer.h"

namespace Universe {
	class UniverseManager {
	public:
		UniverseManager() = default;
	public:
		UniverseManager(const UniverseManager&) = delete;
		UniverseManager& operator=(const UniverseManager&) = delete;

		static UniverseManager& Get() {
			static UniverseManager instance;
			return instance;
		}

		float recentre_dist = 1000.0f;
		double G = 0.07;
		bool isSimulating = false;
		float time = 0.0f;

		std::shared_ptr<Transform> transform;

		void Init(Renderer& renderer, Camera& camera);

		void PushPlanet(std::shared_ptr<Planet> planet) {
			planets.push_back(planet);
			planet->transform->SetParent(transform.get());
		}

		void Recentre(Player& player);
		void ResolveGravity();
		void SetIdealOrbitVelocity(Planet* planet, Planet* target); // Doesnt work
		
		void Update(Renderer& renderer, Player& player);
		void Render(Renderer& renderer, Camera& camera, Planet* sun);

		void Shutdown();
		std::vector<std::shared_ptr<Planet>>& GetPlanets() { return planets; }

	private:
		std::vector<std::shared_ptr<Planet>> planets;

		GLuint baseFBO; // for planets and stuff
		GLuint baseTexture;
		GLuint baseDepth;

		GLuint skyboxFBO;
		GLuint skyboxTexture;
		GLuint skyboxDepth;

		GLuint starFBO; // for planets and stuff
		GLuint starTexture;
		GLuint starDepth;

		Mesh composite_quad;
		std::unique_ptr<Shader> composite_shader;

		int last_width, last_height = 0;
	};
}