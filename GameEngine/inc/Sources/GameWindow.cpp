#include "enginepch.h"

#include "GameWindow.h"
#include "DXUtil.h"
#include "Camera.h"
#include "Resource.h"
#include "GraphicDevice.h"
#include "Input.h"
#include "Light.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Scene.h"
#include "GlyphPool.h"

namespace GameEngine
{
	using namespace std;

	LRESULT CALLBACK gWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		const auto& instance = GameWindow::GetCurrentWindow();
		if(instance != nullptr) {
			return instance->WndProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}


	GameWindow* GameWindow::ptr = nullptr;

	LRESULT GameWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg) {
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_SIZE:
			{
				if(wParam == SIZE_MINIMIZED)
					return 0;
				RECT* rect = new RECT();
				GetClientRect(hWnd, rect);

				width = rect->right - rect->left;
				height = rect->bottom - rect->top;

				positionX = rect->left;
				positionY = rect->top;

				delete rect;

				return 0;
			}
			default:
				return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}



	void GameWindow::Initialize(LPCWSTR title, int width, int height, int updatePerSecond)
	{
		ptr = this;
		WNDCLASS wc = { 0 };
		wc.style = CS_OWNDC;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = _T("DX11Window");
		wc.lpfnWndProc = gWndProc;

		this->width = width;
		this->height = height;

		RECT rect = { 0, 0, width, height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, false);

		if(!RegisterClass(&wc))
			MessageBox(NULL, _T("Error"), _T("Failed to Register Class"), MB_OK);

		hWnd = CreateWindowW(L"DX11Window", title,
							 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
							 CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
							 nullptr, nullptr, nullptr, nullptr);


		graphicDevice = make_shared<GraphicDevice>();
		graphicDevice->Initialize(width, height, hWnd);

		this->updatePerSecond = updatePerSecond;
		updateTime = 1000.0f / updatePerSecond / 1000;
		elaspedTime = 0;

		Camera::main = Camera::CreateCamera(Camera::ProjMode::ePerspective);
		Camera::ui = Camera::CreateCamera(Camera::ProjMode::eOrtho);

		Resource::LoadDefaultResource();

		GlyphPool::GetInstance();
	}

	int GameWindow::Run(int fps)
	{
		run = true;

		while(hWnd) {
			MSG msg = { 0 };
			if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if(msg.message == WM_QUIT) {
					return 1;
				}
			}
			GameTime::Update();
			Update();

			//rendering
			if(graphicDevice) {
				graphicDevice->ClearRenderTarget(Math::Color::CornflowerBlue);
				graphicDevice->ClearDepthStencil();
				Render();
				//swap buffers
				graphicDevice->SwapBuffers();
			}
		}
		return 1;
	}

	void GameWindow::Update()
	{
		elaspedTime += GameTime::unscaledDeltaTime;

		if(elaspedTime >= updateTime) {
			Input::Update(width, height, hWnd);
			if(currScene)
				currScene->Update();		
			elaspedTime -= GameTime::unscaledDeltaTime;
		}
	}

	void GameWindow::Render()
	{
		if(currScene)
			currScene->Render();
	}

	void GameWindow::LoadScene(const shared_ptr<Scene>& scene)
	{
		if(!scene)
			return;
		UnloadScene();
		currScene = scene;
		currScene->OnLoad();
	}

	void GameWindow::UnloadScene()
	{
		if(currScene)
			currScene->OnUnload();
	}
}