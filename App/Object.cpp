#include "Object.h"

XMFLOAT4X4 Object::CalculateMatrix() const
{
	XMMATRIX matTranslate = XMMatrixIdentity();
	XMMATRIX matRotateX = XMMatrixIdentity();	XMMATRIX matRotateY = XMMatrixIdentity();	XMMATRIX matRotateZ = XMMatrixIdentity();
	XMMATRIX matScaling = XMMatrixIdentity();
	XMMATRIX matOrbitX = XMMatrixIdentity();	XMMATRIX matOrbitY = XMMatrixIdentity(); 	XMMATRIX matOrbitZ = XMMatrixIdentity();
	XMMATRIX matFinal = XMMatrixIdentity();

	matTranslate = XMMatrixTranslation( this->Pos.x, this->Pos.y, this->Pos.z);

	matRotateX = XMMatrixRotationX(this->Rot.x);
	matRotateY = XMMatrixRotationY(this->Rot.y);
	matRotateZ = XMMatrixRotationZ(this->Rot.z);

	matScaling = XMMatrixScaling(this->Scale.x, this->Scale.y, this->Scale.z);

	
	matFinal = XMMatrixMultiply(matFinal, matScaling);
	matFinal = XMMatrixMultiply(matFinal, matRotateX);
	matFinal = XMMatrixMultiply(matFinal, matRotateY);
	matFinal = XMMatrixMultiply(matFinal, matRotateZ);
	matFinal = XMMatrixMultiply(matFinal, matTranslate);
	matFinal = XMMatrixMultiply(matFinal, matOrbitX);
	matFinal = XMMatrixMultiply(matFinal, matOrbitY);
	matFinal = XMMatrixMultiply(matFinal, matOrbitZ);
	matFinal = XMMatrixTranspose(matFinal);
	XMFLOAT4X4 returnValue;
	XMStoreFloat4x4(&returnValue, matFinal);

	return returnValue;
}

void Object::Update(float delta)
{

}

Object::Object()
{
	this->Pos = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->Rot = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->Scale = XMFLOAT4(1.f, 1.f, 1.f, 0.0f);
	this->Orbit = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}