#include "enginepch.h"
#include "UI.h"
#include "Renderer.h"


namespace GameEngine
{
	bool operator <(const std::weak_ptr<UIBase>& lhs, const std::weak_ptr<UIBase>& rhs)
	{
		return (lhs.lock()->transform()->worldPosition().z < rhs.lock()->transform()->worldPosition().z);
	}

	const std::shared_ptr<UIRenderer> UIBase::renderer() 
	{
		using namespace std;

		if(_renderer.expired()) {
			if(gameObject) {
				_renderer = gameObject->GetComponent<UIRenderer>();
				if(_renderer.expired())
					_renderer = gameObject->AddComponent<UIRenderer>();
			}
		}
		return _renderer.lock();
	}

	void UIBase::SetColor(const Math::Color& _color)
	{
		color = _color;
		renderer()->material.data.color = color;
	}

}
