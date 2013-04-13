#include "Setup.fx"

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 texColour1 = texture01.Sample( samLinear, input.tex0 );

	float4 finalTexColour;

	if ( texColour1.x < 0.999 )	  // closer to 'black'
	{
		float2 x = input.tex0;
		x.x *= -1;
		x.y *= -1;
		float4 texColour0 = texture00.Sample( samAnisotropic, x );
		normalize(texColour0);
		finalTexColour = texColour0 ;
		finalTexColour.x = 0.0f;
		finalTexColour.z = 0.0f;
	}	
	else
	{
		finalTexColour = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	return finalTexColour;
}