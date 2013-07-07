#include "camera.h"

XMFLOAT4 Camera::Eye() const
{
	return this->eye;
}
XMFLOAT4 Camera::Target() const
{
	XMVECTOR Eye = XMVectorSet( this->eye.x, this->eye.y, this->eye.z, this->eye.w );
	XMVECTOR At = XMVectorSet(  this->targetMagnitude.x, this->targetMagnitude.y, this->targetMagnitude.z, this->targetMagnitude.w );

	XMMATRIX RotationMatrix( XMMatrixRotationRollPitchYaw( this->RadianPitch, this->RadianYaw, this->RadianRoll ));

	At = XMVector3TransformCoord( At, RotationMatrix );
	
	At += Eye;

	XMFLOAT4 returnValue;
	XMStoreFloat4(&returnValue,At);
	return returnValue;
}
float Camera::Pitch() const
{
	return this->RadianPitch;
}
float Camera::Roll() const
{
	return this->RadianRoll;
}
float Camera::Yaw() const
{
	return this->RadianYaw;
}

XMFLOAT4X4 Camera::GetViewMatrix() const 
{
	XMVECTOR Eye = XMVectorSet( this->eye.x, this->eye.y, this->eye.z, this->eye.w );
	XMVECTOR At = XMVectorSet(  this->targetMagnitude.x, this->targetMagnitude.y, this->targetMagnitude.z, this->targetMagnitude.w );
	XMVECTOR Up = XMVectorSet(  this->up.x, this->up.y, this->up.z, this->up.w );

	XMMATRIX RotationMatrix( XMMatrixRotationRollPitchYaw( this->RadianPitch, this->RadianYaw, this->RadianRoll ));

	At = XMVector3TransformCoord( At, RotationMatrix );
    Up = XMVector3TransformCoord( Up, RotationMatrix );
	
	At += Eye;
	
	XMFLOAT4X4  view;
	XMStoreFloat4x4(&view, XMMatrixLookAtLH( Eye, At, Up ));

	return view;
}

void Camera::SetPosition(float x, float y, float z, float w)
{
	this->eye = XMFLOAT4(x, y, z, w);
}
void Camera::SetUp(float x, float y, float z, float w)
{
	this->up = XMFLOAT4(x, y, z, w);
}
void Camera::SetLook(float x, float y, float z, float w)
{
	this->targetMagnitude = XMFLOAT4(x, y, z, w);
}

void Camera::MoveForward(float delta)
{
	XMVECTOR Eye = XMVectorSet( this->eye.x, this->eye.y, this->eye.z, this->eye.w );
	XMVECTOR At = XMVectorSet(  this->targetMagnitude.x, this->targetMagnitude.y, this->targetMagnitude.z, this->targetMagnitude.w );
	
	XMMATRIX RotationMatrix( XMMatrixRotationRollPitchYaw( this->RadianPitch, this->RadianYaw, this->RadianRoll ));

	At = XMVector3TransformCoord( At, RotationMatrix );
	At = XMVector4Normalize(At);
	Eye += At * delta;

	XMStoreFloat4(&this->eye, Eye);
}

void Camera::Pitch(float delta)
{
	this->RadianPitch += delta;
}
void Camera::Roll(float delta)
{
	this->RadianRoll += delta;
}
void Camera::Yaw(float delta)
{
	this->RadianYaw += delta;
}

Camera::Camera()
		: eye( 0.0f, 5.0f, -20.0f, 0.0f )
		, targetMagnitude( 0.0f, 0.0f, 1.0f, 0.0f )
		, up( 0.0f, 1.0f, 0.0f, 0.0f )
		, RadianRoll(0.0f), RadianPitch(0.0f), RadianYaw(0.0f)
{
	//Update();
}

Camera::~Camera()
{

}
	