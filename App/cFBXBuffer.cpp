#include "cFBXBuffer.h"

XMMATRIX cFBXBuffer::JointPose::GetTransform() const
{
    XMMATRIX m = XMMatrixScaling(scale.x, scale.y, scale.z) *
                 XMMatrixRotationQuaternion(XMLoadFloat4(&rotation)) *
                 XMMatrixTranslation(translation.x, translation.y, translation.z);
    return(m);
}