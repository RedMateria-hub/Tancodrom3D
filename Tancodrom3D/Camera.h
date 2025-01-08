#pragma once
#include <GL/glew.h>
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>

enum ECameraMovementType
{
	UNKNOWN,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
private:
	// Default camera values
	const float zNEAR = 0.1f;
	const float zFAR = 500.f;
	const float YAW = -310.2f;
	const float PITCH = -14.f;
	const float FOV = 45.0f;
	glm::vec3 startPosition;

public:
	Camera(const int width, const int height, const glm::vec3& position);

	void Set(const int width, const int height, const glm::vec3& position);

	void Reset(const int width, const int height);

	void Reshape(int windowWidth, int windowHeight);

	const glm::mat4 GetViewMatrix() const;

	const glm::vec3 GetPosition() const;

	const glm::mat4 GetProjectionMatrix() const;

	void RotateHorizontally(float angleDelta);
	void RotateVertically(float angleDelta);

	bool ProcessKeyboard(ECameraMovementType direction, float deltaTime);

	void MouseControl(float xPos, float yPos);

	void ProcessMouseScroll(float yOffset);

	float getYaw();

	float getPitch();

private:
	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

	void UpdateCameraVectors();

protected:
	const float cameraSpeedFactor = 150.0f;
	const float mouseSensitivity = 0.1f;

	// Perspective properties
	float zNear;
	float zFar;
	float FoVy;
	int width;
	int height;
	bool isPerspective;

	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 worldUp;

	// Euler Angles
	float yaw;
	float pitch;

	bool bFirstMouseMove = true;
	float lastX = 0.f, lastY = 0.f;
};