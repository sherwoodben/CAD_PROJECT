#include "Sketch.h"

void SketchObject::Render(unsigned int renderTarget)
{
	switch (this->sketchObjectType)
	{
	case SketchObjectType::POINT:
		break;
	case SketchObjectType::LINE:
		break;
	case SketchObjectType::CURVE:
		break;
	default:
		break;
	}
}

void Sketch::RenderObject()
{
	
	//this->RenderSketchObjects();

	//first, bind the VAO:
	glBindVertexArray(*this->sketchPlane->GetObjectVAOPointer());
	std::cout << "1.875: " << glGetError() << std::endl;
	//then the texture
	//glBindTexture(GL_TEXTURE_2D, this->sketchRenderedTexture);
	std::cout << "2: " << glGetError() << std::endl;
	//now, draw elements (all of them)
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	std::cout << "3: " << glGetError() << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "GL_NO_ERROR: " << GL_NO_ERROR << std::endl;
	std::cout << "GL_INVALID_ENUM: " << GL_INVALID_ENUM << std::endl;
	std::cout << "GL_INVALID_VALUE: " << GL_INVALID_VALUE << std::endl;
	std::cout << "GL_INVALID_OPERATION: " << GL_INVALID_OPERATION << std::endl;
	std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION: " << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl;
	std::cout << "GL_OUT_OF_MEMORY: " << GL_OUT_OF_MEMORY << std::endl;
	std::cout << "GL_STACK_UNDERFLOW: " << GL_STACK_UNDERFLOW << std::endl;
	std::cout << "GL_STACK_OVERFLOW: " << GL_STACK_OVERFLOW << std::endl;

}

void Sketch::RenderSketchObjects()
{
	//first we switch to the plane's FBO:
	glBindFramebuffer(GL_FRAMEBUFFER, this->sketchFBO);
	glActiveTexture(GL_TEXTURE0);
	std::cout << "1: " << glGetError() << std::endl;
	//glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//and we reset the viewport:
	glViewport(0, 0, 2048, 2048);
	//glDisable(GL_DEPTH_TEST);

	for (SketchObject* sO : this->sketchObjects)
	{
		sO->Render(this->sketchRenderedTexture);
	}

	//now we rebind the real framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(0.10f, 0.10f, 0.2f, 1.0f);
	
	int width, height;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);

	

}

void Sketch::AddSketchObject(SketchObject* objectToAdd)
{
	objectToAdd->sketchObjectID = this->nextObjectID++;
	this->sketchObjects.push_back(objectToAdd);
}

void Sketch::DeleteObject()
{
	glDeleteTextures(1, &this->sketchRenderedTexture);
	this->DeleteBuffers();
	glDeleteBuffers(1, &this->sketchFBO);
}

void Sketch::InitSketch()
{
	this->isDatumObject = false;

	this->objectType = "Sketch";

	this->SetShaderType(ShaderType::DEBUG);

	this->SetIsTransparent(true);

	//set up the buffers (same parameters as the parent plane)
	this->SetUpBuffers(Plane::planeVertices, 4, Plane::planeIndices, 6);

	//generate the framebuffer and bind it
	glGenFramebuffers(1, &this->sketchFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->sketchFBO);
	std::cout << "first: " << glGetError() << std::endl;
	//set up the texture
	glGenTextures(1, &this->sketchRenderedTexture);
	glBindTexture(GL_TEXTURE_2D, this->sketchRenderedTexture);
	//load an empty image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//setup correct filtering (turn off to see difference)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

}


