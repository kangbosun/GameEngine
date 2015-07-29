#pragma once
#pragma warning(push)
#pragma warning(disable:4251)



namespace GameEngine 
{
	class GameTime;
	class GraphicDevice;
	class Scene;

	class GAMEENGINE_API GameWindow
	{
	private:		
		float elaspedTime = 0;
		float updateTime = 0;
		int updatePerSecond = 0;
		std::shared_ptr<Scene> currScene;

	protected :
		HWND hWnd = 0;
		std::shared_ptr<GraphicDevice> graphicDevice;
		int width = 0;
		int height = 0;
		int positionX = 0;
		int positionY = 0;
		static GameWindow* ptr;
	private :
		void Update();
		void Render();
		
	public:
		bool run;
		static GameWindow* const  GetCurrentWindow() { return ptr; }
		void Initialize(LPCWSTR title, int width, int height, int updatePerSecond);
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		int Run(int fps = 60);
		void LoadScene(const std::shared_ptr<Scene>& scene);
		void UnloadScene();
		Rect GetRect() { return{ width, height, positionX, positionY }; }
	};

}
#pragma warning(pop)