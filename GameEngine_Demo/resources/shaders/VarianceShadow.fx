

matrix viewMatrix;
matrix projectionMatrix;

matrix worldMatrix;

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
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
};

struct RenderOption
{
	bool useAlbedoMap;
	bool useSpecularMap;
	bool useNormalMap;
	bool useShadowMap;
	bool useSkinning;
};

matrix gBones[MAX_BONE];
RenderOption renderOption;


PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(0, 0, 0, 0);
	
	if(renderOption.useSkinning) {
		[unroll]
		for(int i = 0; i < 4; i++) {
			pos += input.weights[i] * mul(float4(input.position, 1), gBones[input.boneIndices[i]]);
		}
	}
	else {
		pos = float4(input.position, 1);
	}
	
	output.position = mul(pos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.texcoord = input.texcoord;
	return output;
}

Texture2D diffuseMap;
SamplerState state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

float2 PS(PixelInput input) : SV_Target
{
	float4 color = diffuseMap.Sample(state, input.texcoord);
	float depth = input.position.z;
	
	float2 moment = ComputeMoments(depth);
	return float2(moment.x, moment.y);
}

technique11 ShadowMap
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}




