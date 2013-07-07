#include "Setup.fx"

struct VS_INPUT
{
	float4 VertexPos : POSITION;
	float4 VertexNorm : NORMAL;
	float2 tex0 : TEXCOORD0;		
	float2 tex1 : TEXCOORD0;		
};

// Vertex Shader
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	// Combine the matrices first...
	output.PosMVP = input.VertexPos;
	output.PosMVP = mul( output.PosMVP, World );
	output.PosMVP = mul( output.PosMVP, CameraView );
	output.PosMVP = mul( output.PosMVP, Projection );

	// Passed to the pixel shader for correct lighting:
	output.PosWorld = mul( input.VertexPos, World );

	output.Normal = input.VertexNorm;

	output.NormalWorld = mul( input.VertexNorm, World );
	output.NormalWorld = normalize( output.NormalWorld );

	output.LightMVP = input.VertexPos;
	output.LightMVP = mul( output.LightMVP, World );
	output.LightMVP = mul( output.LightMVP, LightView );
	output.LightMVP = mul( output.LightMVP, lightProject );
	static const matrix T = matrix(0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	output.LightMVP = mul( output.LightMVP, T );

	// Pass the texture coordinates to the pixel shader
	output.tex0 = input.tex0;
	output.tex1 = input.tex1;

    return output;
}

