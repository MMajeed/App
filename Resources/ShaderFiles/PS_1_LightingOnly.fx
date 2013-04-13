#include "Setup.fx"

float4 PS( PS_INPUT input ) : SV_Target
{
	bool inShadow = IsInShadow(input.LightMVP, Shadow, samShadow);
	if(inShadow)
	{
		return objectMaterial.ambient;
	}

	float4 finalLightColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );

	for ( int index = 0; index < 10; index++ )
	{
		if ( light[index].lightPowerRangeType.z > 2.9f && light[index].lightPowerRangeType.z < 3.1f ) // Don't do light
		{			
			continue;
		}
		else if ( light[index].lightPowerRangeType.z == 0.0f ) // Parallel light
		{
			finalLightColour += ParallelLight( objectMaterial, light[index], 
										 input.PosWorld, 
										 input.NormalWorld, eye );	
		}
		else if ( light[index].lightPowerRangeType.z == 1.0f ) // Point
		{
			finalLightColour += PointLight(objectMaterial, light[index], 
									 input.PosWorld, 
									 input.NormalWorld, eye );
		}
		else if ( light[index].lightPowerRangeType.z > 1.0f ) // Point
		{
			finalLightColour += Spotlight( objectMaterial, light[index], 
									 input.PosWorld, 
									 input.NormalWorld, eye );
		}
	}


	finalLightColour = saturate( finalLightColour );

	finalLightColour.w = objectMaterial.diffuse.w;

	return finalLightColour;
}