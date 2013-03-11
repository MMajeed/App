#include "Prespective.h"

XMFLOAT4X4 Prespective::GetPrespective()
{
	XMMATRIX prespective =  XMMatrixPerspectiveFovLH( this->FovAngle, this->Width / this->Height, this->MinViewable, this->MaxViewable );
	XMFLOAT4X4 returnValue;
	XMStoreFloat4x4(&returnValue, prespective);
	return returnValue;
}

float Prespective::GetFovAngle()
{
	return this->FovAngle;
}
float Prespective::GetWidth()
{
	return this->Width;
}
float Prespective::GetHeight()
{
	return this->Height;
}
float Prespective::GetMinViewable()
{
	return this->MinViewable;
}
float Prespective::GetMaxViewable()
{
	return this->MaxViewable;
}

void Prespective::SetFovAngle(float value)
{
	this->FovAngle = value;
}
void Prespective::SetWidth(float value)
{
	this->Width = value;
}
void Prespective::SetHeight(float value)
{
	this->Height = value;
}
void Prespective::SetMinViewable(float value)
{
	this->MinViewable = value;
}
void Prespective::SetMaxViewable(float value)
{
	this->MaxViewable = value;
}

Prespective::Prespective()
	: FovAngle(XM_PIDIV4),
	Width(1024.0f),
	Height(768.0f),
	MinViewable(0.01f),
	MaxViewable(5000.0f)
{

}