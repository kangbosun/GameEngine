#include "LightHelper.fx"

#define MAX_BONE 100

struct VertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct SkinnedVertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	uint4   boneIndices : BONEINDICES;
	float4 weights : WEIGHTS;
};

struct ShaderMaterial
{
	float4 emissive;
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float shininess;
};

struct GlobalSetting
{
	float4 ambient;
	float shadowMapSize;
};

struct LightBuffer
{
	float4 color;
	float4 position;
};

cbuffer ViewProj : register(c0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer World : register(c1)
{
	matrix worldMatrix;
	float4 color;
};

matrix gBones[MAX_BONE];


struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float4 shadowPos : TEXCOORD1;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

float4x4 shadowTransform;

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(input.position, 1);
	
	output.position = pos = mul(pos, worldMatrix);
	
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = color;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
	
	// for shadow mapping
	output.shadowPos = mul(pos, shadowTransform);
	
	output.tangent = normalize(mul(input.tangent, (float3x3)worldMatrix));
	output.binormal = normalize(mul(input.binormal, (float3x3)worldMatrix));
	
	return output;
}

PixelInput SkinnedVS(SkinnedVertexInput input)
{
	PixelInput output;
	float4 pos = float4(0, 0, 0, 0);
	float3 normal = float3(0, 0, 0);
	float3 tangent = float3(0, 0, 0);
	float3 binormal = float3(0, 0, 0);
	
	
	for(int i = 0; i < 4; i++) {
		pos += input.weights[i] * mul(float4(input.position, 1), gBones[input.boneIndices[i]]);
		normal += input.weights[i] * mul(input.normal, (float3x3)gBones[input.boneIndices[i]]);
		tangent += input.weights[i] * mul(float4(input.tangent, 1), gBones[input.boneIndices[i]]).xyz;
		binormal += input.weights[i] * mul(float4(input.binormal, 1), gBones[input.boneIndices[i]]).xyz;
	}
	
	output.position = mul(pos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	output.color = color;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(normal, (float3x3)worldMatrix));
	
	// for shadow mapping
	output.shadowPos = mul(pos, shadowTransform);
	
	output.tangent = normalize(mul(float4(tangent, 1), worldMatrix)).xyz;
	output.binormal = normalize(mul(float4(binormal, 1), worldMatrix).xyz);
	
	return output;
}

Texture2D diffuseMap;
Texture2D shadowMap;

LightBuffer light;
GlobalSetting globalSetting;

SamplerState state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ComparisonFunc = LESS_EQUAL;
};

ShaderMaterial material;

float4 PS(PixelInput input, uniform bool receiveShadow) : SV_TARGET
{
	float4 colorTex = float4(1, 1, 1, 1);
	float iDiffuse = 0;
	float4 colorDiffuse = 0;
	float4 colorFinal = float4(0, 0, 0, 1);
	float shadowFactor = 1.0f;
	float3 normal = input.normal;
	
	colorTex = diffuseMap.Sample(state, input.texcoord);
	clip(colorTex.a - 0.1f);
	
	// calc light 
	iDiffuse = CalcDiffuse(light.position, light.color, normal);
	if(iDiffuse > 0.0f) {
		// shadow
		float3 shadow = (input.shadowPos.xyz / input.shadowPos.w);
		shadowFactor = receiveShadow ? 
			CalcVSMFactor(shadowMap, state, shadow.xy, shadow.z) : 1;
		
		colorDiffuse = light.color * material.diffuse;
		colorDiffuse = float4(colorDiffuse.rgb * (iDiffuse * shadowFactor), colorDiffuse.a);
		
		colorFinal = colorDiffuse;
	}
	colorFinal += material.emissive;
	colorFinal += globalSetting.ambient;
	colorFinal *= colorTex;
		
	colorFinal = saturate(colorFinal);
	return (colorFinal);
}
//t0
technique11 Lambert
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false)));
	}
}

//t1
technique11 LambertShadow
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
}

//t2
technique11 LambertSkinned
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false)));
	}
}

//t3
technique11 LambertSkinnedShadow
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
}
