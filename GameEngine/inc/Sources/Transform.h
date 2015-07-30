#pragma once

namespace GameEngine
{
	enum Align
	{
		eCenter = 0,
		eLeft = 1 << 0,
		eRight = 1 << 1,
		eTop = 1 << 2,
		eBottom = 1 << 3
	};

	typedef Align Pivot;

	class GAMEENGINE_API Transform : public Object
	{
		friend class GameObject;
	public :
		Node<Transform> node;
	public:
		GameObject* gameObject;
		//local
		Math::Vector3 position = { 0, 0, 0 };
		//local
		Math::Quaternion rotation = { 0, 0, 0, 1 };
		//local
		Math::Vector3 scale = { 1, 1, 1 };
		Math::Vector3 pivot = { 0, 0, 0 };
		float width = 0;
		float height = 0;

	private:
		Math::Matrix worldMatrix;
		Math::Matrix localMatrix;
		bool changed = false;
		Pivot pivotFlags = Pivot(eCenter | eCenter);

	public:
		Math::Vector3 up() { return worldMatrix.Up();  };
		Math::Vector3 forward() {  return worldMatrix.Forward();  };
		Math::Vector3 right() { return worldMatrix.Right();  };
		Math::Vector3 worldPosition() { return worldMatrix.GetT(); }
	public:
		void BuildLocalMatrix();

	public:
		Transform() = default;
		Transform(const Transform& rhs);
		void SetPivot(Align _pivot);

		void Rotate(const Math::Vector3& axis, float angle);
		void Rotate(const Math::Quaternion& quaternion);
		void SetRotation(Math::Vector3& euler);
		void RotateSelf(const Math::Vector3& axis, float angle);
		void RotateSelf(float x, float y, float z);

		void Translate(Math::Vector3& v);
		void Translate(float x, float y, float z);
		void TranslateSelf(float x, float y, float z);

		void Scale(const Math::Vector3& s);
		void LookAt(const Math::Vector3& p);
		void LookAt(const std::shared_ptr<Transform>& t);
		void SetLocalTransform(const Math::Matrix& mat);

		void SetSize(int _width, int _height);

		Math::Matrix& WorldMatrix() { return worldMatrix; }
		void WorldMatrix(Math::Matrix& world) { worldMatrix = world; }

		Math::Matrix& LocalMatrix() { return localMatrix; }
	};
}