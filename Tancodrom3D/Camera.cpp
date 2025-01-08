#include "Camera.h"

Camera::Camera(const int width, const int height, const glm::vec3& position)
{
	startPosition = position;
	Set(width, height, position);
}

void Camera::Set(const int width, const int height, const glm::vec3& position)
{
	this->isPerspective = true;
	this->yaw = YAW;
	this->pitch = PITCH;

	this->FoVy = FOV;
	this->width = width;
	this->height = height;
	this->zNear = zNEAR;
	this->zFar = zFAR;

	this->worldUp = glm::vec3(0, 1, 0);
	this->position = position;

	lastX = width / 2.0f;
	lastY = height / 2.0f;
	bFirstMouseMove = true;

	UpdateCameraVectors();
}

void Camera::Reset(const int width, const int height)
{
	Set(width, height, startPosition);
}

void Camera::Reshape(int windowWidth, int windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	// define the viewport transformation
	glViewport(0, 0, windowWidth, windowHeight);
}

const glm::mat4 Camera::GetViewMatrix() const
{
	// Returns the View Matrix
	return glm::lookAt(position, position + forward, up);
}

const glm::vec3 Camera::GetPosition() const
{
	return position;
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
	glm::mat4 Proj = glm::mat4(1);
	if (isPerspective) {
		float aspectRatio = ((float)(width)) / height;
		Proj = glm::perspective(glm::radians(FoVy), aspectRatio, zNear, zFar);
	}
	else {
		float scaleFactor = 2000.f;
		Proj = glm::ortho<float>(
			-width / scaleFactor, width / scaleFactor,
			-height / scaleFactor, height / scaleFactor, -zFar, zFar);
	}
	return Proj;
}

void Camera::RotateHorizontally(float angleDelta)
{
	yaw += angleDelta * 5.f; // Update the yaw angle
	UpdateCameraVectors(); // Recalculate forward, right, and up vectors
}

void Camera::RotateVertically(float angleDelta)
{
	pitch += angleDelta * 5.f; // Update the pitch angle
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	UpdateCameraVectors(); // Recalculate forward, right, and up vectors
}

bool Camera::ProcessKeyboard(ECameraMovementType direction, float deltaTime)
{
	//x=-127.839, y=-5.2071, z=-155.855,
	//

	float velocity = static_cast<float>(cameraSpeedFactor * deltaTime);
	glm::vec3 auxPosition = position;
	glm::vec3 minBounds(-200.0f, -5.2f, -200.0f); 
	glm::vec3 maxBounds(200.0f, 40.0f, 200.0f);

	switch (direction) {
	case ECameraMovementType::FORWARD:
		auxPosition += forward * velocity;
		break;
	case ECameraMovementType::BACKWARD:
		auxPosition -= forward * velocity;
		break;
	case ECameraMovementType::LEFT:
		auxPosition -= right * velocity;
		break;
	case ECameraMovementType::RIGHT:
		auxPosition += right * velocity;
		break;
	case ECameraMovementType::UP:
		auxPosition += up * velocity;
		break;
	case ECameraMovementType::DOWN:
		auxPosition -= up * velocity;
		break;
	}

	if (auxPosition.x < minBounds.x || auxPosition.x > maxBounds.x ||
        auxPosition.y < minBounds.y || auxPosition.y > maxBounds.y ||
        auxPosition.z < minBounds.z || auxPosition.z > maxBounds.z) {
        return false;
    }

	if (auxPosition.x == position.x && auxPosition.y == position.y && auxPosition.z == position.z)
		return false;

	position = auxPosition;

	return true;
}

void Camera::MouseControl(float xPos, float yPos)
{
	if (bFirstMouseMove) {
		lastX = xPos;
		lastY = yPos;
		bFirstMouseMove = false;
	}

	float xChange = xPos - lastX;
	float yChange = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	if (fabs(xChange) <= 1e-6 && fabs(yChange) <= 1e-6) {
		return;
	}
	xChange *= mouseSensitivity;
	yChange *= mouseSensitivity;

	ProcessMouseMovement(xChange, yChange);
}

void Camera::ProcessMouseScroll(float yOffset)
{
	if (FoVy >= 1.0f && FoVy <= 90.0f) {
		FoVy -= yOffset;
	}
	if (FoVy <= 1.0f)
		FoVy = 1.0f;
	if (FoVy >= 90.0f)
		FoVy = 90.0f;
}

float Camera::getYaw()
{
	return yaw;
}

float Camera::getPitch()
{
	return pitch;
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
	yaw += xOffset;
	pitch += yOffset;

	//std::cout << "yaw = " << yaw << std::endl;
	//std::cout << "pitch = " << pitch << std::endl;

	// Avem grijã sã nu ne dãm peste cap
	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// Se modificã vectorii camerei pe baza unghiurilor Euler
	UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
	// Calculate the new forward vector
	this->forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->forward.y = sin(glm::radians(pitch));
	this->forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->forward = glm::normalize(this->forward);
	// Also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(forward, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, forward));
}
