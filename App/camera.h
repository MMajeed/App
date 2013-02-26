#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <windows.h>
#include <xnamath.h>
#include <string>

class Camera
{
public:
	XMFLOAT4 eye;	
	XMFLOAT4 target;
	XMFLOAT4 up;

	float RadianRoll;
	float RadianPitch;
	float RadianYaw;

	void SetPosition(float x, float y, float z, float w = 1.0f);
	void SetUp(float x, float y, float z, float w = 1.0f);
	void SetLook(float x, float y, float z, float w = 1.0f);
	
	void MoveForward(float delta);

	void Pitch(float delta);
	void Roll(float delta);
	void Yaw(float delta);

	Camera();
	virtual ~Camera();
	XMFLOAT4X4 const& GetViewMatrix() const ;
	XMFLOAT4X4 const& Update();

protected:	
	XMFLOAT4X4  view_;
};

#endif // __CAMERA_HPP__