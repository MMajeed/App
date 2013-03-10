#include "HLSL_4_BasicLightFunctions.fx"


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
	float4 eye;
	float4 target;
	LightDesc light[10];		// Light type now in light description
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
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


//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR;
	uint4 JointIndx : JOINTINDEX;
	float4 JointWght : JOINTWEIGHT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS( VS_INPUT input ) 
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	

	uint b1 = input.JointIndx.x;	
	matrix k1 = BoneTransforms[b1];
	float w1 = input.JointWght.x;
	output.Pos += w1 * mul( input.Pos, k1);

	uint b2 = input.JointIndx.y;	
	matrix k2 = BoneTransforms[b2];
	float w2 = input.JointWght.y;
	output.Pos += w2 * mul( input.Pos, k2);

	uint b3 = input.JointIndx.z;	
	matrix k3 = BoneTransforms[b3];
	float w3 = input.JointWght.z;
	output.Pos += w3 * mul( input.Pos, k3);
	
	uint b4 = input.JointIndx.w;	
	matrix k4 = BoneTransforms[b4];
	float w4 = input.JointWght.w;
	output.Pos += w4 * mul( input.Pos, k4);

	output.Pos = mul( output.Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	
	output.Color = input.JointWght;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	return input.Color;
}
