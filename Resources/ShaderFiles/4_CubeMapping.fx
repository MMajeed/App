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

	output.Normal = input.VertexNorm;

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
	

	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	normalize( input.Normal );
	float4 cubeColour 
			= myCubeMap.Sample( samLinear, input.Normal );

	float4 lightPos = input.LightMVP;
	lightPos.xyz /= lightPos.w;

	lightPos.xy += 0.5f;

	float4 depthTexture = Shadow.Sample( samShadow, lightPos.xy);

	bool depthWithin = (lightPos.z - depthTexture.x) >= 0.0001;
	if(depthWithin && lightPos.z < 1.0f)
	{
		cubeColour *= 0.1f;
	}

	return cubeColour;
}

