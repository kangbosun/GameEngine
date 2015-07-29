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

TextureCube colorTexture;

SamplerState state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

struct VertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float3 texcoord : POSITION;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(input.position, 1);

	output.position = mul(pos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.texcoord = normalize(input.position);

	return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
	return colorTexture.Sample(state, input.texcoord);
}


RasterizerState noCull
{
	CullMode = None;
};

DepthStencilState LessEqual
{
	DepthFunc = LESS_EQUAL;
};



technique11 diffuse
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));

		SetRasterizerState(noCull);
		SetDepthStencilState(LessEqual, 0);
	}
}