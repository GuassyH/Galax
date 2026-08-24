#include "SystemPresets.h"

#include "shaders/PlanetShader.h"

std::shared_ptr<Universe::Planet> SystemPresets::CreateFirstSystem(Renderer& renderer) {
	// shader->texture = std::make_shared<Texture>("assets/textures/grid.jpg");

	std::shared_ptr<FragShader> unlit = std::make_shared<FragShader>("assets/shaders/default_unlit.frag", "assets/shaders/default_unlit.vert");

	// Charley planetary system
	std::shared_ptr<Universe::Planet> planet_char = std::make_shared<Universe::Planet>();
	planet_char->name = "Charley Planet";
	planet_char->shader = PlanetShader("assets/shaders/universe/planet.frag", "assets/shaders/universe/planet.vert");
	planet_char->radius = 5000;
	planet_char->resolution = 100;
	planet_char->LODradii = { 6.0f, 4.5, 3.0f, 1.5f, 1.0f };

	planet_char->terrainGenerator.numCraters = 1;
	planet_char->terrainGenerator.sizeFalloff = 3.0f;
	planet_char->terrainGenerator.baseSize = 10.0f;
	planet_char->terrainGenerator.sizeExaggeration = 2.0f;
	planet_char->terrainGenerator.smoothingK = 0.1f;
	planet_char->terrainGenerator.craterHeight = 2.0f;


	planet_char->terrainGenerator.noiseLayers.push_back(NewNoiseLayer(glm::vec3(0.0f), NoiseType::Voronoi, 0.0005f, 523.0f, 2, 1.25f, 0.5f, -1050.0f, 14.0f, 0.03f, 9, planet_char->radius));
	planet_char->terrainGenerator.noiseLayers.push_back(NewNoiseLayer(glm::vec3(0.0f), NoiseType::Perlin, 0.001f, 50.0f, 30, 1.25f, 0.75f, -118.0f));
	planet_char->terrainGenerator.noiseLayers.push_back(NewNoiseLayer(glm::vec3(1000, 200, 0.0f), NoiseType::Voronoi, 0.002f, -123.0f, 2, 1.25f, 0.5f, 0.0f, 71.0f, 0.01f, 9, 0.0f));
	//planet_char->terrainGenerator.noiseLayers.push_back(NewNoiseLayer(glm::vec3(20), NoiseType::Perlin, 0.05f, 0.5f));

	planet_char->physicsBody.mass = 10000000;
	planet_char->mpr = 24.0; // x minutes for one rot
	planet_char->rotation_axis = glm::normalize(glm::vec3(0.2f, 1.0f, 0.2f));

	planet_char->Generate(renderer);
	planet_char->transform->local_position = glm::vec3(5555000.0f, 0.0f, 0.0f);
	planet_char->transform->UpdateMatrix();

	planet_char->atmosphere_config.densityFalloff = 3.5f;
	planet_char->atmosphere_config.atmosphereHeight = 1000.0f;
	planet_char->atmosphere_config.scatteringCoefficient = 300.0f;
	planet_char->atmosphere_config.wavelengths = glm::vec3(700.0f, 550.0f, 440.0f);
	planet_char->atmosphere_config.scatteringStrength = 50.150f;
	planet_char->atmosphere_config.intensity = 1.0f;
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


	planet_char->shader.colorMaps.push_back(NewColorMap(glm::vec4(0.7f, 0.7f, 0.25f, 1.0), glm::vec4(0.3f, 0.5f, 0.2f, 1.0), 0.0f, 0.0f, 1.0f, 15.0f));
	planet_char->shader.colorMaps.push_back(NewColorMap(glm::vec4(0.3f, 0.5f, 0.2f, 1.0), {}, 1.0f, 0.4f, 0.6f));
	planet_char->shader.colorMaps.push_back(NewColorMap(glm::vec4(0.3f, 0.5f, 0.2f, 1.0), glm::vec4(0.25f, 0.25f, 0.25f, 1.0), 50.0f, 6.0f, 1.0f, 0.09f, 0.0f, 0.180f));
	planet_char->shader.colorMaps.push_back(NewColorMap(glm::vec4(0.8f, 0.8f, 0.8f, 1.0), glm::vec4(0.25f, 0.25f, 0.25f, 1.0), 170.0f, 50.0f, 1.0f, 0.95f, 0.0f, 1.0f));

	// Moon
	std::shared_ptr<Universe::Planet> moon = std::make_shared<Universe::Planet>();
	moon->name = "Luna";
	moon->shader = PlanetShader("assets/shaders/universe/planet.frag", "assets/shaders/universe/planet.vert");
	moon->radius = 460;
	moon->resolution = 50;
	moon->LODradii = { 6.0, 3.0, 1.5, 1.0 };

	moon->terrainGenerator.numCraters = 80;
	moon->terrainGenerator.baseSize = 3;

	moon->terrainGenerator.noiseLayers.push_back(NewNoiseLayer({}, {}, 0.01f, 14.0f, 20));

	moon->mpr = 60;
	moon->rotation_axis = glm::vec3(0.0, 1.0, 0.0);
	moon->physicsBody.mass = 70409;

	moon->Generate(renderer);
	moon->transform->local_position = planet_char->transform->world_position + glm::vec3(0.0, -1000.0, 20223.0);
	moon->transform->UpdateMatrix();

	moon->shader.colorMaps.push_back(NewColorMap(glm::vec4(0.3f), glm::vec4(0.23f), 0.0f, 0.0f, 1.0f, 0.2f, 0.25f, 0.0f));

	// Sun
	std::shared_ptr<Universe::Planet> sun = std::make_shared<Universe::Planet>();
	sun->name = "Luxia";
	sun->shader = PlanetShader("assets/shaders/universe/planet.frag", "assets/shaders/universe/planet.vert");
	sun->radius = 20000;
	sun->resolution = 40;
	sun->LODradii = { };

	sun->shader.colorMaps.push_back(NewColorMap(glm::vec4(0.99f, 0.99f, 0.8f, 1.0f)));
	sun->shader.lit = false;

	sun->terrainGenerator.noiseLayers.push_back(NewNoiseLayer(glm::vec3(0.0f), NoiseType::Perlin, 0.0001f, 200.0f, 6));

	// GIANT mass, since it should basically be stationary
	sun->physicsBody.mass = 1000000000.0f;

	sun->Generate(renderer);
	sun->transform->local_position = glm::vec3(0.0, 0.0f, 0.0f);
	sun->transform->UpdateMatrix();


	// NEED TO FIX, if planet_char is first, the subdivisions break somehow
	Universe::UniverseManager::Get().PushPlanet(sun);

	Universe::UniverseManager::Get().PushPlanet(moon);
	Universe::UniverseManager::Get().PushPlanet(planet_char);

	planet_char->physicsBody.debug_centre = sun.get();
	moon->physicsBody.debug_centre = planet_char.get();

	moon->physicsBody.velocity = glm::vec3(5.83f, 0.0f, -3.03f);
	planet_char->physicsBody.velocity = glm::vec3(0.0f, 0.0f, -3.55f);

	// Universe::UniverseManager::Get().SetIdealOrbitVelocity(planet_char.get(), sun.get());

	for (auto planet : Universe::UniverseManager::Get().GetPlanets()) {
		renderer.atmosphereRenderer->BakeOpticalDepth(planet->atmosphere_config, planet->radius);
	}

	return sun;
}