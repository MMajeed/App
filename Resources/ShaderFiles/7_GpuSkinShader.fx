#include "Setup.fx"

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float4 Color : COLOR;
	uint4 JointIndx : JOINTINDEX;
	float4 JointWght : JOINTWEIGHT;
};

// Vertex Shader
PS_INPUT VS( VS_INPUT input ) 
{
    PS_INPUT output = (PS_INPUT)0;
	
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
	output.PosMVP = mul( output.PosMVP, CameraView );
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

	
	// Set LightMVP	
	output.LightMVP = VertexPosSkined;
	output.LightMVP = mul( output.LightMVP, World );
	output.LightMVP = mul( output.LightMVP, LightView );
	output.LightMVP = mul( output.LightMVP, Projection );

	// Set Color
	output.Color = input.Color;


    return output;
}


// Pixel Shader
float4 PS( PS_INPUT input ) : SV_Target
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
