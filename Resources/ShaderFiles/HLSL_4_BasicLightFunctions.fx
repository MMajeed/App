// This is used to specify what the material property of the light and objects are
struct MaterialInfo
{
    float4 diffuse;
	float4 ambient;
    float4 spec;
};

struct LightDesc
{
	float4 pos;			// Only used with point and spot
	float4 dir;			// Only used with directional(parallel) and spot
	MaterialInfo material;	// ambient, diffuse, spec
	float4 attenuation;
	// x = power, y = range, z = light type, w = not used
	// 0 = parallel, 1 = point, >1 = spot
	float4 lightPowerRangeType;	
};

// v = material description of the surface
// L = lighting description, including material
// pos = world space vertex position
// normal = world space vertex normal
// eyePos = camera (eye) world position
float4 PointLight(MaterialInfo material, LightDesc light, float4 pos, float4 normal, float4 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	normal = normalize( normal );

	// The vector from the surface to the light.
	float3 lightPos = light.pos.xyz;
	float3 vertexPos = pos.xyz;
	float3 lightVec = lightPos - vertexPos;
		//
	// The distance from surface to light.
	float d = length(lightVec);

	if( d > light.lightPowerRangeType.y )			// y element is range of spot or point light
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);		// If it's too far, set colour to black.
		// Note: since it's black, we don't need to to any more calculations.
	}

	// Normalize the light vector.
	lightVec /= d;			//normalize( lightVec );

	// Add the ambient light term.
	// 	litColor += v.diffuse.xyz * L.material.ambient.xyz;
	litColor += material.diffuse.rgb * light.material.ambient.rgb;			// Same as above: xyzw same as rgba

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal.xyz);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(material.spec.a, 1.0f);
		float3 toEye     = normalize( eyePos.xyz - vertexPos.xyz );
		float3 R         = reflect( -lightVec, normal.xyz );
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);

		// diffuse and specular terms
		litColor += diffuseFactor * material.diffuse.rgb * light.material.diffuse.rgb;
		litColor += specFactor * material.spec.rgb * light.material.spec.rgb;
	}
	
	// attenuate
	litColor =  litColor / dot( light.attenuation.rgb, float3(1.0f, d, d*d));

	return float4( litColor.xyz, 1.0f );
}

//float3 ParallelLight( vSurfaceInfo, Light L, float3 eyePos)
float4 ParallelLight( MaterialInfo material, LightDesc light, float4 pos, float4 normal, float4 eyePos )
{
	normal = normalize( normal );

	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -light.dir.xyz;
	
	// Add the ambient term.
	litColor += material.diffuse.rgb * light.material.ambient.rgb;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot( lightVec, normal.xyz );

	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(material.spec.a, 1.0f);
		float3 toEye     = normalize( eyePos.xyz - pos.xyz );
		float3 R         = reflect( lightVec, normal.xyz );
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
					
		

		// diffuse and specular terms
		litColor += diffuseFactor * material.diffuse.rgb * light.material.diffuse.rgb;
		litColor += specFactor * material.spec.rgb * light.material.spec.rgb;
	}
	
	return float4( litColor.rgb, 1.0f );
}

//float3 Spotlight(SurfaceInfo v, Light L, float3 eyePos)
float4 Spotlight( MaterialInfo material, LightDesc light, float4 pos, float4 normal, float4 eyePos )
{
	normal = normalize( normal );
	
	float4 litColor = PointLight(material, light, pos, normal, eyePos);
			
	// The vector from the surface to the light.
	float3 lightVec = normalize(light.pos - pos);
	
	float s = pow(max(dot(-lightVec, light.dir), 0.0f), light.lightPowerRangeType.x);
	
	// Scale color by spotlight factor.
	return litColor*s;
}