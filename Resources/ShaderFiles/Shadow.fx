

bool IsInShadow(float4 lightPos,
				Texture2D depthMap,
				SamplerState shadowSampler)
{
	lightPos.xyz /= lightPos.w;

	lightPos.xy += 0.5f;

	float4 depthTexture = depthMap.Sample( shadowSampler, lightPos.xy);

	bool depthWithin = (lightPos.z - depthTexture.x) >= 0.0001;

	return depthWithin && (lightPos.z < 1.0f);
}