
#ifndef _CAMERA_FXH_
#define _CAMERA_FXH_

struct Camera
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float3 position;
};

Camera camera;

#endif 