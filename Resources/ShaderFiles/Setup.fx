#include "HLSL_4_BasicLightFunctions.fx"
#include "Shadow.fx"


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbNeverChanges : register( b0 )
{
    matrix CameraView;
	matrix LightView;
	matrix specialMatrix;
	float4 eye;
	float4 target;
	LightDesc light[10];		// Light type now in light description
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
	matrix lightProject;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
	MaterialInfo objectMaterial;
};

cbuffer AnimMatrices : register( b3 )
{
	matrix BoneTransforms[128];
};

SamplerState samLinear : register( s0 );
SamplerState samAnisotropic : register( s1 );


SamplerState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};



Texture2D texture00 : register( t0 );
Texture2D texture01 : register( t1 );
Texture2D texture02 : register( t2 );
Texture2D texture03 : register( t3 );
Texture2D texture04 : register( t4 );

TextureCube myCubeMap : register( t5 );

Texture2D Shadow : register( t10 );

//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 PosMVP      : SV_POSITION;
	float4 PosWorld    : POSITION;
	float4 Normal      : NORMAL1;
	float4 NormalWorld : NORMAL2;
	float4 LightMVP    : LightMVP;
    float4 Color       : COLOR0;
	float2 tex0        : TEXCOORD0;		
	float2 tex1        : TEXCOORD0;		
};
