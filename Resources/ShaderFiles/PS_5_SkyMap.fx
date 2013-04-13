#include "Setup.fx"

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 finalLightColour = float4( 1.0f, 1.0f, 1.0f, 1.0f );


	float4 directionToCamera = input.PosWorld - eye;
	normalize( directionToCamera );

	float4 directionOfReflection 
		= reflect( directionToCamera, input.Normal );


	finalLightColour = myCubeMap.Sample( samLinear, directionOfReflection.xyz );
	//finalLightColour = directionOfReflection;
	return finalLightColour;
}