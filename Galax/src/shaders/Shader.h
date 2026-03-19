#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

// READFILE IS BORROWED
inline std::string ReadFile(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filepath << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

class Shader {
public:
	unsigned int ID = 0;

	enum ShaderType {
		Frag,
		Compute
	};

	ShaderType type;

	Shader() = default;
	virtual ~Shader() = default;

	virtual void Compile(const char* path_1, const char* path_2) {}
	virtual void Compile(const char* path_1) {}
	virtual void Use() = 0;
	virtual void Delete() = 0;
	void CheckCompileErrors(unsigned int shader, const std::string& type);

	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec2(const std::string& name, float x, float y) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec3(const std::string& name, float x, float y, float z) const;
	void SetVec4(const std::string& name, const glm::vec4& value) const;
	void SetVec4(const std::string& name, float x, float y, float z, float w) const;
	void SetMat2(const std::string& name, const glm::mat2& mat) const;
	void SetMat3(const std::string& name, const glm::mat3& mat) const;
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
};
