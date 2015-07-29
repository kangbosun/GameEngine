
#ifndef _NORMALMAPPING_H_
#define _NORMALMAPPING_H_

Texture2D normalMap;
SamplerState normalMapSamplerState
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

interface INormalMapping
 {
	float3 GetNormal(float2 texcoord, float3 tangent, float3 binormal, float3 normal);
 };
 
  class CNormalMapDisabled : INormalMapping
 {
 	float3 GetNormal(float2 texcoord, float3 tangent, float3 binormal, float3 normal)
	{
		return normal;
	}
 };
 
 class CNormalMapEnabled : INormalMapping
 {
 	float3 GetNormal(float2 texcoord, float3 tangent, float3 binormal, float3 normal)
	{
		tangent = normalize(tangent);
		binormal = normalize(binormal);
		normal = normalize(normal);
		
		float2 bump = normalMap.Sample(normalMapSamplerState, texcoord).xy;
		bump = (bump * 2) - 1;
		
		normal = normal + (bump.x * tangent) + (bump.y * binormal);
		return normalize(normal);
	}
 };
 
 

 CNormalMapDisabled normalMapDisable;
 CNormalMapEnabled normalMapEnable;
 INormalMapping normalMapping = normalMapDisable;
 
 #endif 