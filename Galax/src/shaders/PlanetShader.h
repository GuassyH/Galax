#pragma once

#include <iostream>
#include <memory>

#include "Shader.h"
#include "rendering/Texture.h"

struct ColorMap {
	glm::vec4 col = glm::vec4(0.0f);
	glm::vec4 steepCol = glm::vec4(0.0f);
	float height;
	float steepness; // match CPU padding
	float heightSharpness = 0.0f;
	float steepSharpness = 0.0f;
};

class PlanetShader : public Shader {
public:
	PlanetShader() = default;
	PlanetShader(const char* frag_path, const char* vert_path);
	~PlanetShader() = default;

	virtual void Compile(const char* frag_path, const char* vert_path) override;
	virtual void Use() override;
	virtual void Delete() override;

	std::vector<ColorMap> colorMaps;
	bool lit = true;

	std::string fragment_path;
	std::string vertex_path;
};