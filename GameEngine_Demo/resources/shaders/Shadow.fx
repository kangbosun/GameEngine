
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
	float4 color : COLOR;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(input.position, 1);
	output.position = mul(pos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	output.color = color;
	output.texcoord = input.texcoord;
	return output;
}

// ps

Texture2D diffuseMap;
SamplerState state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

void PS(PixelInput input) 
{
	float4 diffuse = diffuseMap.Sample(state, input.texcoord);
	clip(diffuse.a - 0.15f);
}



technique11 ShadowMap
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}