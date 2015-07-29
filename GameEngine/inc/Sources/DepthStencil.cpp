
#include "enginepch.h"
#include "Texture2D.h"
#include "GraphicDevice.h"

namespace GameEngine
{
	void DepthStencil::Initialize(int width, int height)
	{
		this->format = D24_UNORM_S8_UINT;
		this->width = width;
		this->height = height;
		isValid = false;
		
		depthStencilView.Release();
		srv.Release();

		CreateTexture2D(width, height, format, USAGE_DEFAULT, BIND_DEPTH_STENCIL);

		auto& graphic = GraphicDevice::Instance();

		auto hr = graphic->device->CreateDepthStencilView(texture2D, 0, &depthStencilView.p);
		if(FAILED(hr)) {
			isValid = false;
			return;
		}
		isValid = true;
	}

}