#include "Setup.fx"

struct VS_INPUT
{
	float4 VertexPos : POSITION;
	float4 VertexNorm : NORMAL;
	float2 tex0 : TEXCOORD0;		
	float2 tex1 : TEXCOORD0;		
};

struct PS_INPUT		// DX11
{
    float4 VertexPosMVP : SV_POSITION;
	float4 VertexPosWorld : POSITION;
	float4 VertexNormalWorld : NORMAL;
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
	matrix matFinalMVP = mul( World, View );
	matFinalMVP = mul( matFinalMVP, Projection );

	output.VertexPosMVP = input.VertexPos;
	// To place the vertex in the correct location on screen:
	output.VertexPosMVP = mul( input.VertexPos, matFinalMVP );

	// Passed to the pixel shader for correct lighting:
	output.VertexPosWorld = mul( input.VertexPos, World );
	output.VertexNormalWorld = mul( input.VertexNorm, World );

	output.VertexNormalWorld = normalize( output.VertexNormalWorld );

	// Pass the texture coordinates to the pixel shader
	// (remember, if we don't pass them, the pixel shader is unaware of them)
	output.tex0 = input.tex0;
	//output.tex1 = input.tex1;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );

	float4 texColour0 = texture00.Sample( samLinear, input.tex0 );
	//float4 texColour0 = texture00.Sample( samLinear, newUV );
	float4 texColour1 = texture01.Sample( samAnisotropic, input.tex1 );

	//float4 cubeColour = cubeMap.Sample( samAnisotropic, VertexNormalWorld );

	float4 finalTexColour = texColour0 + texColour1;

	finalTexColour = normalize( finalTexColour );

	finalTexColour.w = objectMaterial.diffuse.w;

	return finalTexColour;
}

