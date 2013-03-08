#ifndef HELPERFUNCS_H
#define HELPERFUNCS_H

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "cBuffer.h"

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct AnimMatrices
{
	XMMATRIX mBoneMatrices[128];
};

struct Joint
{
    _XMFLOAT4X4 invBindPose;
    int      parent;
    char     name[28];

    Joint() : parent(0) 
    { name[0]='\0'; }
};

struct JointPose
{
    XMFLOAT3 translation;
    XMFLOAT4 rotation;
    XMFLOAT3 scale;

    XMMATRIX GetTransform() const;
};

struct SimpleSkinnedVertex
{
    XMFLOAT3      Pos;              
    XMFLOAT4      Color;            
    unsigned char JointIndex[4];    
    float         JointWeight[4];   

    SimpleSkinnedVertex() : Pos(0.0f, 0.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f) 
    { JointIndex[0] = JointIndex[1] = JointIndex[2] = JointIndex[3] = 0; JointWeight[0] = JointWeight[1] = JointWeight[2] = JointWeight[3] = 0.0f; }
};

#endif
