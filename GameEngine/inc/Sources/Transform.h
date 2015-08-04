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

	class GAMEENGINE_API Transform final : public Object, public Node<Transform>
	{
		friend class GameObject;
	public:
		GameObject* gameObject;
		//local
		Vector3 position = { 0, 0, 0 };
		//local
		Quaternion rotation = { 0, 0, 0, 1 };
		//local
		Vector3 scale = { 1, 1, 1 };
		Vector3 pivot = { 0, 0, 0 };
		float width = 0;
		float height = 0;

	private:
		Matrix worldMatrix;
		Matrix localMatrix;
		bool changed = false;
		Pivot pivotFlags = Pivot(eCenter | eCenter);

	public:
		Vector3 up() { return worldMatrix.Up();  };
		Vector3 forward() {  return worldMatrix.Forward();  };
		Vector3 right() { return worldMatrix.Right();  };
		Vector3 worldPosition() { return worldMatrix.GetT(); }
	public:
		void BuildLocalMatrix();

	public:
		Transform() = default;
		Transform(const Transform& rhs);
		void SetPivot(Align _pivot);

		void Rotate(const Vector3& axis, float angle);
		void Rotate(const Quaternion& quaternion);
		void SetRotation(Vector3& euler);
		void RotateSelf(const Vector3& axis, float angle);
		void RotateSelf(float x, float y, float z);

		void Translate(Vector3& v);
		void Translate(float x, float y, float z);
		void TranslateSelf(float x, float y, float z);

		void Scale(const Vector3& s);
		void LookAt(const Vector3& p);
		void LookAt(const std::shared_ptr<Transform>& t);
		void SetLocalTransform(const Matrix& mat);

		void SetSize(int _width, int _height);

		Matrix& WorldMatrix() { return worldMatrix; }
		void WorldMatrix(Matrix& world) { worldMatrix = world; }

		Matrix& LocalMatrix() { return localMatrix; }
	};
}