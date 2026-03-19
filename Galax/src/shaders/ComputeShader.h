#pragma once
#include "Shader.h"

class ComputeShader : public Shader {
public:
	ComputeShader() = default;
	ComputeShader(const char* comp_path);
	~ComputeShader() = default;

	virtual void Compile(const char* comp_path) override;
	virtual void Use() override;
	virtual void Delete() override;

	void Run(int workgroup_x, int workgroup_y, int workgroup_z);

	std::string compute_path;
};