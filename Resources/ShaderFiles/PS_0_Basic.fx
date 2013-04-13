#include "Setup.fx"

float4 PS( PS_INPUT input ) : SV_Target
{
	bool inShadow = IsInShadow(input.LightMVP, Shadow, samShadow);
	if(inShadow)
	{
		return float4(0.1f, 0.1f, 0.1f, 1.0f);
	}

	float4 finalLightColour = objectMaterial.diffuse;

	return finalLightColour;
}