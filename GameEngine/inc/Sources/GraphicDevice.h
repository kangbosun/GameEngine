#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	struct Viewport
		: public D3D11_VIEWPORT
	{
		Viewport() :
			D3D11_VIEWPORT()
		{
		};

		inline Viewport(int width, int height, int maxDepth = 1, int minDepth = 0, int topLeftX = 0, int topLeftY = 0)
		{
			Width = (float)width; Height = (float)height; MaxDepth = (float)maxDepth; MinDepth = (float)minDepth;
			TopLeftX = (float)topLeftX; TopLeftY = (float)topLeftY;
		}
	};

	class RenderTarget;
	class DepthStencil;

	////////////////////////////////////////////////////

	class GAMEENGINE_API GraphicDevice : public std::enable_shared_from_this<GraphicDevice>
	{
	private:
		static std::shared_ptr<GraphicDevice> instance;
		DXGI_SAMPLE_DESC sampleDesc;
		D3D11_FILL_MODE fillMode;

		D3D11_RASTERIZER_DESC rasterizerDesc;

		std::shared_ptr<RenderTarget> renderTarget;
		std::shared_ptr<DepthStencil> depthStencil;
		std::shared_ptr<RenderTarget> currRenderTarget;
		std::shared_ptr<DepthStencil> currDepthStencil;

		CComPtr<ID3D11DepthStencilState> depthStencilState;
		CComPtr<ID3D11DepthStencilState> disableDepthStencilState;

		CComPtr<ID3D11RasterizerState> solidRasterState;
		CComPtr<ID3D11RasterizerState> wireRasterState;
		CComPtr<ID3D11RasterizerState> rasterizeState;

		CComPtr<ID3D11BlendState> blendState;
		CComPtr<ID3D11BlendState> disableBlendState;

		int videoMemory;
		std::wstring videoCardDesc;

		D3D11_VIEWPORT viewPort;

	public:
		UINT vSync;
		CComPtr<ID3D11Device> device;
		CComPtr<ID3D11DeviceContext> context;
		CComPtr<IDXGISwapChain> swapChain;

	public:
		static std::shared_ptr<GraphicDevice>& Instance() { return instance; }
		void Initialize(int width, int height, HWND hWnd);
		//void Release();

		void Resize(int width, int height);

		void SetDepthEnable(bool b);
		void SetFillMode(D3D11_FILL_MODE mode);

		void ClearRenderTarget(const Math::Color& color);
		void ClearDepthStencil();

		void SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget);
		void SetRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget, const std::shared_ptr<DepthStencil>& depthStencil);
		void SetBackBufferRenderTarget();
		void SetViewport(const Viewport& viewport);

		const CComPtr<ID3D11DepthStencilView>& GetDepthStencilView();
		void SwapBuffers();
		void SetCullMode(D3D11_CULL_MODE mode);

		int VMemory();
		std::wstring VideoCardName();

		void SetAlphaBlend(bool b);
		Math::Vector2 GetViewportSize() { return Math::Vector2(viewPort.Width, viewPort.Height); }

		int MSAA() { return sampleDesc.Count; }
	};
}
#pragma warning(pop)