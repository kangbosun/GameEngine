
#ifndef _LIGHTING_H_
#define _LIGHTING_H_

#include "Material.h"

struct Light
{
	float4 color;
	float3 dir;	
	float lightType;
	float3 position;
};


Light light;

// physically based rendering
float Schlick(float NdotL, float NdotV, float roughness)
{
	float k = roughness * sqrt(2 / 3.14159265358979);
	float oneMinusK = 1 - k;
	return (NdotL / ((NdotL * oneMinusK) + k)) * 
		(NdotV / ((NdotV * oneMinusK) + k));
}

// percentage-closer filtering shadow map Function
static const float SMAP_SIZE = 2048;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcPCFFactor(SamplerComparisonState samShadow, 
						Texture2D shadowMap,
						float4 shadowPosH)
{
	shadowPosH.xyz /= shadowPosH.w;
	float depth = shadowPosH.z - 0.0003f;
	const float dx = SMAP_DX;
	
	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
	};
	
	[unroll]
	for(int i = 0; i < 9; i++) {
		percentLit += shadowMap.SampleCmpLevelZero(samShadow,
		shadowPosH.xy + offsets[i], depth).r;
	}
	return (percentLit / 9.0f);
}

// Variance Shadow Map Function
float gMinVariance = 0.0000001f;

float2 ComputeMoments(float depth)
{
	float2 moment;
	moment.x = depth;
	float dx = ddx(depth);
	float dy = ddy(depth);
	moment.y = (depth*depth) + (0.25*((dx*dx) + (dy*dy)));
	return moment;
}





interface ILighting 
{
	float GetSpecular(float3 N, float3 V, float3 L);
	float GetDiffuse(float3 N, float3 L);
	float3 GetLightColor();
};

class CLightingDisalbed : ILighting
{
	float GetSpecular(float3 N, float3 V, float3 L) { return 0;}
	float GetDiffuse(float3 N, float3 L) { return 1; }
	float3 GetLightColor() { return float3(1, 1, 1); }
};

class CLigtingEnabled : ILighting
{
	float GetSpecular(float3 N, float3 V, float3 L)
	{
		float F, G, D;
		float3 H = normalize(L + V);
		
		float NdotL = saturate(dot(N, L));
		float NdotV = saturate(dot(N, V));
		float metalic = material.metallic;
		float roughness = material.roughness;
		
		//Fresnel Term
		F = metalic + ((1 - metalic) * pow((1 - dot(L, H)), 5));
		
		//Distribution Term(BlinnPhong)
		float a = (2.0f /(roughness * roughness)) - 2;
		D = (a + 2) * pow(max(dot(N, H), 0), a) / (2 * 3.14159265358979);
		
		//Geometry Term(schlick's approximation)
		G = Schlick(NdotL, NdotV, roughness);
		
		//final
		return ((F * G * D) / (4 * NdotL * NdotV));	
	}
	
	float GetDiffuse(float3 N, float3 L)
	{
		float NdotL = saturate(dot(N, L));
		return (NdotL * (1 - material.metallic));
	}
	
	float3 GetLightColor() { return light.color.rgb; }
};

CLightingDisalbed lightingDisable;
CLigtingEnabled lightingEnable;
ILighting lighting = lightingEnable;

#endif 
