#include "enginepch.h"
#include "Camera.h"
#include "Transform.h"
#include "DebugClass.h"
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
		cameraData.position = GetTransform()->GetPosition();
		float aspect = GlobalSetting::aspectRatio;
		// view matrix
		auto forward = GetTransform()->forward();
		Vector3 look;
		if(lockTarget)
			look = target;
		else
			look = cameraData.position + forward;
    	Matrix::LookAtLH(cameraData.position, look, GetTransform()->up(), cameraData.viewMatrix);

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