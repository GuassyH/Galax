#include "ComputeShader.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

ComputeShader::ComputeShader(const char* comp_path) {
	Compile(comp_path);
}

void ComputeShader::Compile(const char* comp_path) {
	type = Compute;

	compute_path = std::string(comp_path);
	
	std::string compCode = ReadFile(compute_path);
	const char* compSrc = compCode.c_str();

	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &compSrc, NULL);
	glCompileShader(computeShader);
	CheckCompileErrors(computeShader, "COMPUTE");

	ID = glCreateProgram();
	glAttachShader(ID, computeShader);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	glDeleteShader(computeShader);
}

void ComputeShader::Use() {
	glUseProgram(ID);
}

void ComputeShader::Run(int workgroup_x, int workgroup_y, int workgroup_z) {
	glDispatchCompute(workgroup_x, workgroup_y, workgroup_z);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
void ComputeShader::Delete() {
	glDeleteProgram(ID);
}
