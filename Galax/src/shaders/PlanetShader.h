#pragma once

#include <iostream>
#include <memory>

#include "Shader.h"
#include "rendering/Texture.h"

class PlanetShader : public Shader {
public:
	PlanetShader() = default;
	PlanetShader(const char* frag_path, const char* vert_path);
	~PlanetShader() = default;

	virtual void Compile(const char* frag_path, const char* vert_path) override;
	virtual void Use() override;
	virtual void Delete() override;


	struct MaterialPreset {
		glm::vec3 shallow_colour;
		float height_above_sea = 0.0f;
		// std::shared_ptr<Texture> shallow_texture;
		
		glm::vec3 steep_colour;
		float angle = 0.0f;
		// std::shared_ptr<Texture> steep_texture;
	};
	
	std::vector<MaterialPreset> colour_presets = { };

	std::string fragment_path;
	std::string vertex_path;
};