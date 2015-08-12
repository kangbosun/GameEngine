 
#ifndef _DIFFUSEMAPPING_FXH_
#define _DIFFUSEMAPPING_FXH_
 
Texture2D diffuseMap;
SamplerState diffuseMapSamplerState
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};
 
interface IDiffuseMapping
{
	float4 GetDiffuseColor(float2 texcoord);
};
 
class CDiffuseMappingDisalbed : IDiffuseMapping
{
	float4 GetDiffuseColor(float2 texcoord)
	{
		return float4(1, 1, 1, 1);
	}
};

class CDiffuseMappingEnabled : IDiffuseMapping
{
	float4 GetDiffuseColor(float2 texcoord)
	{
		return diffuseMap.Sample(diffuseMapSamplerState, texcoord);
	}
};
 

CDiffuseMappingDisalbed diffuseMapDisable;
CDiffuseMappingEnabled diffuseMapEnable;
IDiffuseMapping diffuseMapping = diffuseMapEnable;
 
#endif 
 