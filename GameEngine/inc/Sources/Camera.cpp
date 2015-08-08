#include "enginepch.h"
#include "Camera.h"
#include "Transform.h"
#include "Debug.h"
#include "GameObject.h"
#include "GlobalSetting.h"

namespace GameEngine
{
	using namespace std;
	

	list<weak_ptr<Camera>> Camera::allCameras;

	void Camera::Update()
	{
		width = GlobalSetting::resolutionX;
		height = GlobalSetting::resolutionY;
		BuildViewProjMatrix();
	}

	void Camera::BuildViewProjMatrix()
	{
		cameraData.position = transform()->position;
		float aspect = GlobalSetting::aspectRatio;
		// view matrix
		auto& forward = transform()->forward();
		Vector3 look;
		if(lockTarget)
			look = target;
		else
			look = transform()->position + forward;

		Matrix::LookAtLH(transform()->position, look, transform()->up(), cameraData.viewMatrix);

		float f = MathUtil::ToRadians(fov);

		if(mode == ProjMode::ePerspective)
			Matrix::PerspectiveFovLH(f, aspect, nearplane, farplane, cameraData.projMatrix);

		else if(mode == ProjMode::eOrtho) {
			float halfw = width * 0.5f * size;
			float halfh = height * 0.5f * size;

			Matrix::OrthographicOffCenterLH(-halfw, halfw, -halfh, halfh, nearplane, farplane, cameraData.projMatrix);
		}
	}

	std::shared_ptr<Camera> Camera::CreateCamera(ProjMode mode)
	{
		auto go = GameObject::Instantiate("Camera");
		auto& com = go->AddComponent<Camera>();
		allCameras.push_back(com);
		com->mode = mode;
		return com;
	}

	std::shared_ptr<Camera> Camera::main;
	std::shared_ptr<Camera> Camera::ui;
}