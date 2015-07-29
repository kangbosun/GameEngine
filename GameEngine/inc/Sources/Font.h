
#pragma once

#include "Singleton.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{

	//for FreeType Library (Glyph)
#ifndef FT_FREETYPE_H
	struct FT_LibraryRec_;
	typedef FT_LibraryRec_ *FT_Library;

	struct FT_FaceRec_;
	typedef FT_FaceRec_ *FT_Face;
#endif

	class GAMEENGINE_API Font : public Object
	{
	private:
		FT_Face face = nullptr;
		Font() {}
	public:
		std::string fontName;

	public:
		static std::shared_ptr<Font> LoadFont(const std::string& fontname, const std::string& path);
		const FT_Face GetFace() { return face; }
		~Font();
	};

	// for BMFont
	//	struct GAMEENGINE_API CharInfo
	//	{
	//		int page;
	//		float x;
	//		float y;
	//		float width;
	//		float height;
	//	};
	//
	//	class GAMEENGINE_API Font
	//	{
	//	private:
	//		std::string fontname;
	//		std::vector<std::string> pages;
	//		std::unordered_map<int, CharInfo> chars;
	//		
	//		int offset;
	//
	//		int nChars;
	//
	//		int scaleW;
	//		int scaleH;
	//
	//		static void parse(std::vector<std::string>& tokens, const std::shared_ptr<Font>& font);
	//
	//	public:
	//		std::vector<std::shared_ptr<Texture2D>> Texture2Ds;
	//		Font();
	//		const CharInfo& GetCharInfo(int code);
	//
	//		static std::shared_ptr<Font> LoadBMFont(LPCWSTR descfile, const CComPtr<ID3D11Device>& device);
	//		int GetCountOfPages() { return (int)pages.size(); }
	//		std::string GetPageNameByIndex(int index) { if(index < pages.size()) return pages[index]; return "Invalid index"; }
	//	};
}

#pragma warning(pop)