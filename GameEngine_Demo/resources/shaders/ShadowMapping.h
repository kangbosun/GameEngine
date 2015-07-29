
#ifndef _SHADOWMAPPING_H_
#define _SHADOWMAPPING_H_


Texture2D shadowMap;
float4x4 shadowTransform;
SamplerState shadowMapSamplerState
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

interface IShadowMapping
{
	float GetShadowFactor(float4 shadowPos);
	float4 GetVSShadowPos(float4 worldPos);
};

class CShadowMappingDisabled : IShadowMapping
{
	float GetShadowFactor(float4 shadowPos)
	{
		return 1;
	}
	float4 GetVSShadowPos(float4 worldPos)
	{
		return float4(0, 0, 0, 0);
	}
};


class CShadowMappingEnabled : IShadowMapping
{
	float GetShadowFactor(float4 shadowPos)
	{
		float3 ss = shadowPos.xyz / shadowPos.w;
		float2 texcoord = ss.xy;
		float dist = ss.z;

		float2 moment = shadowMap.Sample(shadowMapSamplerState, texcoord).xy;
		
		// Calculate ChebyshevUpperBound
		float p = (dist <= moment.x);
		float variance = moment.y - (moment.x * moment.x);
		variance = max(variance, gMinVariance);
		
		float d = dist - moment.x;
		float pMax = variance / (variance + (d * d));
		return smoothstep(0, 1, pMax);
	}
	float4 GetVSShadowPos(float4 worldPos)
	{
		return mul(worldPos, shadowTransform);
	}
};



CShadowMappingDisabled shadowMapDisable;
CShadowMappingEnabled shadowMapEnable;
IShadowMapping shadowMapping = shadowMapDisable;

#endif 