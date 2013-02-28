#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <windows.h>
#include <xnamath.h>
#include <string>

class Camera
{
public:
	
	XMFLOAT4X4 GetViewMatrix() const;

	XMFLOAT4 Eye() const;
	XMFLOAT4 Target() const;

	float Pitch() const;
	float Roll() const;
	float Yaw() const;

	void SetPosition(float x, float y, float z, float w = 1.0f);
	void SetUp(float x, float y, float z, float w = 1.0f);
	void SetLook(float x, float y, float z, float w = 1.0f);
	
	void MoveForward(float delta);

	void Pitch(float delta);
	void Roll(float delta);
	void Yaw(float delta);

	Camera();
	virtual ~Camera();

protected:	
	XMFLOAT4 eye;	
	XMFLOAT4 target; // Relative to the eye
	XMFLOAT4 up;

	float RadianRoll;
	float RadianPitch;
	float RadianYaw;
};

#endif // __CAMERA_HPP__