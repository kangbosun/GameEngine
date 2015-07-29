
#ifndef _VERTEXINPUT_H
#define _VERTEXINPUT_H

struct VertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	uint4   boneIndices : BLENDINDICES;
	float4 weights : BLENDWEIGHTS;
};

#endif 