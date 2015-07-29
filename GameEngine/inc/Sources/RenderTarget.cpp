
#include "enginepch.h"
#include "Texture2D.h"
#include "GraphicDevice.h"
#include "DXUtil.h"

namespace GameEngine
{
	void RenderTarget::Initialize(int width, int height, TextureFormat format)
	{
		this->format = format;

		this->width = width;
		this->height = height;
		isValid = false;
		srv.Release();
		renderTargetView.Release();

		auto& device = GraphicDevice::Instance()->device;

		CreateTexture2D(width, height, format, USAGE_DEFAULT, BindFlag(BIND_RENDER_TARGET | BIND_SHADER_RESOURCE));

		HRESULT hr = device->CreateRenderTargetView(texture2D, 0, &renderTargetView.p);
		if(FAILED(hr)) {
			Debug(ToString() + " - failed to create RTV");
			isValid = false;
			return;
		}
		isValid = true;
	}

	void RenderTarget::Initialize(int width, int height, TextureFormat format, const CComPtr<ID3D11Texture2D>& buffer)
	{
		this->format = format;		
		this->width = width;
		this->height = height;
		isValid = false;
		srv.Release();
		renderTargetView.Release();

		auto& device = GraphicDevice::Instance()->device;

		HRESULT hr = device->CreateRenderTargetView(buffer, 0, &renderTargetView.p);
		if(FAILED(hr)) {
			Debug(ToString() + " - failed to create RTV");
			isValid = false;
			return;
		}
		isValid = true;
	}
}