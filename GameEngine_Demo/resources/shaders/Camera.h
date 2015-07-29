
#ifndef _CAMERA_H_
#define _CAMERA_H_

struct Camera
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float3 position;
};

Camera camera;

#endif 