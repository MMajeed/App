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

	output.PosMVP = input.VertexPos;
	output.PosMVP = mul( output.PosMVP, World );
	output.PosMVP = mul( output.PosMVP, CameraView );
	output.PosMVP = mul( output.PosMVP, Projection );

	// Passed to the pixel shader for correct lighting:
	output.PosWorld = mul( input.VertexPos, World );

	output.Normal = mul( input.VertexNorm, World );
	output.Normal = normalize( output.Normal );

	output.LightMVP = input.VertexPos;
	output.LightMVP = mul( output.LightMVP, World );
	output.LightMVP = mul( output.LightMVP, LightView );
	output.LightMVP = mul( output.LightMVP, lightProject );
	output.LightMVP = mul( output.LightMVP, specialMatrix );

	// Pass the texture coordinates to the pixel shader
	output.tex0 = input.tex0;
	output.tex1 = input.tex1;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	float4 lightPos = input.LightMVP;
	lightPos.xyz /= lightPos.w;

	lightPos.xy += 0.5f;

	float4 depthTexture = Shadow.Sample( samShadow, lightPos.xy);
	
	bool depthWithin = (lightPos.z - depthTexture.x) >= 0.0001;
	if(depthWithin && lightPos.z < 1.0f)
	{
		return objectMaterial.ambient;
	}

	float4 texColour0 = texture00.Sample( samLinear, input.tex0 );
	float4 texColour1 = texture01.Sample( samAnisotropic, input.tex1 );

	float4 finalTexColour = texColour0 + texColour1;

	MaterialInfo colorInfo = objectMaterial;
	colorInfo.diffuse = finalTexColour;

	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );

	for ( int index = 0; index < 10; index++ )
	{
		if ( light[index].lightPowerRangeType.z > 2.9f && light[index].lightPowerRangeType.z < 3.1f ) // Don't do light
		{			
			continue;
		}
		else if ( light[index].lightPowerRangeType.z == 0.0f ) // Parallel light
		{
			finalLightColour += ParallelLight( colorInfo, light[index], 
										 input.PosWorld, 
										 input.Normal, eye );	
		}
		else if ( light[index].lightPowerRangeType.z == 1.0f ) // Point
		{
			finalLightColour += PointLight(colorInfo, light[index], 
									 input.PosWorld, 
									 input.Normal, eye );
		}
		else if ( light[index].lightPowerRangeType.z > 1.0f ) // Point
		{
			finalLightColour += Spotlight( colorInfo, light[index], 
									 input.PosWorld, 
									 input.Normal, eye );
		}
	}

	finalLightColour.w = objectMaterial.diffuse.w;

	return finalLightColour;
}

