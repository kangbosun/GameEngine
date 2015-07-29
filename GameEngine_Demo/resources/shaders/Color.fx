
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};

cbuffer ViewProj
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer World 
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
	return output;
}



float4 PS(PixelInput input) : SV_TARGET
{
    return input.color;
}


technique11 tech
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}