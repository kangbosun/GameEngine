#include "enginepch.h"
#include "GlyphPool.h"
#include "Texture2D.h"
#include "GraphicDevice.h"
#include "Font.h"
#include "GlobalSetting.h"

namespace GameEngine
{
	void GlyphPool::Init()
	{
		using namespace std;
		FT_Init_FreeType(&library);
		glyphMap = make_shared<Texture2D>();

		glyphMap->CreateTexture2D(MapWidth, MapHeight, R8G8B8A8_UNORM, USAGE_DEFAULT,
								  BIND_SHADER_RESOURCE, CPU_ACCESS_DEFAULT);

		// init texture with(255, 255, 255, 0)
		auto graphicDevice = GraphicDevice::Instance();

		size_t bytelen = (MapWidth * MapHeight) << 2;
		vector<Byte> subResource(bytelen);
		memset(&subResource[0], 255, bytelen);
		for(int i = 3; i < bytelen; i += 4) 
			subResource[i] = 0;
		
		D3D11_BOX box = { 0, 0, 0, (UINT)MapWidth, (UINT)MapHeight, 1 };
		graphicDevice->context->UpdateSubresource(glyphMap->texture2D, 0, &box, &subResource[0], MapWidth << 2, 0);
		tiles.resize(tileWidth * tileHeight);
	}

	GlyphPool::GlyphPool()
	{
		Init();
	}

	bool GlyphPool::TestSpace(int x, int y, int w, int h)
	{
		int ho = x + w;
		int ve = y + h;

		if(ho >= tileWidth) return false;
		if(ve >= tileHeight) return false;

		for(int i = x; i < ho; ++i) {
			for(int j = y; j < ve; ++j) {
				if(MapAt(i, j))
					return false;
			}
		}
		return true;
	}

	void GlyphPool::FillSpace(int x, int y, int w, int h, wchar_t c)
	{
		for(int i = x; i < x + w; ++i) {
			for(int j = y; j < y + h; ++j) {
				MapAt(i, j) = c;
			}
		}
	}

	Vector2 GlyphPool::FindSpace(int w, int h)
	{
		for(int j = 0; j < tileHeight; ++j) {
			for(int i = 0; i < tileWidth; ++i) {
				if(TestSpace(i, j, w, h))
					return Vector2((float)i, (float)j);
			}
		}
		// if no empty space
		GC();

		// refind
		for(int j = 0; j < tileHeight; ++j) {
			for(int i = 0; i < tileWidth; ++i) {
				if(TestSpace(i, j, w, h))
					return Vector2((float)i, (float)j);
			}
		}

		// error
		return Vector2(-1, 0);
	}

	GlyphData GlyphPool::GetGlyph(const std::shared_ptr<Font>& font, wchar_t c, int size)
	{
		using namespace std;
		

		FT_Face _face = font->GetFace();
		Glyph glyph = { _face, c, size };
		GlyphData data = { 0, };

		auto& it = glyphs.find(glyph);
		if(it != glyphs.cend())
			return it->second;

		FT_UInt ix = FT_Get_Char_Index(_face, c);
		if(ix == 0)
			return data;

		FT_Set_Pixel_Sizes(_face, 0, size);
		//FT_Set_Char_Size(_face, 0, size<<6, GlobalSetting::HDPI, GlobalSetting::VDPI);

		if(FT_Load_Glyph(_face, ix, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP))
			return data;

		data.bmW = _face->glyph->bitmap.width;
		data.bmH = _face->glyph->bitmap.rows;

		int tileRectWidth = ((data.bmW + tileSize - 1) / tileSize);
		int tileRectHeight = ((data.bmH + tileSize - 1) / tileSize);

		auto& p = FindSpace(tileRectWidth, tileRectHeight);

		if(p.x == -1)
			return data;

		Vector2 texPos = { p.x * tileSize, p.y * tileSize };
		int a = _face->glyph->metrics.horiAdvance >> 6;
		data.advX = _face->glyph->advance.x >> 6;
		data.advY = _face->glyph->advance.y >> 6;
		data.height = _face->height >> 6;
		data.left = _face->glyph->bitmap_left;
		data.top = _face->glyph->bitmap_top;
		data.u = texPos.x / (float)MapWidth;
		data.v = texPos.y / (float)MapHeight;
		data.w = data.bmW / (float)MapWidth;
		data.h = data.bmH / (float)MapHeight;

		//map
		auto graphicDevice = GraphicDevice::Instance();

		vector<Byte> subResource(data.bmH * data.bmW * 4);
		for(size_t j = 0; j < data.bmH; ++j) {
			for(size_t i = 0; i < data.bmW; ++i) {
				BYTE b = _face->glyph->bitmap.buffer[(j*data.bmW) + i];
				int pos = int(i + (j*data.bmW)) << 2;
				subResource[pos + 0] = 255;
				subResource[pos + 1] = 255;
				subResource[pos + 2] = 255;
				subResource[pos + 3] = b;
			}
		}

		if(subResource.size() > 0) {
			D3D11_BOX box = { (UINT)texPos.x, (UINT)texPos.y, 0, (UINT)(texPos.x + data.bmW), (UINT)(texPos.y + data.bmH), 1 };
			graphicDevice->context->UpdateSubresource(glyphMap->texture2D, 0, &box, &subResource[0], data.bmW << 2, 0);

			FillSpace((int)p.x, (int)p.y, tileRectWidth, tileRectHeight, c);
		}
		data.used = 2;
		glyphs[glyph] = data;
		return data;
	}

	void GlyphPool::Update()
	{
		for(auto& pair : glyphs)
			pair.second.used = min(pair.second.used - 1, 0);
	}

	void GlyphPool::GC()
	{
		auto& pair = glyphs.begin();
		while(pair != glyphs.cend()) {
			auto& data = pair->second;
			if(data.used == 0) {
				FillSpace((int)(data.u * tileWidth), (int)(data.v * tileHeight),
						  (data.bmW + 3) / tileSize, (data.bmH + 3) / tileSize, 0);
				glyphs.erase(pair++);
			}
			else pair++;
		}
	}

	GlyphPool::~GlyphPool()
	{
		if(library)
			FT_Done_FreeType(library);
	}
}