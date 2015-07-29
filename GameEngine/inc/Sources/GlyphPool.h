
#pragma once
#include "Singleton.h"

namespace GameEngine
{
	class Font;

	struct Glyph
	{
		FT_Face face;
		wchar_t character;
		int size;

		bool operator < (const Glyph& g) const
		{
			if(character != g.character)
				return character < g.character;
			else if(face->family_name != g.face->family_name)
				return face->family_name < g.face->family_name;
			if(size != g.size)
				return size < g.size;
			return false;
		}
	};

	struct GlyphData
	{
		float u, v, w, h;
		int left, top;
		int bmW, bmH;
		int advX, advY;
		int height;
		int used;
	};

	class Texture2D;

	class GlyphPool : public Singleton<GlyphPool>
	{
	private :
		const int MapWidth = 1024;
		const int MapHeight = 1024;
		const int tileSize = 4;
		const int tileWidth = MapWidth / tileSize;
		const int tileHeight = MapHeight / tileSize;

		FT_Library library = nullptr;
		std::shared_ptr<Texture2D> glyphMap;
		std::vector<wchar_t> tiles;
		std::map<Glyph, GlyphData> glyphs;


	private :
		wchar_t& MapAt(int x, int y) { return tiles[x + (y * tileWidth)]; }
		bool TestSpace(int x, int y, int w, int h);
		void FillSpace(int x, int y, int w, int h, wchar_t c);
		Math::Vector2 FindSpace(int w, int h);
		void GC();

	public:
		void Init();
		void Update();
		GlyphData GetGlyph(const std::shared_ptr<Font>& font, wchar_t c, int size);
		const FT_Library GetLibrary() const { return library; }
		const std::shared_ptr<Texture2D>& GetTexture() const { return glyphMap; }
		GlyphPool();
		~GlyphPool();
	};

}