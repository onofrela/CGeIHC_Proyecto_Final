#include "Camera.h"
#include "Window.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position1 = startPosition;
	position2 = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (modoAereo)
	{
		if (keys[GLFW_KEY_W])
			position2.z -= velocity;
		if (keys[GLFW_KEY_S])
			position2.z += velocity;
		if (keys[GLFW_KEY_A])
			position2.x -= velocity;
		if (keys[GLFW_KEY_D])
			position2.x += velocity;
		position2.y = 120.0f;
		return; 
	}
	if (keys[GLFW_KEY_W])
		position1 += front * velocity;
	if (keys[GLFW_KEY_S])
		position1 -= front * velocity;
	if (keys[GLFW_KEY_A])
		position1 -= right * velocity;
	if (keys[GLFW_KEY_D])
		position1 += right * velocity;
}

void Camera::setModoAereo(bool activo) {
	modoAereo = activo;
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	if (modoAereo)
	{
		glm::vec3 camPos = glm::vec3(position2.x, 120.0f, position2.z);
		glm::vec3 target = glm::vec3(position2.x, 0.0f, position2.z);
		return glm::lookAt(camPos, target, glm::vec3(0.0f, 0.0f, -1.0f));
	}

	return glm::lookAt(position1, position1 + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position1;
}


glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}


Camera::~Camera()
{
}
