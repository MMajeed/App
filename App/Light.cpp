#include "Light.h"

Light::Light()
{
	this->diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->spec = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	this->pos = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->dir = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	this->attenuation = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	this->power = 0.0f;
	this->range = 0.0f;
	this->type = this->None;
}
Light::~Light()
{

}