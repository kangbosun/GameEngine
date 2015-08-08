#include "enginepch.h"
#include "UI.h"
#include "Input.h"
#include "GraphicDevice.h"
#include "GlobalSetting.h"
#include "GameWindow.h"

namespace GameEngine
{
	using namespace std;
	

	weak_ptr<SelectableUI> UIInputManager::selectedUI;
	list<weak_ptr<SelectableUI>> UIInputManager::registedUI;
	std::shared_ptr<UIInputManager> UIInputManager::instance;

	shared_ptr<UIInputManager> UIInputManager::GetInstance()
	{
		if(!instance) {
			auto g = GameObject::Instantiate("UIInputManager");
			instance = g->AddComponent<UIInputManager>();
			GameObject::Register(g);
		}
		return instance;
	}

	void UIInputManager::Register(const std::shared_ptr<SelectableUI>& ui)
	{
		if(ui) {
			registedUI.push_back(ui);
		}
	}

	Vector2 UIInputManager::ConvertMousePos(const Vector2& pos)
	{
		int cameraWidth = Camera::ui->width;
		int cameraHeight = Camera::ui->height;
		float w = (float)cameraWidth;
		float h = (float)cameraHeight;
		auto screenRect = GameWindow::GetCurrentWindow()->GetRect();

		float xratio = w / screenRect.width;
		float yratio = h / screenRect.height;

		float x = pos.x * xratio;
		float y = h - (pos.y * yratio);

		return{ x - (w / 2), y - (h / 2) };
	}

	void UIInputManager::Update()
	{
		using namespace DirectX;
		auto mousepos = Input::GetMousePos();
		auto uimpos = ConvertMousePos(mousepos);

		Vector3 origin = Vector3(uimpos.x, uimpos.y, 0);
		Vector3 dir = Vector3::Z;
		float dis = 10;

		bool skip = false;

		registedUI.sort();

		for(auto iter = registedUI.begin(); iter != registedUI.end();) {
			if(iter->expired())
				iter = registedUI.erase(iter);

			else {
				auto ui = iter->lock();
				if(skip) {
					ui->SetState({ false, false });
				}
				else {
					auto transform = ui->transform();
					Vector3 center = { 0, 0, 0 };
					Vector3 extends = Vector3(transform->width / 2, transform->height / 2, 0);
					BoundingBox box = { center, extends };

					Vector3 s, t;
					Quaternion q;
					Matrix world = transform->WorldMatrix();
					world.Decompose(t, q, s);
					float sx = GlobalSetting::UIScaleFactorX;
					float sy = GlobalSetting::UIScaleFactorY;
					t.x *= sx; t.y *= sy;
					s.x *= sx; s.y *= sy;
					Matrix::CreateTransform(world, t, q, s);

					box.Transform(box, XMMATRIX(&world.m[0][0]));

					bool b = box.Intersects(XMLoadFloat3(&origin), XMLoadFloat3(&dir), dis);
					ui->SetState({ b, Input::GetKeyDown(VK_LBUTTON) });
					if(b)
						skip = true;
				}
				++iter;
			}
		}
	}

	void UISelector::Start()
	{
		auto& ui = gameObject->GetComponent<SelectableUI>();
		if(ui)
			UIInputManager::GetInstance()->Register(ui);
	}
}