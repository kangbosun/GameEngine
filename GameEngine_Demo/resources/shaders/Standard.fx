
#include "Lighting.h"
#include "DiffuseMapping.h"
#include "NormalMapping.h"
#include "ShadowMapping.h"
#include "Skinning.h"
#include "Camera.h"

// structs
struct VertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	uint4   boneIndices : BLENDINDICES;
	float4 weights : BLENDWEIGHTS;
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

struct GlobalSetting
{
	float4 ambient;
	float shadowMapSize;
};



// VertexShader variables
float4x4 worldMatrix;


PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4x4 skinnedMatrix = skinning.GetMatrix(input.boneIndices, input.weights);
	float4 pos = mul(float4(input.position,1), skinnedMatrix);
	float3 normal = mul(float4(input.normal,1), skinnedMatrix).xyz;
	float3 tangent = mul(float4(input.tangent,1), skinnedMatrix).xyz;
	
	float4 worldPos = mul(pos, worldMatrix);
	float4 worldViewPos = mul(worldPos, transpose(camera.viewMatrix));
	float4 screenPos = mul(worldViewPos, transpose(camera.projectionMatrix));

	// for normalMap;
	tangent = normalize(mul(tangent, (float3x3)worldMatrix));
	normal = normalize(mul(normal, (float3x3)worldMatrix));
	float3 binormal = normalize(cross(tangent, normal));

	output.position = screenPos;
	output.texcoord = input.texcoord;
	output.normal = normal;
	output.shadowPos = shadowMapping.GetVSShadowPos(worldPos);
	output.tangent = tangent;
	output.binormal = binormal;
	output.worldPos = worldPos;
	return output;
}


Texture2D specularMap;
GlobalSetting globalSetting;

float4 PS(PixelInput input) : SV_TARGET
{
	float4 colorFinal = float4(0, 0, 0, 1);

	float4 diffuseColor = diffuseMapping.GetDiffuseColor(input.texcoord);

	float3 normal = normalMapping.GetNormal(input.texcoord, input.tangent, input.binormal, input.normal);
	// calc light 
	float3 N = normalize(normal);
	float3 L = -normalize(light.dir);
	float3 V = normalize(camera.position - input.worldPos.xyz);
	float3 H = normalize(L + V);

	float spec = lighting.GetSpecular(N, V, L);
	float diffuse = lighting.GetDiffuse(N, L);
	float lightIntensity = saturate(spec + diffuse);

	float shadowFactor = shadowMapping.GetShadowFactor(input.shadowPos);

	float4 tempColor = float4(lighting.GetLightColor() * shadowFactor * lightIntensity, 1);

	colorFinal += tempColor;
	colorFinal *= diffuseColor * material.color;
	colorFinal = saturate(colorFinal);
	
	return colorFinal;
}



////////////////////////////////////////////////////////////
//deferred

//struct GBufferOut
//{
//	half2 normal : SV_Target0;
//	float4 diffuseMetallic : SV_Target1;
//	float4 specularRoughness : SV_Target2;
//	float4 position : SV_Target3; 
//};
//
//half2 encodeNormal(float3 n)
//{
//	half2 enc = half2(normalize(n.xy) * sqrt(-n.z*0.5+0.5));
//	enc = enc * 0.5 + 0.5;
//	return enc;
//}
//
//half3 decodeNormal(half2 enc)
//{
//	half4 nn = (2 * half4(enc, 0, 0)) + half4(-1, -1, 1, -1);
//	half l = dot(nn.xyz, -nn.xyw);
//	nn.z = l;
//	nn.xy *= (half)sqrt(l);
//	return nn.xyz * 2 + half3(0, 0, -1);
//}


technique11 Standard
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}






