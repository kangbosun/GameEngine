#include "LightHelper.fx"

#define MAX_BONE 100

// structs
struct VertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	uint4   boneIndices : BLENDINDICES;
	float4 weights : BLENDWEIGHTS;
};

struct ShaderMaterial
{
	float4 color;
	float metallic;
	float roughness;
};

struct GlobalSetting
{
	float4 ambient;
	float shadowMapSize;
};

struct LightData
{
	float4 color;
	float3 dir;	
	float lightType;
	float3 position;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;	
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 shadowPos : TEXCOORD1;
	float4 worldPos : TEXCOORD2;
};

struct RenderOption
{
	bool useAlbedoMap;
	bool useSpecularMap;
	bool useNormalMap;
	bool useShadowMap;
	bool useSkinning;
};

//

// VertexShader variables
float4x4 worldMatrix;
float4x4 viewMatrix;
float4x4 projectionMatrix;
	
float4x4 shadowTransform;

float3 cameraPos;

float4x4 gBones[MAX_BONE];

RenderOption renderOption;

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(0, 0, 0, 0);
	float4 worldPos;
	float4 worldViewPos;
	float3 normal = float3(0, 0, 0);	
	float3 tangent = float3(0, 0, 0);
	float3 binormal = float3(0, 0, 0);
	float4 shadowPos = float4(0, 0, 0, 0);
	
	matrix worldViewMatrix = worldMatrix * viewMatrix;
		
	if(renderOption.useSkinning) {
		[unroll]
		for(int i = 0; i < 4; ++i) {
			pos += input.weights[i] * mul(float4(input.position, 1), gBones[input.boneIndices[i]]);
			normal += input.weights[i] * mul(input.normal, (float3x3)gBones[input.boneIndices[i]]);
			tangent += input.weights[i] * mul(float4(input.tangent, 1), gBones[input.boneIndices[i]]).xyz;
			binormal += input.weights[i] * mul(float4(input.binormal, 1), gBones[input.boneIndices[i]]).xyz;
		}
	}
	else {
		pos = float4(input.position, 1);
		normal = input.normal;
		tangent = input.tangent;
		binormal = input.binormal;
	}
	
	worldPos = mul(pos, worldMatrix);
	worldViewPos = mul(worldPos, viewMatrix);
	pos = mul(worldViewPos, projectionMatrix);
		
	// for shadow mapping
	shadowPos = mul(worldPos, shadowTransform);
	
	// for normalMap;
	tangent = normalize(mul(float4(tangent, 1), worldMatrix)).xyz;
	binormal = normalize(mul(float4(binormal, 1), worldMatrix).xyz);
	
	output.position = pos;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(normal, (float3x3)worldMatrix));
	output.shadowPos = shadowPos;
	output.tangent = tangent;
	output.binormal = binormal;
	output.worldPos = worldPos;
	return output;
}

SamplerState samplerStateLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState samplerStatePoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerComparisonState samplerStateShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ComparisonFunc = LESS_EQUAL;
};

Texture2D albedoMap;
Texture2D specularMap;
Texture2D normalMap;

Texture2D shadowMap;

LightData light;
GlobalSetting globalSetting;
ShaderMaterial material;

float3 applyNormalMap(float3 tangent, float3 binormal, float3 normal, Texture2D normalMap, float2 texcoord)
{
	float4 bump = normalMap.Sample(samplerStateLinear, texcoord);
	bump = (bump * 2.0f) - 1.0f;
	normal = normal + (bump.x * tangent) + (bump.y * binormal);
	return normalize(normal);
}

float4 PS(PixelInput input) : SV_TARGET
{
	float4 albedoColor = float4(1, 1, 1, 1);
	float4 colorFinal = float4(0, 0, 0, 1);
		
	albedoColor = renderOption.useAlbedoMap ? albedoMap.Sample(samplerStateLinear, input.texcoord)
												: albedoColor;
	
	
	float3 normal = renderOption.useNormalMap ? applyNormalMap(input.tangent, input.binormal, input.normal, normalMap, input.texcoord)
                                              :	input.normal;
	//float3 normal = input.normal;
	// calc light 
	float3 N = normalize(normal);
	float3 L = -normalize(light.dir);
	float3 V = normalize(cameraPos - input.worldPos.xyz);
	float3 H = normalize(L + V);
	
	float NdotL = saturate(dot(N, L));
	
	// shadow
	float shadowFactor = 1.0f;
	if(renderOption.useShadowMap) {
		float3 shadow = (input.shadowPos.xyz / input.shadowPos.w);
		shadowFactor = CalcVSMFactor(shadowMap, samplerStateLinear, shadow.xy, shadow.z);
	}
	
	float BRDF = calcBRDF(material.metallic, material.roughness, N, V, L, H);
	float4 tempColor = float4(light.color.rgb * shadowFactor * BRDF, 1);
	colorFinal += saturate(tempColor);
	
	colorFinal += globalSetting.ambient;
	colorFinal *= albedoColor;
		
	colorFinal = saturate(colorFinal);
	return (colorFinal);
}



////////////////////////////////////////////////////////////
//deferred

struct GBufferOut
{
	half2 normal : SV_Target0;
	float4 diffuseMetallic : SV_Target1;
	float4 specularRoughness : SV_Target2;
	float4 position : SV_Target3; 
};

half2 encodeNormal(float3 n)
{
	half2 enc = half2(normalize(n.xy) * sqrt(-n.z*0.5+0.5));
	enc = enc * 0.5 + 0.5;
	return enc;
}

half3 decodeNormal(half2 enc)
{
	half4 nn = (2 * half4(enc, 0, 0)) + half4(-1, -1, 1, -1);
	half l = dot(nn.xyz, -nn.xyw);
	nn.z = l;
	nn.xy *= (half)sqrt(l);
	return nn.xyz * 2 + half3(0, 0, -1);
}

GBufferOut GBufferPS(PixelInput input)
{
	GBufferOut bufferOut;
	float4 albedoColor = float4(0, 0, 0, 0);
	
	clip(material.color.a - 0.1f);
	
	if(renderOption.useAlbedoMap) {
		albedoColor = albedoMap.Sample(samplerStateLinear, input.texcoord).rgba;
		clip(albedoColor.a - 0.1f);
	}
	
	float3 normal = input.normal;
	if(renderOption.useNormalMap) {
		float4 bump = normalMap.Sample(samplerStateLinear, input.texcoord);
		bump = (bump * 2.0f) - 1.0f;
		normal = normal + bump.x * input.tangent + bump.y * input.binormal;
		normal = normalize(normal);
	}
	
	bufferOut.normal = encodeNormal(normal);
	bufferOut.diffuseMetallic = float4(albedoColor.rgb, material.metallic);
	bufferOut.specularRoughness = float4(1, 1, 1, material.roughness);
	bufferOut.position = input.position;
	
	return bufferOut;
}

technique11 PBS
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

technique11 PBS_Deferred
{
	pass geometryPass
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, GBufferPS()));
	}
}






