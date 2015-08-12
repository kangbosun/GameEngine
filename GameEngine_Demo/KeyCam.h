

#pragma once

class KeyCam : public Cloneable<KeyCam, Component>
{
public:
	float speed = 5;

	void Update()
	{
		// camera move
		float deltaTime = GameTime::deltaTime;
		if(Input::GetKeyDown((BYTE)'A'))
			GetTransform()->Rotate(Vector3(0, 1, 0), -30 * deltaTime);
		if(Input::GetKeyDown('D'))
			GetTransform()->Rotate(Vector3(0, 1, 0), 30 * deltaTime);
		if(Input::GetKeyDown(VK_UP))
			GetTransform()->TranslateSelf(0, 0, speed * deltaTime);
		if(Input::GetKeyDown(VK_DOWN))
			GetTransform()->TranslateSelf(0, 0, -speed * deltaTime);
		if(Input::GetKeyDown('W'))
			GetTransform()->RotateSelf(Vector3(1, 0, 0), 10 * deltaTime);
		if(Input::GetKeyDown('S'))
			GetTransform()->RotateSelf(Vector3(1, 0, 0), -10 * deltaTime);
		if(Input::GetKeyDown(VK_LEFT))
			GetTransform()->TranslateSelf(-speed * deltaTime, 0, 0);
		if(Input::GetKeyDown(VK_RIGHT))
			GetTransform()->TranslateSelf(speed * deltaTime, 0, 0);
		if(Input::GetKeyDown(VK_NEXT))
			GetTransform()->Translate(0, -speed * deltaTime, 0);
		if(Input::GetKeyDown(VK_PRIOR))
			GetTransform()->Translate(0, speed * deltaTime, 0);
	}
};