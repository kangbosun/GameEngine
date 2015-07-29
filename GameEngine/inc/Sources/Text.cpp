#include "enginepch.h"
#include "UI.h"
#include "Renderer.h"
#include "Resource.h"
#include "Renderer.h"
#include "Font.h"
#include "Mesh.h"
#include "Shader.h"
#include "DXUtil.h"
#include "GameObject.h"
#include "GraphicDevice.h"
#include "GlyphPool.h"

namespace GameEngine
{
	using namespace std;
	using namespace Math;

	void Text::Update()
	{
		SetMesh();
		//SetPivot();
	}

	void Text::SetFont(const std::shared_ptr<Font>& _font)
	{
		font = _font;
	}

	void Text::SetText(const wstring& text)
	{
		if(this->text != text) {
			this->text = text;
			textChanged = true;
		}
	}

	void Text::SetFontSize(int size)
	{
		if(fontsize != size) {
			fontsize = max(size, 10);
			textChanged = true;
		}
	}

	void Text::SetMesh()
	{
		if(!textChanged)
			return;
		auto& graphicDevice = GraphicDevice::Instance();
		auto& device = graphicDevice->device;
		if(device == nullptr) {
			Debug("GraphicDevice is invalid");
			return;
		}

		if(font == nullptr)
			return;

		vector<TextLine> lines;
		map<wchar_t, GlyphData> datas;
		wstring str = text;
		unsigned long long pos = 0;
		while(pos != str.npos) {
			pos = str.find('\n');
			wstring token = str.substr(0, pos);
			str = str.substr(pos + 1);
			TextLine line = { token, (int)token.size() };
			lines.emplace_back(std::move(line));
		}

		int width = (int)transform()->width;
		int height = (int)transform()->height;

		int heightPerLine = fontsize;
		int maxWidth = width;
		int maxHeight = heightPerLine * (int)lines.size();
		int totalChars = 0;
		//prepare
		auto& gp = GlyphPool::GetInstance();

		for(int n = 0; n < lines.size(); ++n) {
			wstring& line = lines[n].str;
			int tempWidth = 0;
			int len = (int)line.size();
			totalChars += len;
			for(int i = 0; i < len; ++i) {
				int code = line[i];
				auto& data = gp->GetGlyph(font, code, fontsize);
				tempWidth += data.advX;
				datas[code] = data;
			}
			lines[n].width = tempWidth;
		}

		vector<Vertex> vertices(totalChars * 4);
		vector<unsigned long> indices(totalChars * 6);

		int index = 0;
		int index2 = 0;

		float yoffset = 0;
		if(align & eBottom)
			yoffset = (-height * 0.5f) + maxHeight;
		else if(align & eTop)
			yoffset = height * 0.5f;
		else
			yoffset = maxHeight * 0.5f;

		for(int n = 0; n < lines.size(); ++n) {
			wstring line = lines[n].str;
			float xoffset = 0;
			if(align & eLeft)
				xoffset = -width * 0.5f;
			else if(align & eRight)
				xoffset = (width * 0.5f) - lines[n].width;
			else
				xoffset = -lines[n].width * 0.5f;

			for(int i = 0; i < line.size(); ++i) {
				auto& data = datas[line[i]];
				float left = xoffset + data.left;
				float right = (left + data.bmW);
				float top = yoffset - (fontsize - data.top);
				float bottom = top - data.bmH;

				vertices[index].pos = Vector3(left, top, 0.0f);
				vertices[index++].tex = Vector2(data.u, data.v);

				vertices[index].pos = Vector3(right, top, 0.0f);
				vertices[index++].tex = Vector2(data.u + data.w, data.v);

				vertices[index].pos = Vector3(right, bottom, 0.0f);
				vertices[index++].tex = Vector2(data.u + data.w, data.v + data.h);

				vertices[index].pos = Vector3(left, bottom, 0.0f);
				vertices[index++].tex = Vector2(data.u, data.v + data.h);

				indices[index2++] = index - 4;
				indices[index2++] = index - 3;
				indices[index2++] = index - 2;

				indices[index2++] = index - 2;
				indices[index2++] = index - 1;
				indices[index2++] = index - 4;

				xoffset += data.advX;
			}
			yoffset -= heightPerLine;
		}
		auto mesh = make_shared<Mesh>();
		mesh->Initialize(&vertices[0], &indices[0], totalChars * 4, totalChars * 6, 0, 0, device);
		renderer()->mesh = mesh;
		renderer()->material.diffuseMap = GlyphPool::GetInstance()->GetTexture();
		textChanged = false;
	}

	std::shared_ptr<Text> Text::CreateText(const Math::Vector3& pos, const Math::Vector2& size, std::wstring str, const std::wstring& fontName)
	{
		shared_ptr<GameObject> temp = GameObject::Instantiate("Text");
		auto& com = temp->AddComponent<Text>();
		com->transform()->position = pos;
		com->transform()->SetSize((int)size.x, (int)size.y);
		com->SetText(str);
		com->SetFont(Resource::GetFont(fontName));

		return com;
	}
}