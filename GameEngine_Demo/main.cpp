

//

#include <GameEngine.h>
//
using namespace std;
using namespace GameEngine;
using namespace GameEngine::Math;

#include "Grid.h"
#include "KeyCam.h"

class Rot : public Component
{
public:
	Vector3 axis = Vector3::Z;
	float speed = 15;
	void Update()
	{
		transform()->Rotate(axis, speed * GameTime::deltaTime);
	}
};


class MousePos : public ClonableObject < Component, MousePos >
{
public:
	weak_ptr<Text> text;

	void Start()
	{
		text = gameObject->GetComponent<Text>();
	}

	void Update()
	{
		auto aa = text.lock()->GetText();
		auto mousepos = Input::GetMousePos();
		auto uimpos = UIInputManager::ConvertMousePos(mousepos);
		wstring str = L"x : " + to_wstring((int)uimpos.x) + L'\n';
		str += L"y : " + to_wstring((int)uimpos.y);
		text.lock()->SetText(str);
	}
};

class Scene01 : public Scene
{
public:
	shared_ptr<GameObject> talia;
	shared_ptr<Light> light1;
	void OnLoad()
	{		
		GameObject::Register(Camera::main->gameObject);
		GameObject::Register(Camera::ui->gameObject);

		auto g = GameObject::Instantiate("Grid");
		g->AddComponent<Grid>();
		GameObject::Register(g);

		light1 = Light::CreateDirectionalLight();
		light1->transform()->Translate(1, 3, 2);
		light1->transform()->LookAt(Vector3(0, 0, 0));
		light1->renderShadow = false;
		GameObject::Register(light1->gameObject);

		//auto& sphere = Resource::GetModel(L"sphere");
		//sphere->transform()->scale = Vector3(10, 10, 10);
		//sphere->transform()->Translate(1, 0.5f, 0);

		auto& plane = Resource::GetModel(L"plane");
		plane->transform.scale = Vector3(200, 1, 200);
		plane->transform.Translate(0.0f, -0.01f, 0.0f);
		GameObject::Register(plane);


		Camera::main->transform()->position = Vector3(0.0f, 1.0f, 2.0f);
		Camera::main->transform()->LookAt(Vector3(0, 0, 0));
		Camera::main->gameObject->AddComponent<KeyCam>();

		Resource::AddModel(L"attack01", "resources\\models", "attack01.fbx");
		auto& attack = Resource::GetModel(L"attack01");
		auto& mat = attack->GetComponentInChildren<MeshRenderer>()->materials[0];
		attack->transform()->Translate(-1, 0, 0);
		attack->GetComponentInChildren<Animation>()->Play("Take 001");
		attack->GetComponentInChildren<Animation>()->speed = 0.2f;
		GameObject::Register(attack);

		Resource::AddModel(L"NightWing", "resources\\models\\nightwing", "NW.fbx");
		auto& nightwing = Resource::GetModel(L"NightWing");
		nightwing->transform()->Scale(Vector3(10, 10, 10));
		nightwing->transform()->Translate(1, 0, 0);
		GameObject::Register(nightwing);

		Resource::AddModel(L"Talia", "resources\\models\\Talia", "Talia.fbx");
		talia = Resource::GetModel(L"Talia");
		//GameObject::Register(talia);

		auto text2 = Text::CreateText(Vector3(960, 540, 2), Vector2(100, 100), L"a", L"NanumGothic");
		text2->transform()->SetPivot(Align(eRight | eTop));
		text2->SetFontSize(32);
		text2->SetAlign(Align(eRight | eTop));
		text2->gameObject->AddComponent<MousePos>();
		text2->SetColor(Color::White);
		GameObject::Register(text2->gameObject);
		text2->SetMesh();
		auto img = Image::CreateImage(Vector3(-960, 540, 1), Vector2(400, 400), text2->renderer()->material.diffuseMap);
		string name = img->ToString();
		img->transform()->SetPivot(Pivot(eLeft | eTop));
		GameObject::Register(img->gameObject);

		auto button = Button::CreateButton(Vector3(960, -540, 1), Vector2(200, 60), L"Button");
		button->transform()->SetPivot(Pivot(eRight | eBottom));
		GameObject::Register(button->gameObject);
		button->SetText(L"Button1");
		button->onClick = bind([&]() { GameObject::Register(talia); });

		auto button2 = dynamic_pointer_cast<GameObject>(button->gameObject->Clone());
		GameObject::Register(button2);
		button2->transform()->Translate(0, 70, 1);
		button2->GetComponent<Button>()->SetText(L"Shadow On/Off");
		button2->GetComponentInChildren<Text>()->SetFontSize(24);
		button2->GetComponent<Button>()->onClick = bind([&]() { light1->renderShadow = !light1->renderShadow; });

		//GraphicDevice::Instance()->vSync = false;
		//GraphicDevice::Instance()->SetFillMode(D3D11_FILL_WIREFRAME);
	}
};


void main()
{
	cout << "hello" << endl;
	Vector3 v;
	GameWindow window;
	window.Initialize(L"GameEngine", 640, 360, 60);
	window.LoadScene(std::make_shared<Scene01>());
	window.Run();
}
