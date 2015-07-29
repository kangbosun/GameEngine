#include "enginepch.h"
#include "Font.h"
#include "Resource.h"
#include "GraphicDevice.h"
#include "GlyphPool.h"
#include "DXUtil.h"

namespace GameEngine
{
	using namespace std;

	std::shared_ptr<Font> Font::LoadFont(const string& fontname, const string& path)
	{
		auto& font = shared_ptr<Font>(new Font());
		if(font->face)
			FT_Done_Face(font->face);
		auto lib = GlyphPool::GetInstance()->GetLibrary();
		if(FT_New_Face(lib, path.c_str(), 0, &font->face)) {
			Debug("Failed - FT_New_Face(" + path + ")");
			return font;
		}
		font->fontName = fontname;
		return font;
	}

	Font::~Font()
	{
		if(face)
			FT_Done_Face(face);
	}

	//Font::Font()
	//{
	//	fontname = "";
	//	int offset = 0;
	//}

	//const CharInfo& Font::GetCharInfo(int code)
	//{
	//	return chars[code];
	//}

	//void Font::parse(vector<string>& tokens, const shared_ptr<Font>& font)
	//{
	//	string tag[] = { "char", "id", "chars", "count", "page", "file", "info", "face" };
	//	for(int i = 0; i < (int)tokens.size(); i++) {
	//		string token = tokens[i];
	//		//chracters
	//		if(token == tag[0]) {
	//			if(tokens[++i] == tag[1]) {
	//				int id = stoi(tokens[++i]);
	//				font->chars[id].x = stoi(tokens[++++i]) / (float)font->scaleW;
	//				font->chars[id].y = stoi(tokens[++++i]) / (float)font->scaleH;;
	//				font->chars[id].width = stoi(tokens[++++i]) / (float)font->scaleW;
	//				font->chars[id].height = stoi(tokens[++++i]) / (float)font->scaleH;
	//				while(tokens[++i] != tag[4]);

	//				font->chars[id].page = stoi(tokens[++i]);
	//			}
	//		}

	//		else if(token == "scaleW") {
	//			font->scaleW = stoi(tokens[++i]);
	//		}
	//		else if(token == "scaleH") {
	//			font->scaleH = stoi(tokens[++i]);
	//		}

	//		// char count
	//		else if(token == tag[2]) {
	//			if(tokens[++i] == tag[3]) {
	//				int nChar = stoi(tokens[++i]);
	//				font->nChars = nChar;
	//			}
	//		}
	//		// page id part
	//		else if(token == tag[4]) {
	//			if(tokens[++i] == tag[1]) {
	//				if(tokens[++++i] == tag[5]) {
	//					font->pages.push_back(tokens[++i]);
	//				}
	//			}
	//		}
	//		// font name part
	//		else if(token == tag[6]) {
	//			if(tokens[++i] == tag[7]) {
	//				font->fontname = tokens[++i];
	//			}
	//		}
	//	}
	//}

	//std::shared_ptr<Font> Font::LoadBMFont(LPCWSTR descfile, const CComPtr<ID3D11Device>& device)
	//{
	//	std::shared_ptr<Font> font = make_shared<Font>();
	//	ifstream in(descfile);
	//	stringstream stream;
	//	stream << in.rdbuf();
	//	in.close();

	//	vector<string> tokens;
	//	Tokenize(tokens, stream.str(), "= \n");

	//	stream.clear();
	//	stream.str("");

	//	parse(tokens, font);

	//	tokens.clear();

	//	return font;
	//}
}