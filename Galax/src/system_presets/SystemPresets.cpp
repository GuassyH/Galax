#include "SystemPresets.h"

#include "shaders/PlanetShader.h"

std::shared_ptr<Universe::Planet> SystemPresets::CreateFirstSystem() {
	std::shared_ptr<PlanetShader> shader = std::make_shared<PlanetShader>("assets/shaders/universe/planet.frag", "assets/shaders/universe/planet.vert");
	// shader->texture = std::make_shared<Texture>("assets/textures/grid.jpg");

	std::shared_ptr<FragShader> unlit = std::make_shared<FragShader>("assets/shaders/default_unlit.frag", "assets/shaders/default_unlit.vert");

	// Charley planetary system
	std::shared_ptr<Universe::Planet> planet_char = std::make_shared<Universe::Planet>();
	planet_char->name = "Charley Planet";
	planet_char->shader = shader;
	planet_char->radius = 1000;
	planet_char->resolution = 100;
	planet_char->LODradii = { 6.0f, 3.0f, 1.5f, 1.0f };

	planet_char->terrainGenerator.numCraters = 50;
	planet_char->terrainGenerator.sizeFalloff = 3.0f;
	planet_char->terrainGenerator.baseSize = 10.0f;
	planet_char->terrainGenerator.sizeExaggeration = 2.0f;
	planet_char->terrainGenerator.smoothingK = 0.1f;
	planet_char->terrainGenerator.craterHeight = 2.0f;

	planet_char->terrainGenerator.numLayers = 25;
	planet_char->terrainGenerator.noiseStrength = 30.0f;
	planet_char->terrainGenerator.noiseHeightShift = -2.0f;
	planet_char->terrainGenerator.noiseScale = 0.025f;
	planet_char->terrainGenerator.surfaceColor = glm::vec4(0.396f, 0.58f, 0.306f, 1.0f);
	planet_char->terrainGenerator.peakColor = glm::vec4(0.569f, 0.498f, 0.286f, 1.0f);

	planet_char->physicsBody.mass = 10000000;
	planet_char->mpr = 24.0; // x minutes for one rot
	planet_char->rotation_axis = glm::normalize(glm::vec3(0.2f, 1.0f, 0.2f));

	planet_char->Generate();
	planet_char->transform->local_position = glm::vec3(555000.0f, 0.0f, 0.0f);
	planet_char->transform->UpdateMatrix();

	planet_char->atmosphere_config.planetRadius = planet_char->radius;
	planet_char->atmosphere_config.densityFalloff = 8.0f;
	planet_char->atmosphere_config.atmosphereHeight = 200.0f;
	planet_char->atmosphere_config.scatteringCoefficient = 200.0f;
	planet_char->atmosphere_config.wavelengths = glm::vec3(700.0f, 550.0f, 440.0f);
	planet_char->atmosphere_config.scatteringStrength = 0.5f;
	planet_char->atmosphere_config.intensity = 0.9f;
	planet_char->atmosphere_config.UpdateWavelengthScatter();
	planet_char->hasAtmosphere = true;

	planet_char->ocean_config.radius = planet_char->radius;
	planet_char->ocean_config.densityFalloff = 2.0f;
	planet_char->ocean_config.oceanColor = glm::vec4(0.0, 0.1, 0.3, 1.0);
	planet_char->hasOcean = true;
	planet_char->ocean_config.normalTexture = std::make_shared<Texture>("assets/textures/water_four.jpeg", GL_REPEAT, GL_LINEAR);
	planet_char->ocean_config.normalRepeat = 100.0f;
	planet_char->ocean_config.normalStrength = 0.2f;
	planet_char->ocean_config.triplanarBlend = 2.5f;


	// Moon
	std::shared_ptr<Universe::Planet> moon = std::make_shared<Universe::Planet>();
	moon->name = "Luna";
	moon->shader = shader;
	moon->radius = 160;
	moon->resolution = 50;
	moon->LODradii = { 6.0, 3.0, 1.5, 1.0 };

	moon->terrainGenerator.numCraters = 2;
	moon->terrainGenerator.baseSize = 3;

	moon->mpr = 60;
	moon->rotation_axis = glm::vec3(0.0, 1.0, 0.0);
	moon->physicsBody.mass = 70409;
	moon->physicsBody.velocity = glm::vec3(5.280, 0.0, -10.230f);

	moon->Generate();
	moon->transform->local_position = planet_char->transform->world_position + glm::vec3(0.0, -1000.0, 20223.0);
	moon->transform->UpdateMatrix();


	// Sun
	std::shared_ptr<Universe::Planet> sun = std::make_shared<Universe::Planet>();
	sun->name = "Luxia";
	sun->shader = unlit;
	sun->radius = 2000;
	sun->resolution = 50;
	sun->LODradii = { };

	sun->terrainGenerator.numCraters = 0;

	sun->terrainGenerator.numLayers = 2;
	sun->terrainGenerator.noiseStrength = 10.0f;
	sun->terrainGenerator.noiseHeightShift = 0.0f;
	sun->terrainGenerator.noiseBaseFrequency = 0.025f;

	sun->terrainGenerator.surfaceColor = glm::vec4(1.0f, 0.8f, 0.3f, 1.0f);
	sun->terrainGenerator.peakColor = glm::vec4(1.0f, 0.9f, 0.6f, 1.0f);

	// GIANT mass, since it should basically be stationary
	sun->physicsBody.mass = 1000000000.0f;

	sun->Generate();
	sun->transform->local_position = glm::vec3(0.0, 0.0f, 0.0f);
	sun->transform->UpdateMatrix();


	// NEED TO FIX, if planet_char is first, the subdivisions break somehow
	Universe::UniverseManager::Get().PushPlanet(sun);
	Universe::UniverseManager::Get().PushPlanet(moon);
	Universe::UniverseManager::Get().PushPlanet(planet_char);


	planet_char->physicsBody.debug_centre = sun.get();
	moon->physicsBody.debug_centre = planet_char.get();

	Universe::UniverseManager::Get().SetIdealOrbitVelocity(planet_char.get(), sun.get());

	return sun;
}