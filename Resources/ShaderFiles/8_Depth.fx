#include "Setup.fx"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_Target
{
	return float4(input.LightMVP.x, input.LightMVP.y, input.LightMVP.z, 1.0f);
}
