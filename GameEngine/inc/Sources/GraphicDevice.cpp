#include "enginepch.h"
#include "GraphicDevice.h"
#include "DebugClass.h"
#include "GlobalSetting.h"
#include "Texture2D.h"
#include "Mesh.h"

namespace GameEngine
{
	using namespace std;

	std::shared_ptr<GraphicDevice> GraphicDevice::instance(nullptr);

	void GraphicDevice::Initialize(int width, int height, HWND hWnd)
	{
		instance = shared_from_this();
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;
		fillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result;

		IDXGIFactory* factory;
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if(FAILED(result)) Debug::Log("failed to create DXGIFactory");

		IDXGIAdapter* adapter;
		result = factory->EnumAdapters(0, &adapter);
		if(FAILED(result)) Debug::Log("failed to factory->EnumAdapters");

		IDXGIOutput* adapterOut;
		result = adapter->EnumOutputs(0, &adapterOut);
		if(FAILED(result)) Debug::Log("failed to adapter->EnumOutputs");

		unsigned int numModes;
		result = adapterOut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		result = adapterOut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if(FAILED(result)) Debug::Log("failed to adapterOut->GetDisplayModeList");

		int numerator = displayModeList[numModes - 1].RefreshRate.Numerator;
		int denominator = displayModeList[numModes - 1].RefreshRate.Denominator;

		int maxWidth = displayModeList[numModes - 1].Width;
		int maxHeight = displayModeList[numModes - 1].Height;

		width = maxWidth;
		height = maxHeight;

		DXGI_ADAPTER_DESC adapterDesc;
		result = adapter->GetDesc(&adapterDesc);
		if(FAILED(result)) Debug::Log("failed to adapter->GetDesc");
		videoMemory = (int)(adapterDesc.DedicatedVideoMemory >> 20);

		videoCardDesc = adapterDesc.Description;

		videoCardDesc += (L"(" + to_wstring(videoMemory) + L"MB)");
		Debug::Log(videoCardDesc);

		delete[] displayModeList;
		adapterOut->Release();
		adapter->Release();
		factory->Release();

		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferCount = 1;
		scd.BufferDesc.Width = width;
		scd.BufferDesc.Height = height;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator = numerator;
		scd.BufferDesc.RefreshRate.Denominator = denominator;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.OutputWindow = hWnd;
		scd.SampleDesc.Quality = sampleDesc.Quality;
		scd.SampleDesc.Count = sampleDesc.Count;
		scd.Windowed = true;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
		scd.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0
		};
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, 0, 0, featureLevels,
											   ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &scd, &swapChain, &device, NULL, &context);
		if(FAILED(result))
			Debug::Log("failed to create Device and Swapchain");

		renderTarget = std::make_shared<RenderTarget>();
		depthStencil = std::make_shared<DepthStencil>();

		Resize(width, height);

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

		depthStencilDesc.DepthEnable = false;
		device->CreateDepthStencilState(&depthStencilDesc, &disableDepthStencilState);

		context->OMSetDepthStencilState(depthStencilState, 1);

		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;

		result = device->CreateRasterizerState(&rasterizerDesc, &rasterizeState.p);
		if(FAILED(result)) Debug::Log("failed to create RasterizerState");

		context->RSSetState(rasterizeState);

		D3D11_BLEND_DESC bdesc = { 0 };
		bdesc.RenderTarget[0].BlendEnable = true;
		bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = device->CreateBlendState(&bdesc, &blendState);
		if(FAILED(result)) Debug::Log("failed to create BlendState");

		bdesc.RenderTarget[0].BlendEnable = false;
		result = device->CreateBlendState(&bdesc, &disableBlendState);
		if(FAILED(result)) Debug::Log("failed to create BlendState");

		currDepthStencil = depthStencil;
		currRenderTarget = renderTarget;

		SetAlphaBlend(true);
		vSync = true;
	}

	void GraphicDevice::SetCullMode(D3D11_CULL_MODE mode)
	{
		rasterizerDesc.CullMode = mode;
		rasterizeState.Release();
		device->CreateRasterizerState(&rasterizerDesc, &rasterizeState.p);
		context->RSSetState(rasterizeState);
	}

	void GraphicDevice::SetFillMode(D3D11_FILL_MODE mode)
	{
		rasterizerDesc.FillMode = mode;
		rasterizeState.Release();
		device->CreateRasterizerState(&rasterizerDesc, &rasterizeState.p);
		context->RSSetState(rasterizeState);
	}

	void GraphicDevice::SetDepthEnable(bool b)
	{
		if(b)
			context->OMSetDepthStencilState(depthStencilState, 1);
		else
			context->OMSetDepthStencilState(disableDepthStencilState, 1);
	}

	void GraphicDevice::Resize(int width, int height)
	{
		TextureFormat format = R8G8B8A8_UNORM;
		swapChain->ResizeBuffers(1, width, height, (DXGI_FORMAT)format, 0);
		CComPtr<ID3D11Texture2D> backBufferPtr;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr.p);

		renderTarget->msaa = sampleDesc.Count;
		renderTarget->Initialize(width, height, format, backBufferPtr);
		depthStencil->msaa = sampleDesc.Count;
		depthStencil->Initialize(width, height);

		context->OMSetRenderTargets(1, &renderTarget->renderTargetView.p, depthStencil->depthStencilView);

		viewPort.Width = (float)width;
		viewPort.Height = (float)height;
		viewPort.MaxDepth = 1.0f;
		viewPort.MinDepth = 0.0f;
		viewPort.TopLeftX = 0.0f;
		viewPort.TopLeftY = 0.0f;

		context->RSSetViewports(1, &viewPort);

		GlobalSetting::resolutionX = width;
		GlobalSetting::resolutionY = height;
		GlobalSetting::aspectRatio = width / float(height);
		GlobalSetting::UIScaleFactorX = width / (float)GlobalSetting::maxUIWidth;
		GlobalSetting::UIScaleFactorY = height / (float)GlobalSetting::maxUIHeight;
	}

	inline const CComPtr<ID3D11DepthStencilView>& GraphicDevice::GetDepthStencilView() { return depthStencil->depthStencilView; }

	void GraphicDevice::SwapBuffers()
	{
		swapChain->Present(vSync, 0);
	}

	void GraphicDevice::ClearRenderTarget(const Color& color)
	{
		context->ClearRenderTargetView(currRenderTarget->renderTargetView, (float*)&color);
	}

	void GraphicDevice::ClearDepthStencil()
	{
		context->ClearDepthStencilView(currDepthStencil->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void GraphicDevice::SetViewport(const Viewport& viewport)
	{
		context->RSSetViewports(1, &viewport);
	}

	int GraphicDevice::VMemory()
	{
		return videoMemory;
	}

	wstring GraphicDevice::VideoCardName()
	{
		return videoCardDesc;
	}

	void GraphicDevice::SetAlphaBlend(bool b)
	{
		float blendFactor[4] = { 0.0f };
		if(b)
			context->OMSetBlendState(blendState.p, blendFactor, 0xffffffff);
		else
			context->OMSetBlendState(disableBlendState.p, blendFactor, 0xffffffff);
	}

	void GraphicDevice::SetMeshBuffer(const std::shared_ptr<Mesh>& mesh)
	{
		if(context && mesh && mesh->IsValid()) {
			auto& vertexBuffer = mesh->vertexBuffer;
			auto& indexBuffer = mesh->indexBuffer;
			context->IASetVertexBuffers(0, 1, (&vertexBuffer.p), &mesh->stride, &mesh->offset);
			context->IASetIndexBuffer(indexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
			context->IASetPrimitiveTopology(mesh->primitiveType);
		}
	}

	void GraphicDevice::SetBackBufferRenderTarget()
	{
		context->OMSetRenderTargets(1, &renderTarget->renderTargetView.p, depthStencil->depthStencilView);
		context->RSSetViewports(1, &viewPort);
		currDepthStencil = depthStencil;
		currRenderTarget = renderTarget;
	}

	void GraphicDevice::SetRenderTarget(const std::shared_ptr<RenderTarget>& _renderTarget)
	{
		currRenderTarget = _renderTarget;
		context->OMSetRenderTargets(1, &_renderTarget->renderTargetView.p, 0);
	}

	void GraphicDevice::SetRenderTarget(const std::shared_ptr<RenderTarget>& _renderTarget, const std::shared_ptr<DepthStencil>& _depthStencil)
	{
		currRenderTarget = _renderTarget;
		currDepthStencil = _depthStencil;
		context->OMSetRenderTargets(1, &_renderTarget->renderTargetView.p, _depthStencil->depthStencilView);
	}
}