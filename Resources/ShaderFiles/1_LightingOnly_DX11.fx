#include "Setup.fx"

struct VS_INPUT
{
	float4 VertexPos : POSITION;
	float4 VertexNorm : NORMAL;
	float2 tex0 : TEXCOORD0;		
	float2 tex1 : TEXCOORD0;		
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	// Combine the matrices first...
	matrix matFinalMVP = mul( World, CameraView );
	matFinalMVP = mul( matFinalMVP, Projection );
	output.PosMVP = ( input.VertexPos * 5.0f );

	// To place the vertex in the correct location on screen:
	output.PosMVP = mul( input.VertexPos, matFinalMVP );

	// Passed to the pixel shader for correct lighting:
	output.PosWorld = mul( input.VertexPos, World );
	output.Normal = mul( input.VertexNorm, World );

	output.Normal = normalize( output.Normal );

	output.LightMVP = mul( output.LightMVP, World );
	output.LightMVP = mul( output.LightMVP, LightView );
	output.LightMVP = mul( output.LightMVP, Projection );

	// Pass the texture coordinates to the pixel shader
	// (remember, if we don't pass them, the pixel shader is unaware of them)
	output.tex0 = input.tex0;
	//output.tex1 = input.tex1;

	//// Don't draw anything...
	//PS_INPUT outputB = (PS_INPUT)0;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
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

