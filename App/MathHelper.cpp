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

XMFLOAT4 MathHelper::Normalize(XMFLOAT4 v1)
{
	XMVECTOR v1Vec = XMVectorSet(  v1.x, v1.y, v1.z, v1.w );
	v1Vec = XMVector4Normalize(v1Vec);
	XMFLOAT4 v1Nomrlized;
	XMStoreFloat4(&v1Nomrlized, v1Vec);
	return v1Nomrlized;
}