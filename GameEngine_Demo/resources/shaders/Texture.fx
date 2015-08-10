

#include "VertexInput.fxh"
#include "DiffuseMapping.fxh"
#include "Material.fxh"
#include "Camera.fxh"

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

matrix worldMatrix;

PixelInput VS(VertexInput input)
{
	PixelInput output;

	output.position = mul(float4(input.position, 1), worldMatrix);
	output.position = mul(output.position, transpose(camera.viewMatrix));
	output.position = mul(output.position, transpose(camera.projectionMatrix));

	output.texcoord = input.texcoord;

	return output;
}

//////////////////////////////////////////////////////////


float4 PS(PixelInput input) : SV_TARGET
{
	float4 texColor = diffuseMapping.GetDiffuseColor(input.texcoord);
	texColor *= material.color;
	texColor = saturate(texColor);;
	return texColor;
}


technique11 tech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}