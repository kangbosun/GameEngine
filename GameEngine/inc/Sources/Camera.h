#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

#include "GameObject.h"
#include "GraphicDevice.h"

namespace GameEngine
{
	struct CameraData
	{
		Matrix viewMatrix;
		Matrix projMatrix;
		Vector3 position;
	};

	class GAMEENGINE_API Camera final : public Cloneable<Camera, Component>
	{
	private:
		static std::list<std::weak_ptr<Camera>> allCameras;
		CameraData cameraData;
	public:
		enum ProjMode
		{
			eOrtho,
			ePerspective,
		};

		static std::shared_ptr<Camera> main;
		static std::shared_ptr<Camera> ui;

		Vector3 target = Vector3::Zero;
		bool lockTarget = false;

		float fov = 60.0f;
		float nearplane = 0.1f;
		float farplane = 1000.0f;

		ProjMode mode = ePerspective;

		int width = 1;
		int height = 1;
		float size = 1;

		Viewport viewport;
	public:
		void BuildViewProjMatrix();
		void LockTarget(const Vector3& targetPos) { lockTarget = true; target = targetPos; }
		void UnlockTarget() { lockTarget = false; }
		const CameraData& GetCameraData() const { return cameraData; }
		virtual void Update();

	public:
		static std::shared_ptr<Camera> CreateCamera(ProjMode mode);

	private:
	};
}

#pragma warning(pop)
