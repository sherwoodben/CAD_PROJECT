#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	//get vertex/fragment codes
	std::string vertexCode, fragmentCode;
	std::ifstream vShaderFile, fShaderFile;

	//enable exception throwing
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//read files into stream
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close handlers
		vShaderFile.close();
		fShaderFile.close();
		//stream to string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "Unable to read shader file." << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];
	//vertex
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	//errors
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Vertex shader compilation failed.\n" << infoLog << std::endl;
	}
	//fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	//errors
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Fragment shader compilation failed.\n" << infoLog << std::endl;
	}

	//shader program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex);
	glAttachShader(this->ID, fragment);
	glLinkProgram(this->ID);
	//errors
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "Shader linking failed.\n" << infoLog << std::endl;
	}

	//delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Use()
{
	glUseProgram(this->ID);
	//std::cout << "Switched to Shader #" << this->ID << std::endl;
}

void Shader::deleteProgram()
{
	glDeleteProgram(this->ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::setVec3(std::string name, const glm::vec3& value) const
{
	glUniform1f(glGetUniformLocation(this->ID, (name.append(".x")).c_str()), (int)value[0]);
	glUniform1f(glGetUniformLocation(this->ID, (name.append(".y")).c_str()), (int)value[1]);
	glUniform1f(glGetUniformLocation(this->ID, (name.append(".z")).c_str()), (int)value[2]);
}

void Shader::setVec4(std::string name, const glm::vec4& value) const
{
	glUniform4f(glGetUniformLocation(this->ID, name.c_str()), value.r, value.g, value.b, value.a);
	
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}