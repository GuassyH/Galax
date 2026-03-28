#include "Shader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "core/Log.h"

void Shader::CheckCompileErrors(unsigned int shader, const std::string& type) {
	int success;
	char infoLog[1024];

	if (type != "PROGRAM") {
		// Shader compilation error checking
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			GX_ERROR("Shader Compilation Error (Type:{})", infoLog);
		}
	}
	else {
		// Shader program linking error checking
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			GX_ERROR("Program Linking Error\n{}", infoLog);
		}
	}
}

void Shader::SetBool(const std::string& name, bool value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform1i(ID, loc, value ? 1 : 0);
}
void Shader::SetInt(const std::string& name, int value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform1i(ID, loc, value);
}
void Shader::SetFloat(const std::string& name, float value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform1f(ID, loc, value);
}
void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform2f(ID, loc, value.x, value.y);
}
void Shader::SetVec2(const std::string& name, float x, float y) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform2f(ID, loc, x, y);
}
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform3f(ID, loc, value.x, value.y, value.z);
}
void Shader::SetVec3(const std::string& name, float x, float y, float z) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform3f(ID, loc, x, y, z);
}
void Shader::SetVec3(const std::string& name, const glm::dvec3& value) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform3f(ID, loc, float(value.x), float(value.y), float(value.z));
}
void Shader::SetVec3(const std::string& name, double x, double y, double z) const {
	GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform3f(ID, loc, float(x), float(y), float(z));
}
void Shader::SetVec4(const std::string& name, const glm::vec4& value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform4f(ID, loc, value.x, value.y, value.z, value.a);
}
void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniform4f(ID, loc, x, y, z, w);
}
void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniformMatrix2fv(ID, loc, 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniformMatrix3fv(ID, loc, 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
	if (loc >= 0) glProgramUniformMatrix4fv(ID, loc, 1, GL_FALSE, glm::value_ptr(mat));
}