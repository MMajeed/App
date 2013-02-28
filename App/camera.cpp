#include "camera.h"

XMFLOAT4 Camera::Eye() const
{
	return this->eye;
}

XMFLOAT4 Camera::Target() const
{
	XMVECTOR Eye = XMVectorSet( this->eye.x, this->eye.y, this->eye.z, this->eye.w );
	XMVECTOR At = XMVectorSet(  this->target.x, this->target.y, this->target.z, this->target.w );

	XMMATRIX RotationMatrix( XMMatrixRotationRollPitchYaw( this->RadianPitch, this->RadianYaw, this->RadianRoll ));

	At = XMVector3TransformCoord( At, RotationMatrix );
	
	At += Eye;

	XMFLOAT4 returnValue;
	XMStoreFloat4(&returnValue,At);
	return returnValue;
}

XMFLOAT4X4 Camera::GetViewMatrix() const 
{
	XMVECTOR Eye = XMVectorSet( this->eye.x, this->eye.y, this->eye.z, this->eye.w );
	XMVECTOR At = XMVectorSet(  this->target.x, this->target.y, this->target.z, this->target.w );
	XMVECTOR Up = XMVectorSet(  this->up.x, this->up.y, this->up.z, this->up.w );

	XMMATRIX RotationMatrix( XMMatrixRotationRollPitchYaw( this->RadianPitch, this->RadianYaw, this->RadianRoll ));

	At = XMVector3TransformCoord( At, RotationMatrix );
    Up = XMVector3TransformCoord( Up, RotationMatrix );
	
	At += Eye;

	
	XMFLOAT4X4  view_;
	XMStoreFloat4x4(&view_, XMMatrixLookAtLH( Eye, At, Up ));

	return view_;
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
	this->target = XMFLOAT4(x, y, z, w);
}

void Camera::MoveForward(float delta)
{
	this->target.x = 0.0f;
	this->target.y = 0.0f;
	this->target.z = 1.0f;

	XMVECTOR Eye = XMVectorSet( this->eye.x, this->eye.y, this->eye.z, this->eye.w );
	XMVECTOR At = XMVectorSet(  this->target.x, this->target.y, this->target.z, this->target.w );
	
	XMMATRIX RotationMatrix( XMMatrixRotationRollPitchYaw( this->RadianPitch, this->RadianYaw, this->RadianRoll ));

	At = XMVector3TransformCoord( At, RotationMatrix );

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
		: eye( 0.0f, 0.0f, -5.0f, 0.0f )
		, target( 0.0f, 0.0f, 1.0f, 0.0f )
		, up( 0.0f, 1.0f, 0.0f, 0.0f )
		, RadianRoll(0.0f), RadianPitch(0.0f), RadianYaw(0.0f)
{
	//Update();
}

Camera::~Camera()
{

}
	