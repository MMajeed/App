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
	MaterialInfo objectMaterial;
};
//************************************************/

Texture2D texture00 : register( t0 );
Texture2D texture01 : register( t1 );
// And the cube map
TextureCube   myCubeMap : register( t2 );


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
	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );

	for ( int index = 0; index < 10; index++ )
	{
		if ( light[index].lightPowerRangeType.z == 0.0f ) // Parallel light
		{
			finalLightColour += ParallelLight( objectMaterial, light[index], 
										 input.VertexPosWorld, 
										 input.VertexNormalWorld, eye );	
		}
		else if ( light[index].lightPowerRangeType.z == 1.0f ) // Point
		{
			finalLightColour += PointLight(objectMaterial, light[index], 
									 input.VertexPosWorld, 
									 input.VertexNormalWorld, eye );
		}
		else 
		{
			finalLightColour += Spotlight( objectMaterial, light[index], 
									 input.VertexPosWorld, 
									 input.VertexNormalWorld, eye );
		}
	}	// for (int index = 0;....

	// Since we are adding all lights, they will be GT 1.0.
	//finalLightColour = saturate( finalColour );
	//finalLightColour = normalize( finalColour );	
	//finalLightColour = normalize( finalLightColour );

	float4 texColour0 = texture00.Sample( samLinear, input.tex0 );
	float4 texColour1 = texture01.Sample( samAnisotropic, input.tex1 );

	float4 finalTexColour = texColour0 + texColour1;

	finalTexColour = normalize( finalTexColour );

	// NOTE that these are multiplied (aka "modulate")
	float4 finalColour =  finalLightColour * finalTexColour;
	//float4 finalColour =  texColour1;

	finalLightColour = saturate( finalColour );

	// Show only texture 1
	finalLightColour = texColour0;


	// And.... we're going to ignore that (in this case)
	// and do a look up in the cube map...
	float4 cubeColour 
			= myCubeMap.Sample( samLinear, input.VertexNormalWorld );

	finalLightColour = cubeColour;


	// Use reflect to get the angle from the surface from the camera
	float4 directionToCamera = input.VertexPosWorld - eye;
	normalize( directionToCamera );

	float4 directionOfReflection 
			= reflect( directionToCamera, input.VertexNormalWorld );
	// 2.419 is index of refraction for diamond
	// 1.50-ish is for glass
	//float4 directionOfReflection 
			//= refract( directionToCamera, input.VertexNormalWorld, 2.419f );
	//normalize( directionOfReflection );

	finalLightColour = myCubeMap.Sample( samLinear, directionOfReflection.xyz );
	//refract();

	finalLightColour.w = objectMaterial.diffuse.w;

	return finalLightColour;
}

