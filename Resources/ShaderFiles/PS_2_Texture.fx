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
		return objectMaterial.ambient;
	}
	
	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );

	float4 texColour0 = texture00.Sample( samLinear, input.tex0 );
	//float4 texColour0 = texture00.Sample( samLinear, newUV );
	float4 texColour1 = texture01.Sample( samAnisotropic, input.tex1 );

	//float4 cubeColour = cubeMap.Sample( samAnisotropic, Normal );

	float4 finalTexColour = texColour0 + texColour1;

	finalTexColour = normalize( finalTexColour );

	finalTexColour.w = objectMaterial.diffuse.w;

	return finalTexColour;
}