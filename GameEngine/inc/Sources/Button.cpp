#include "enginepch.h"
#include "Transform.h"
#include "UI.h"
#include "Resource.h"

namespace GameEngine
{
	void Button::Start()
	{
		if(gameObject) {
			_text = gameObject->GetComponentInChildren<Text>();
			_image = gameObject->GetComponentInChildren<Image>();
		}
	}

	std::shared_ptr<Button> Button::CreateButton(const Vector3& pos, const Vector2& size, const std::wstring& str)
	{
		using namespace std;
		auto g = GameObject::Instantiate("Button");

		auto text = Text::CreateText(Vector3(0, 0, -0.1f), size, str, "NanumGothic");
		text->SetColor(Color::Black);
		text->SetAlign(Align(eCenter | eCenter));
		text->SetFontSize((int)(size.y * 0.5f));

		auto img = g->AddComponent<Image>();
		img->SetSize((int)size.x, (int)size.y);
		img->SetTexture(Resource::textures.Find("white"));

		g->transform.AddChild(text->transform());
		auto& button = g->AddComponent<Button>();
		button->SetSize((int)size.x, (int)size.y);
		button->transform()->position = pos;

		g->AddComponent<UISelector>();

		return button;
	}

	void Button::SetText(const std::wstring& str)
	{
		if(_text) {
			_text->SetText(str);
		}
	}

	void Button::SetSize(int width, int height)
	{
		if(_text) {
			_text->SetFontSize((int)(height * 0.8f));
			_text->transform()->SetSize(width, height);
			_image->SetSize(width, height);
		}
	}
}