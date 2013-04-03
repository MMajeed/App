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
    float4 PosMVP : SV_POSITION;
	float4 PosWorld : POSITION;
	float4 Normal : NORMAL;
    float4 Color : COLOR0;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
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
	
	uint JoinIndexArray[4] = { input.JointIndx.x, input.JointIndx.y, input.JointIndx.z, input.JointIndx.w } ;
	float JointWghtArray[4] = { input.JointWght.x, input.JointWght.y, input.JointWght.z, input.JointWght.w } ;
	
	
	float4 VertexPosSkined = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0; i < 4; ++i)
	{
		uint b1 = JoinIndexArray[i];	
		matrix k1 = BoneTransforms[b1];
		float w1 = JointWghtArray[i];
		VertexPosSkined += w1 * mul( input.Pos, k1);
	}
	output.PosMVP = VertexPosSkined;
	output.PosMVP = mul( output.PosMVP, World );
	// Set pos World
	output.PosWorld = output.PosMVP;
	// Set pos MVP	
	output.PosMVP = mul( output.PosMVP, View );
	output.PosMVP = mul( output.PosMVP, Projection );
	
	// Set Normal
	float4 VertexNormalSkined = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0; i < 4; ++i)
	{
		uint b1 = JoinIndexArray[i];	
		matrix k1 = BoneTransforms[b1];
		float w1 = JointWghtArray[i];
		VertexNormalSkined += w1 * mul( input.Normal, k1);
	}
	output.Normal = normalize(VertexNormalSkined);

	// Set Color
	output.Color = input.Color;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );

	for ( int index = 0; index < 10; index++ )
	{
		if ( light[index].lightPowerRangeType.z > 2.9f && light[index].lightPowerRangeType.z < 3.1f ) // Don't do light
		{			
			continue;
		}
		else if ( light[index].lightPowerRangeType.z == 0.0f ) // Parallel light
		{
			finalLightColour += ParallelLight( objectMaterial, light[index], 
										 input.PosWorld, 
										 input.Normal, eye );	
		}
		else if ( light[index].lightPowerRangeType.z == 1.0f ) // Point
		{
			finalLightColour += PointLight(objectMaterial, light[index], 
									 input.PosWorld, 
									 input.Normal, eye );
		}
		else if ( light[index].lightPowerRangeType.z > 1.0f ) // Point
		{
			finalLightColour += Spotlight( objectMaterial, light[index], 
									 input.PosWorld, 
									 input.Normal, eye );
		}
	}


	finalLightColour = saturate( finalLightColour );

	finalLightColour.w = objectMaterial.diffuse.w;

	return finalLightColour;

}
