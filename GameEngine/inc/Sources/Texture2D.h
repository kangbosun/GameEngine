#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	enum TextureFormat
	{
		R8G8B8A8_UNORM = DXGI_FORMAT_R8G8B8A8_UNORM,
		R32G32B32A32_FLOAT = DXGI_FORMAT_R32G32B32A32_FLOAT,
		D24_UNORM_S8_UINT = DXGI_FORMAT_D24_UNORM_S8_UINT,
		R32G32_FLOAT = DXGI_FORMAT_R32G32_FLOAT,

		//DXT
		BC3_UNORM = DXGI_FORMAT_BC3_UNORM,
		BC4_UNORM = DXGI_FORMAT_BC4_UNORM,
		BC5_UNORM = DXGI_FORMAT_BC5_UNORM,
		BC7_UNORM = DXGI_FORMAT_BC7_UNORM
	};

	enum Usage
	{
		USAGE_DEFAULT = D3D11_USAGE_DEFAULT,
		USAGE_DYNAMIC = D3D11_USAGE_DYNAMIC,
		USAGE_STAGING = D3D11_USAGE_STAGING,
		USAGE_IMMUTABLE = D3D11_USAGE_IMMUTABLE,
	};

	enum BindFlag
	{
		BIND_SHADER_RESOURCE = D3D11_BIND_SHADER_RESOURCE,
		BIND_DEPTH_STENCIL = D3D11_BIND_DEPTH_STENCIL,
		BIND_CONSTANT_BUFFER = D3D11_BIND_CONSTANT_BUFFER,
		BIND_INDEX_BUFFER = D3D11_BIND_INDEX_BUFFER,
		BIND_VERTEX_BUFFER = D3D11_BIND_VERTEX_BUFFER,
		BIND_RENDER_TARGET = D3D11_BIND_RENDER_TARGET,
		BIND_STREAM_OUTPUT = D3D11_BIND_STREAM_OUTPUT,
	};

	enum CpuAccess
	{
		CPU_ACCESS_DEFAULT = 0,
		CPU_ACCESS_READ = D3D11_CPU_ACCESS_READ,
		CPU_ACCESS_WRITE = D3D11_CPU_ACCESS_WRITE,
	};

	class GAMEENGINE_API Texture2D : public Object
	{
	protected:
		bool isValid = false;
	public:
		std::string name;
		int width = 0;
		int height = 0;
		TextureFormat format;

		int msaa = 1;
		CComPtr<ID3D11Texture2D> texture2D;
		CComPtr<ID3D11ShaderResourceView> srv;
		bool IsValid() const { return isValid; }
		void LoadFromFile(const std::wstring& path, bool compressToDXT = false);
		void CreateTexture2D(int width, int height, TextureFormat format, Usage usage = USAGE_DEFAULT, BindFlag flag = BIND_SHADER_RESOURCE, CpuAccess cpuflag = (CpuAccess)0);
	};

	/////////////////////////////////////////////////

	class GAMEENGINE_API RenderTarget : public Texture2D
	{
	public:
		bool IsValid() { return isValid; }
		CComPtr<ID3D11RenderTargetView> renderTargetView;
		void Initialize(int width, int height, TextureFormat format);
		void Initialize(int width, int height, TextureFormat format, const CComPtr<ID3D11Texture2D>& buffer);
	};

	//////////////////////////////////////////

	class GAMEENGINE_API DepthStencil : public Texture2D
	{
	public:
		bool IsValid() { return isValid; }
		CComPtr<ID3D11DepthStencilView> depthStencilView;
		void Initialize(int width, int height);
	};
}

#pragma warning(pop)