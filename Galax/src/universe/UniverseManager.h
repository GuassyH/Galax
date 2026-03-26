#pragma once

#include "Camera.h"
#include "planetary/Planet.h"
#include "stars/StarSkybox.h"
#include "atmosphere/Atmosphere.h"
#include "Player.h"

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

		std::unique_ptr<AtmosphereRenderer> atmosphereRenderer;
		std::unique_ptr<OceanRenderer> oceanRenderer;
		std::unique_ptr<StarSkybox> starSkybox;
		std::vector<std::shared_ptr<Planet>> planets;

		void Init(Camera& camera);

		void ResolveGravity();
		void Update(Player& player, GLFWwindow* window, int w, int h);
		void Render(Camera& camera, Planet* sun, int w, int h);

		void Shutdown();

	private:
		GLuint baseFBO; // for planets and stuff
		GLuint baseTexture;
		GLuint baseDepth;

		GLuint starFBO; // for planets and stuff
		GLuint starTexture;
		GLuint starDepth;

		int last_width, last_height = 0;
	};
}