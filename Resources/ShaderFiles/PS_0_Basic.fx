#include "Setup.fx"

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 lightPos = input.LightMVP;
	lightPos.xyz /= lightPos.w;

	lightPos.xy += 0.5f;

	float4 depthTexture = Shadow.Sample( samShadow, lightPos.xy);

	bool depthWithin = (lightPos.z - depthTexture.x) >= 0.0001;
	if(depthWithin && lightPos.z < 1.0f)
	{
		return float4(0.1f, 0.1f, 0.1f, 1.0f);
	}

	float4 finalLightColour = objectMaterial.diffuse;

	return finalLightColour;
}