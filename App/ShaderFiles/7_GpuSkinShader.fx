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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR, uint4 JointIndx : JOINTINDEX, float4 JointWght : JOINTWEIGHT ) 
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	
	output.Pos = Pos;

	uint b1 = JointIndx.x;	
	matrix k1 = BoneTransforms[b1];
	float w1 = JointWght.x;
	output.Pos = w1 * mul( output.Pos, k1);
	
	output.Pos = mul( output.Pos, World );
	output.Pos = mul( output.Pos, View );
	output.Pos = mul( output.Pos, Projection );

	
	output.Color = JointWght;// mul( Pos, World );

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    //return float4( 1.0f, 1.0f, 0.0f, 1.0f );    // Yellow, with Alpha = 1
	return input.Color;
	//return float4( 0.0f, 1.0f, 1.0f, 1.0f );
}
