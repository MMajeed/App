#include "MathHelper.h"

float MathHelper::Length(XMFLOAT4 v1, XMFLOAT4 v2)
{
	XMVECTOR vector1 = XMLoadFloat4(&v1);
    XMVECTOR vector2 = XMLoadFloat4(&v2);
    XMVECTOR vectorSub = XMVectorSubtract(vector1,vector2);
	XMVECTOR length = XMVector4Length(vectorSub);

    float distance = 0.0f;
    XMStoreFloat(&distance,length);
    return distance;
}

float MathHelper::Length(XMFLOAT4 v1)
{
	XMVECTOR vector1 = XMLoadFloat4(&v1);
	XMVECTOR length = XMVector4Length(vector1);

    float distance = 0.0f;
    XMStoreFloat(&distance,length);
    return distance;
}

XMFLOAT4 MathHelper::Normalize(XMFLOAT4 v1)
{
	XMVECTOR v1Vec = XMVectorSet(  v1.x, v1.y, v1.z, v1.w );
	v1Vec = XMVector4Normalize(v1Vec);
	XMFLOAT4 v1Nomrlized;
	XMStoreFloat4(&v1Nomrlized, v1Vec);
	return v1Nomrlized;
}

XMFLOAT4 MathHelper::IdentityFloat4()
{
	return XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

XMFLOAT4X4 MathHelper::IdentityFloat4X4()
{
	XMMATRIX identity = XMMatrixIdentity();
	XMFLOAT4X4 returnValue;
	XMStoreFloat4x4(&returnValue, identity);
	return returnValue;
}

XMFLOAT4 operator-(const XMFLOAT4& rh, const XMFLOAT4& lh)
{
	XMFLOAT4 minus = XMFLOAT4(rh.x - lh.x, rh.y - lh.y, rh.z - lh.z, rh.w - lh.w);
	return minus;
}

XMFLOAT4 operator+(const XMFLOAT4& rh, const XMFLOAT4& lh)
{
	XMFLOAT4 added = XMFLOAT4(rh.x + lh.x, rh.y + lh.y, rh.z + lh.z, rh.w + lh.w);
	return added;
}

XMFLOAT4 operator*(const XMFLOAT4& rh, float lh)
{
	XMFLOAT4 returnValue;
	returnValue.x = rh.x * lh;
	returnValue.y = rh.y * lh;
	returnValue.z = rh.z * lh;
	returnValue.w = rh.w * lh;

	return returnValue;
}
XMFLOAT4 operator*(float rh, const XMFLOAT4&lh)
{
	return lh * rh;
}

XMFLOAT4 operator/(const XMFLOAT4& rh, float lh)
{
	XMFLOAT4 returnValue;
	returnValue.x = rh.x / lh;
	returnValue.y = rh.y / lh;
	returnValue.z = rh.z / lh;
	returnValue.w = rh.w / lh;

	return returnValue;
}
XMFLOAT4 operator/(float rh, const XMFLOAT4&lh)
{
	return lh/rh;
}