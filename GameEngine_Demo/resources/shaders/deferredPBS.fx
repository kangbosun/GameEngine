
// deferred shader
// geomeytry pass

#define MAX_BONE 100

// structs
struct VertexInput
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
	float4 color;
	float metalic;
	float roughness;
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

struct RenderOption
{
	bool useAlbedoMap;
	bool useSpecularMap;
	bool useNormalMap;
	bool useShadowMap;
	bool useSkinning;
};

// VertexShader variables
float4x4 worldMatrix;
float4x4 viewMatrix;
float4x4 projectionMatrix;

PixelInput VS(VertexInput)
{
	
}


	