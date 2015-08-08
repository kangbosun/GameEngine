

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
			transform()->Rotate(Vector3(0, 1, 0), -30 * deltaTime);
		if(Input::GetKeyDown('D'))
			transform()->Rotate(Vector3(0, 1, 0), 30 * deltaTime);
		if(Input::GetKeyDown(VK_UP))
			transform()->TranslateSelf(0, 0, speed * deltaTime);
		if(Input::GetKeyDown(VK_DOWN))
			transform()->TranslateSelf(0, 0, -speed * deltaTime);
		if(Input::GetKeyDown('W'))
			transform()->RotateSelf(Vector3(1, 0, 0), 10 * deltaTime);
		if(Input::GetKeyDown('S'))
			transform()->RotateSelf(Vector3(1, 0, 0), -10 * deltaTime);
		if(Input::GetKeyDown(VK_LEFT))
			transform()->TranslateSelf(-speed * deltaTime, 0, 0);
		if(Input::GetKeyDown(VK_RIGHT))
			transform()->TranslateSelf(speed * deltaTime, 0, 0);
		if(Input::GetKeyDown(VK_NEXT))
			transform()->Translate(0, -speed * deltaTime, 0);
		if(Input::GetKeyDown(VK_PRIOR))
			transform()->Translate(0, speed * deltaTime, 0);
	}
};