#pragma once

#include "Shader.h"
#include "Texture.h"

class PlanetShader : public Shader {
public:
	PlanetShader() = default;
	PlanetShader(const char* frag_path, const char* vert_path);
	~PlanetShader() = default;

	virtual void Compile(const char* frag_path, const char* vert_path) override;
	virtual void Use() override;
	virtual void Delete() override;

	struct AngleColor {
		glm::vec4 color;
		float angle;
		float pad[3];
	};
	
	std::vector<AngleColor> angles_to_colors = { };
	// std::shared_ptr<Texture> texture = nullptr;

	std::string fragment_path;
	std::string vertex_path;
};