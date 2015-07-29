

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