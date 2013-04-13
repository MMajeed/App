#include "Setup.fx"

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

