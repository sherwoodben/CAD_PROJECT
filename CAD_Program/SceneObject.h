#pragma once

//include string for access to strings
#include <string>
#include <stdio.h>
#include <iostream>

//include the required OpenGL files
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

struct Color
{
	int r;
	int g;
	int b;
	int a = 255;
};

enum ShaderType
{
	DEBUG,
	PLANE_GRID,
	FLAT,
};

class SceneObject
{
public:
	SceneObject() {};
	~SceneObject() {};

	//keeps track of the object's display
	//name
	std::string displayName = "Unnamed Scene Object";
	//as well as the object's "named type"
	std::string objectType = "Unknown Type";

	//keeps track of if an object is a default
	//object (false by default)
	bool isDefaultObject = false;

	//keeps track of if an object is visible
	//(true by default)
	bool isVisible = true;

	//virtual function for rendering a scene
	//object; each type of object will implement
	//a different method of rendering
	virtual void RenderObject() {};

	//virtual function that deletes an object (if
	//that's allowed); different objects will implement
	//a different method
	virtual void DeleteObject() {};

	//we need a way to retrieve the VBO (ID)
	unsigned int* GetObjectVBOPointer() { return &this->VBO; };
	//we need a way to retrieve the VAO (ID)
	unsigned int* GetObjectVAOPointer() { return &this->VAO; };
	//we need a way to retrieve the EBO (ID)
	unsigned int* GetObjectEBOPointer() { return &this->EBO; };

	//gets the model matrix of the object
	virtual glm::mat4 GetModelMatrix();

	//get the position of the object
	glm::vec3 GetObjectPosition() { return this->objectPosition; };
	void SetObjectPosition(glm::vec3 newPos) { this->objectPosition = newPos; };
	//get the angles of the object
	glm::vec3 GetObjectAngles() { return this->objectAngles; };
	void SetObjectAngles(float roll, float pitch, float yaw) { this->objectAngles = { roll, pitch, yaw }; };

	//get and set the object scale
	float GetObjectScale() { return this->objectScale; };
	void SetObjectScale(float newScale) { this->objectScale = newScale; };

	Color GetDebugColor() { return this->debugColor; };
	void SetDebugColor(Color newColor) { this->debugColor = newColor; };

	Color GetFlatColor() { return this->flatColor; };
	void SetFlatColor(Color newColor) { this->flatColor = newColor; };

	int GetShaderType() { return this->shaderType; };
	void SetShaderType(ShaderType shaderType) { this->shaderType = shaderType; };

	bool GetIsTransparent() { return this->hasTransparency; };
	void SetIsTransparent(bool newValue) { this->hasTransparency = newValue; };

private:
	//an ID for this SceneObject's VBO
	//(vertex buffer object)
	unsigned int VBO;
	//an ID for this SceneObject's VAO
	//(vertex array object)
	unsigned int VAO;
	//an ID for this SceneObject's EBO
	//(element buffer array)
	unsigned int EBO;
	//an int to keep track of the shader type; defined
	//elsewhere
	ShaderType shaderType = ShaderType::DEBUG;

	//keep track of the object's origin
	//(defaults to [0, 0, 0] )
	glm::vec3 objectPosition = { 0.0f, 0.0f, 0.0f };
	//keep track of the object's angles
	//TO-DO-- should replace this with quaternions
	//to avoid problems (maybe?)
	//stored as Roll - Pitch - Yaw
	glm::vec3 objectAngles = { 0.0f, 0.0f, 0.0f };

	//to control the scale of an object;
	//will most likely be 1 always but always
	//nice to have the option to change
	float objectScale = 1.0f;

	Color debugColor = { 255, 255, 255 };
	Color flatColor = { 128, 128, 128 };

	//keep track of if the object has transparency or not
	//(defaults to false)
	bool hasTransparency = false;
};
