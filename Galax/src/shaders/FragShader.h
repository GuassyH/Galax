#pragma once
#include "Shader.h"

class FragShader : public Shader {
public:
	FragShader();
	FragShader(const char* frag_path, const char* vert_path);
	~FragShader() = default;

	virtual void Compile(const char* frag_path, const char* vert_path) override;
	virtual void Use() override;
	virtual void Delete() override;

	std::string fragment_path;
	std::string vertex_path;
};