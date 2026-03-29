#include "PlanetShader.h"

#include <glad/glad.h>

PlanetShader::PlanetShader(const char* frag_path, const char* vert_path) {
	Compile(frag_path, vert_path);
}

void PlanetShader::Compile(const char* frag_path, const char* vert_path) {
	type = Frag;

	std::string fragCode = ReadFile(frag_path);
	std::string vertCode = ReadFile(vert_path);

	const char* vertSrc = vertCode.c_str();
	const char* fragSrc = fragCode.c_str();

	fragment_path = std::string(frag_path);
	vertex_path = std::string(vert_path);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertSrc, NULL);
	glCompileShader(vertexShader);
	CheckCompileErrors(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragSrc, NULL);
	glCompileShader(fragmentShader);
	CheckCompileErrors(fragmentShader, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void PlanetShader::Use() {
	glUseProgram(ID);

}

void PlanetShader::Delete() {
	glDeleteProgram(ID);
}