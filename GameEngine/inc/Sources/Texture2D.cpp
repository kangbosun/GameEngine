#include "enginepch.h"
#include "DebugClass.h"
#include "Texture2D.h"
#include "GraphicDevice.h"


namespace GameEngine
{
	using namespace std;

	void Texture2D::LoadFromFile(const wstring & path, bool compressToDXT)
	{
		Debug::Log(L"#Load : " + path);
		isValid = false;
		auto& device = GraphicDevice::Instance()->device;

		ilInit();
		auto id = ilGenImage();
		ilBindImage(id);
		
		auto result = ilLoadImage(path.c_str());
		if(!result) {
			Debug::Failed("ilLoadImage()");
			return;
		}
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		ILinfo info; 
		iluGetImageInfo(&info);
		ILubyte* pData = info.Data;
		format = R8G8B8A8_UNORM;

		if(compressToDXT) {
			ILuint dxtSize;
			pData = ilCompressDXT(pData, info.Width, info.Height, info.Depth, IL_DXT5, &dxtSize);
			
			format = BC3_UNORM;
		}
			

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.CPUAccessFlags = 0;
		texDesc.Format = (DXGI_FORMAT)format;
		texDesc.Height = height = info.Height;
		texDesc.MipLevels = 1;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc = { 1, 0 };
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Width = width = info.Width;

		D3D11_SUBRESOURCE_DATA data = { pData, info.Width<<2, 0 };

		auto hr = device->CreateTexture2D(&texDesc, &data, &texture2D.p);
		
		if(FAILED(hr)) {
			Debug::Failed("CreateTexture2()");
			return;
		}

		hr = device->CreateShaderResourceView(texture2D, 0, &srv.p);
		if(FAILED(hr)) {
			Debug::Failed("CreateShaderResourceView()");
			return;
		}

		ilDeleteImage(id);
		ilShutDown();

		Debug::Success();
		isValid = true;
	}

	void Texture2D::CreateTexture2D(int width, int height, TextureFormat format, Usage usage, BindFlag bindFlag, CpuAccess cpuflag)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = (UINT)width;
		texDesc.Height = (UINT)height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = (DXGI_FORMAT)format;
		texDesc.SampleDesc.Count = msaa;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = (D3D11_USAGE)usage;
		texDesc.BindFlags = (UINT)bindFlag;
		texDesc.CPUAccessFlags = cpuflag;
		texDesc.MiscFlags = 0;

		auto& device = GraphicDevice::Instance()->device;

		HRESULT hr = device->CreateTexture2D(&texDesc, 0, &texture2D.p);
		if(FAILED(hr)) {
			Debug::Failed("CreateTexture2D(" + ToString() + ")");
			return;
		}

		if(bindFlag & D3D11_BIND_SHADER_RESOURCE) {
			hr = device->CreateShaderResourceView(texture2D, 0, &srv.p);
			if(FAILED(hr)) {
				Debug::Failed("CreateShaderResourceView(" + ToString() + ")");
				return;
			}
		}
		isValid = true;
	}
}