
matrix viewMatrix;
matrix projectionMatrix;

matrix worldMatrix;
float4 color;


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

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;	
};

PixelInput VS(VertexInput input)
{
	PixelInput output;

	output.position = mul(float4(input.position, 1), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = color;
	output.tex = input.texcoord;

	return output;
}

//////////////////////////////////////////////////////////
Texture2D diffuseMap;
SamplerState state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


float4 PS(PixelInput input) : SV_TARGET
{
	float4 texColor = diffuseMap.Sample(state, input.tex);
	texColor *= input.color;
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