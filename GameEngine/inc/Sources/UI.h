#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

#include "Camera.h"
#include "Transform.h"
#include "Singleton.h"

namespace GameEngine
{
	enum UIRenderMode
	{
		eOverlay, eWorldSpace
	};
	enum UIState
	{
		eUp, eDown, eHover
	};
	class UIRenderer;

	// ui object
	class GAMEENGINE_API UIBase  : public Component
	{
	public:
		UIRenderMode renderMode = UIRenderMode::eOverlay;
		Math::Color color = Math::Color::White;
		bool isBillboard = true;

		std::shared_ptr<Camera> renderCamera = Camera::ui;
	protected:
		std::weak_ptr<UIRenderer> _renderer;
	public:
		const std::shared_ptr<UIRenderer> renderer();

	public:
		UIBase() = default;
		UIBase(const UIBase& ui) { color = ui.color; isBillboard = ui.isBillboard; renderCamera = ui.renderCamera; }
		void SetColor(const Math::Color& color);
	};
	//---------------------------------------------------------------------

	class Font;

	class GAMEENGINE_API Text final : public ClonableObject<UIBase, Text>
	{
	private:
		struct TextLine
		{
			std::wstring str;
			int length;
			int width;
		};
	public:

	private:
		std::shared_ptr<Font> font;
		std::wstring text = L"";
		int fontsize = 24;
		bool textChanged = true;
		Align align = Align(Align::eLeft | Align::eTop);

	public:
		void Start() { textChanged = true; }
		void SetAlign(Align _align) { align = _align; textChanged = true; }
		void SetMesh();
		void SetText(const std::wstring& text);
		void SetFontSize(int size);
		void SetFont(const std::shared_ptr<Font>& Font);
		void Update();
		static std::shared_ptr<Text> CreateText(const Math::Vector3& pos, const Math::Vector2& size, std::wstring str, const std::wstring& fontName);

		std::wstring& GetText() { return text; }
	};

	class GAMEENGINE_API Image : public ClonableObject<UIBase, Image>
	{
	public:
		void SetSize(int _width, int _height);
		void SetTexture(const std::shared_ptr<Texture2D>& texture);
		const std::shared_ptr<Texture2D>& GetTexture();
		static std::shared_ptr<Image> CreateImage(const Math::Vector3& pos, const Math::Vector2& size, const std::shared_ptr<Texture2D>& texture = nullptr);
	};

	struct UIStateParam
	{
		bool instersects;
		bool lbuttonDown;
	};

	class GAMEENGINE_API SelectableUI : public ClonableObject<UIBase, SelectableUI>
	{
	public:
		typedef std::function<void()> callback;
	private:
		typedef std::function<void(const UIStateParam&)> UIStateMachine;
		UIState prevState = UIState::eUp;
		UIState state = UIState::eUp;

		UIStateMachine stateMachine;

	public:
		callback onClick;
		callback onDown;
		callback onUp;
		callback onHoverEnter;
		callback onHoverExit;

	public:
		Math::Color colorUp = Math::Color(0.8f, 0.8f, 0.8f, 1);
		Math::Color colorDown = Math::Color(0.5f, 0.5f, 0.5f, 1);
		Math::Color colorHover = Math::Color(0.7f, 0.7f, 0.7f, 1);

		SelectableUI();
		SelectableUI(const SelectableUI& o);

		void SetState(const UIStateParam& param);

	private:
		//for finite state machine
		void state_Up(const UIStateParam& param);
		void state_Down(const UIStateParam& param);
		void state_Hover(const UIStateParam& param);
	};

	class GAMEENGINE_API Button : public ClonableObject<SelectableUI, Button>
	{
	private:
		std::shared_ptr<Image> _image;
		std::shared_ptr<Text> _text;

	public:
		void Start();
		void SetText(const std::wstring& str);
		const std::shared_ptr<Text>& GetText() { return _text; }
		void SetSize(int width, int height);
		static std::shared_ptr<Button> CreateButton(const Math::Vector3& pos, const Math::Vector2& size, const std::wstring& str);
	};

	bool operator <(const std::weak_ptr<UIBase>& lhs, const std::weak_ptr<UIBase>& rhs);

	class GAMEENGINE_API UIInputManager : public ClonableObject<Component, UIInputManager>
	{
	public:
		static std::shared_ptr<UIInputManager> GetInstance();
	private:
		static std::shared_ptr<UIInputManager> instance;
		static std::weak_ptr<SelectableUI> selectedUI;
		static std::list<std::weak_ptr<SelectableUI>> registedUI;

	public:
		static Math::Vector2 ConvertMousePos(const Math::Vector2& pos);
		void Update();
		void Register(const std::shared_ptr<SelectableUI>& ui);
	};

	class GAMEENGINE_API UISelector : public ClonableObject < Component, UISelector >
	{
	public:
		void Start();
	};
}

#pragma warning(pop)