#pragma once
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
					GLfloat aIntensity, GLfloat dIntensity,
					GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	// Setter methods for day-night cycle animation
	void SetDirection(GLfloat xDir, GLfloat yDir, GLfloat zDir);
	void SetColor(GLfloat red, GLfloat green, GLfloat blue);
	void SetIntensities(GLfloat aIntensity, GLfloat dIntensity);

	~DirectionalLight();

private:
	glm::vec3 direction;
};

