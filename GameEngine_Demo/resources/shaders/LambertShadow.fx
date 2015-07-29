
#include "LightHelper.fx"

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

float4x4 shadowTransform;

struct VertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float4 shadowPos : TEXCOORD1;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(input.position, 1);

	output.position = mul(pos, worldMatrix);
	output.shadowPos = mul(output.position, shadowTransform);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = color;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
	
	return output;
}


struct LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float4 position;
};

Texture2D diffuseMap;
Texture2D shadowMap;

LightBuffer lightBuffer;

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


float4 PS(PixelInput input, uniform bool useTexture) : SV_TARGET
{
	float4 colorTex;
	float iDiffuse = 0;
	float4 colorDiffuse;
	float4 colorFinal = float4(0, 0, 0, 1);

	colorTex = diffuseMap.Sample(state, input.texcoord);
	clip(colorTex.a - 0.1f);
	
	colorFinal = lightBuffer.ambientColor;

	float shadowFactor = CalcShadowFactor(samShadow, shadowMap, input.shadowPos);
	
	if(lightBuffer.position.w == 0.0f) { // directional light
		float3 dir = normalize(lightBuffer.position.xyz);
			iDiffuse = saturate(dot(input.normal, dir));
	}
	colorFinal += (lightBuffer.diffuseColor*iDiffuse * shadowFactor);

	colorFinal *= colorTex;

	colorFinal = saturate(colorFinal);
	return (colorFinal);
}

technique11 diffuse
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
}
