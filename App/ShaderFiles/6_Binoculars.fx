// The lighting functions are (modified) from Frank Luna's DirectX 10 book (chapter 6)

#include "HLSL_4_BasicLightFunctions.fx"

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbNeverChanges : register( b0 )
{
    matrix mView;
	float4 eye;
	float4 target;
	LightDesc light[10];		// Light type now in light description
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix mProjection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix mWorld;
	//matrix mTexture;		// I've translated it (i.e. moved it in x...)
	MaterialInfo objectMaterial;
};
//************************************************/

Texture2D texture00 : register( t0 );
Texture2D texture01 : register( t1 );
//TextureCube cubeMap : register( t2 );


SamplerState samLinear : register( s0 );
SamplerState samAnisotropic : register( s1 );

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
	matrix matFinalMVP = mul( mWorld, mView );
	matFinalMVP = mul( matFinalMVP, mProjection );

	output.VertexPosMVP = input.VertexPos;
	// To place the vertex in the correct location on screen:
	output.VertexPosMVP = mul( input.VertexPos, matFinalMVP );

	// Passed to the pixel shader for correct lighting:
	output.VertexPosWorld = mul( input.VertexPos, mWorld );
	output.VertexNormalWorld = mul( input.VertexNorm, mWorld );

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
	float4 texColour1 = texture01.Sample( samLinear, input.tex0 );

	float4 finalTexColour;

	if ( texColour1.x < 0.999 )	  // closer to 'black'
	{
		float2 x = input.tex1;
		x.x *= -1;
		x.y *= -1;
		float4 texColour0 = texture00.Sample( samAnisotropic, x );
		finalTexColour = texColour0 ;
		finalTexColour.x = 0.0f;
		finalTexColour.z = 0.0f;
	}	
	else
	{
		finalTexColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	return finalTexColour;
}

