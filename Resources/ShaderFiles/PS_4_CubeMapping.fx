#include "Setup.fx"

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	normalize( input.Normal );
	float4 cubeColour 
			= myCubeMap.Sample( samLinear, input.Normal );

	bool inShadow = IsInShadow(input.LightMVP, Shadow, samShadow);
	if(inShadow)
	{
		cubeColour *= 0.1f;
	}

	return cubeColour;
}

