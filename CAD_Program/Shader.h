#pragma once

//we need to include some stuff:
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

//include appropriate OpenGL files:
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
	//program ID
	unsigned int ID;

	//constructor, reads and builds
	Shader(const char* vertexPath, const char* fragmentPath);

	//destructor, deletes the program
	~Shader() { this->deleteProgram(); }

	//use the shader
	void Use();

	//delete the program
	void deleteProgram();

	//uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(std::string name, const glm::vec3& value) const;
	void setVec4(std::string name, const glm::vec4& value) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
};

