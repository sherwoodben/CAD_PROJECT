#include "Sketch.h"

#define SKETCH_CLEAR_R 1.0f
#define SKETCH_CLEAR_G 1.0f
#define SKETCH_CLEAR_B 1.0f

#define SKETCH_RESOLUTION 2048

void Sketch::PassShaderData(Shader* shader)
{
	shader->Use();
	shader->setMat4("model", this->sketchPlane->GetModelMatrix());

	//set the flat color
	float r, g, b, a;
	r = SKETCH_CLEAR_R;
	g = SKETCH_CLEAR_G;
	b = SKETCH_CLEAR_B;
	a = 1.0f;
	//now, make  the color a vec4
	glm::vec4 bgColor = glm::vec4(r, g, b, 1.0f);
	//update the background color uniform and the alpha uniform
	shader->setVec4("bgColor", bgColor);
	shader->setFloat("alpha", a);
}

void Sketch::RenderObject(Shader* sketchInPlaneShader)
{
	this->PassShaderData(sketchInPlaneShader);
	//first, bind the VAO:
	glBindVertexArray(*this->sketchPlane->GetObjectVAOPointer());
	//then the texture
	glBindTexture(GL_TEXTURE_2D, this->sketchRenderedTexture);
	//now, draw elements (all of them)
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	int errorVal = glGetError();
	
	if (errorVal < 0)
	{
		std::cout << "ERROR: " << errorVal << std::endl;
		std::cout << "GL_NO_ERROR: " << GL_NO_ERROR << std::endl;
		std::cout << "GL_INVALID_ENUM: " << GL_INVALID_ENUM << std::endl;
		std::cout << "GL_INVALID_VALUE: " << GL_INVALID_VALUE << std::endl;
		std::cout << "GL_INVALID_OPERATION: " << GL_INVALID_OPERATION << std::endl;
		std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION: " << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl;
		std::cout << "GL_OUT_OF_MEMORY: " << GL_OUT_OF_MEMORY << std::endl;
		std::cout << "GL_STACK_UNDERFLOW: " << GL_STACK_UNDERFLOW << std::endl;
		std::cout << "GL_STACK_OVERFLOW: " << GL_STACK_OVERFLOW << std::endl;
	}
}

void Sketch::RenderSketchObjects(Shader* shader)
{
	//switch to the correct shader
	shader->Use();
	//first we switch to the plane's FBO:
	glBindFramebuffer(GL_FRAMEBUFFER, this->sketchFBO);
	glActiveTexture(GL_TEXTURE0);
	glClearColor(SKETCH_CLEAR_R, SKETCH_CLEAR_G, SKETCH_CLEAR_B, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//and we reset the viewport:
	glViewport(0, 0, SKETCH_RESOLUTION, SKETCH_RESOLUTION);

	//make a temporary VAO and VBO:
	unsigned int tempVAO;
	glGenVertexArrays(1, &tempVAO);
	//bind the temp VAO
	glBindVertexArray(tempVAO);
	//set vertex attribute pointers (location = 0!)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	//render all the sketch objects
	for (SketchObject* sO : this->sketchObjects)
	{
		sO->Render(shader);
	}

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &tempVAO);

	//now we rebind the real framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(0.10f, 0.10f, 0.2f, 1.0f);
	int width, height;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);

	

}

void Sketch::DeleteSketchObjects()
{
	std::erase_if(this->sketchObjects, [](SketchObject* x) {return (x->canDelete && x->tryDelete); });
	for (SketchObject* sO : this->sketchObjects)
	{
		sO->tryDelete = false;
	}
}

void Sketch::DeleteObject()
{
	for (int objectIndex = this->sketchObjects.size() - 1; objectIndex >= 0; objectIndex--)
	{
		delete this->sketchObjects[objectIndex];
	}

	glDeleteTextures(1, &this->sketchRenderedTexture);
	this->DeleteBuffers();
	glDeleteBuffers(1, &this->sketchFBO);
}

glm::vec3 Sketch::SketchToWorldPosition(float x, float y)
{
	glm::vec3 worldSpace = glm::vec3(0.0f);

	worldSpace += (sketchPlane->GetPlaneRightVector() * x);
	worldSpace += (sketchPlane->GetPlaneUpVector() * y);

	worldSpace += this->sketchPlane->GetObjectPosition();

	return worldSpace;
}

void Sketch::InitSketch()
{
	this->isDatumObject = false;

	this->objectType = "Sketch";

	this->SetShaderType(ShaderType::SKETCH);

	this->SetIsTransparent(true);

	//set up the buffers (same parameters as the parent plane)
	this->SetUpBuffers(Plane::planeVertices, 4, Plane::planeIndices, 6);

	//generate the framebuffer and bind it
	glGenFramebuffers(1, &this->sketchFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->sketchFBO);
	//std::cout << "first: " << glGetError() << std::endl;
	//set up the texture
	glGenTextures(1, &this->sketchRenderedTexture);
	glBindTexture(GL_TEXTURE_2D, this->sketchRenderedTexture);
	//load an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SKETCH_RESOLUTION, SKETCH_RESOLUTION, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//setup correct filtering (turn off to see difference)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	//more setup: connect "GL_COLOR_ATTACHMENT0" to our texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->sketchRenderedTexture, 0);

	//set the list of draw buffers
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "SketchObject: framebuffer error." << std::endl;
	}

	//unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//test implementations
	this->AddPoint(0.0f, 0.4f);
	
	this->AddLine(SketchPoint(0.125f, 0.125f), SketchPoint(0.125f, -0.125f));
	this->AddLine(SketchPoint(0.125f, -0.125f), SketchPoint(0.25f, -0.125f));

	this->AddCurve(new SketchArc(SketchPoint(-0.25f, 0.25f), SketchPoint(0.0f,0.0f), glm::pi<float>()));

	SketchPoint tempPoints[8] = {
		SketchPoint(0.1f,0.0f),
		SketchPoint(0.0f,0.1f),
		SketchPoint(0.1f, 0.2f),
		SketchPoint(0.0f, 0.3f),

		SketchPoint(0.1f, 0.4f),
		SketchPoint(0.0f, 0.5f),
		SketchPoint(0.1f, 0.6f),
		SketchPoint(0.0f, 0.7f),
	};

	SketchPoint tempPoints2[3] = {
		SketchPoint(0.0f, 0.2f),
		SketchPoint(0.2f, 0.7f),
		SketchPoint(0.4f, 0.0f)
	};

	//SketchBSpline* tempQuadBSpline = new SketchBSpline();
	//SketchBSpline* tempCubicBSpline = new SketchBSpline();

	SketchBezier* tempQuadBezier = new SketchBezier();
	
	//tempQuadBSpline->SetControlPoints(8, 2, tempPoints);
	//tempCubicBSpline->SetControlPoints(8, 3, tempPoints);

	tempQuadBezier->SetControlPoints(3, tempPoints2);

	//this->AddCurve(tempQuadBSpline);
	//this->AddCurve(tempCubicBSpline);
	this->AddCurve(tempQuadBezier);

}


