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