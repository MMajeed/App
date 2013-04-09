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
	output.PosMVP = input.VertexPos;

	// To place the vertex in the correct location on screen:
	output.PosMVP = mul( input.VertexPos, matFinalMVP );

	// Passed to the pixel shader for correct lighting:
	output.PosWorld = mul( input.VertexPos, World );
	output.Normal = mul( input.VertexNorm, World );;

	output.LightMVP = mul( output.LightMVP, World );
	output.LightMVP = mul( output.LightMVP, LightView );
	output.LightMVP = mul( output.LightMVP, Projection );

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalLightColour = objectMaterial.diffuse;

	return finalLightColour;
}

