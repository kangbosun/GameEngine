
#include "LightHelper.fx"
#include "structs.fx"

#define MAX_BONE 100

matrix gBones[MAX_BONE];

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


float3 cameraPos;


struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
	float4 viewDir : TEXCOORD1;
	float4 shadowPos : TEXCOORD2;
};

float4x4 shadowTransform;

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(input.position, 1);
	float4 worldPos;
	output.position = worldPos = mul(pos, worldMatrix);
	output.shadowPos = mul(output.position, shadowTransform);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = color;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3)worldMatrix));
	output.tangent = normalize(mul(input.tangent, (float3x3)worldMatrix));
	output.binormal = normalize(mul(input.binormal, (float3x3)worldMatrix));
	
	output.viewDir = normalize(float4(cameraPos, 1) - worldPos);
	
	return output;
}

PixelInput SkinnedVS(SkinnedVertexInput input)
{
	PixelInput output;
	float4 pos = float4(0, 0, 0, 0);
	float3 normal = float3(0, 0, 0);
	float3 tangent = float3(0, 0, 0);
	float3 binormal = float3(0, 0, 0);
	float4 worldPos;
	
	for(int i = 0; i < 4; i++) {
		pos += input.weights[i] * mul(float4(input.position, 1), gBones[input.boneIndices[i]]);
		normal += input.weights[i] * mul(input.normal, (float3x3)gBones[input.boneIndices[i]]);
		tangent += input.weights[i] * mul(float4(input.tangent, 1), gBones[input.boneIndices[i]]).xyz;
		binormal += input.weights[i] * mul(float4(input.binormal, 1), gBones[input.boneIndices[i]]).xyz;
	}
			
	output.position = worldPos = mul(pos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	output.color = color;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(normal, (float3x3)worldMatrix));
	
	// for shadow mapping
	output.shadowPos = mul(pos, shadowTransform);
	
	output.tangent = normalize(mul(float4(tangent, 1), worldMatrix)).xyz;
	output.binormal = normalize(mul(float4(binormal, 1), worldMatrix).xyz);
	
	output.viewDir = normalize(float4(cameraPos, 1) - worldPos);
	
	return output;
}
////////////////////////////////////////////////////////////////////////




SamplerState state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

// ps variables
LightBuffer light;
GlobalSetting globalSetting;
ShaderMaterial material;

Texture2D diffuseMap;
Texture2D shadowMap;

SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ComparisonFunc = LESS_EQUAL;
};


float4 PS(PixelInput input, uniform bool receiveShadow) : SV_TARGET
{
	float4 colorTex = float4(1, 1, 1, 1);
	float iDiffuse = 0;
	float iSpecular = 0;
	float4 colorDiffuse = float4(0, 0, 0, 0);
	float4 colorSpecular = float4(0, 0, 0, 0);
	float4 colorFinal = float4(0, 0, 0, 1);
	float shadowFactor = 1.0f;
	
	colorTex = diffuseMap.Sample(state, input.texcoord);
	clip(colorTex.a - 0.1f);
	
	// shadow
	float3 shadow = (input.shadowPos.xyz / input.shadowPos.w);
	shadowFactor = receiveShadow ? 
		CalcVSMFactor(shadowMap, state, shadow.xy, shadow.z) : 1;
		
	// calc light 
	iDiffuse = CalcDiffuse(light.position, light.color, input.normal.xyz);

	if(iDiffuse > 0.0f) {
		iSpecular = CalcSpecular(light.position, light.color, 
		input.normal.xyz, input.viewDir.xyz, material.shininess);
			
		colorDiffuse = light.color * material.diffuse;
		colorDiffuse = float4(colorDiffuse.rgb * (iDiffuse * shadowFactor), colorDiffuse.a);
		
		colorSpecular = light.color * material.specular;
		colorSpecular = float4(colorSpecular.rgb * (iSpecular * shadowFactor), colorSpecular.a);
		colorFinal = colorDiffuse;
		colorFinal += colorSpecular;
	}
		
	colorFinal += material.emissive;
	colorFinal += globalSetting.ambient;
	colorFinal *= colorTex;
	colorFinal = saturate(colorFinal);

	return (colorFinal);
}

technique11 Phong
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false)));
	}
}

technique11 PhongShadow
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
}

technique11 PhongSkinned
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetPixelShader(CompileShader(ps_5_0, PS(false)));
	}
}

//t3
technique11 PhongSkinnedShadow
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
}
