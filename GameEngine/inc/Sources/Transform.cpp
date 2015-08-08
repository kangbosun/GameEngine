#include "enginepch.h"
#include "Transform.h"
#include "GameObject.h"
#include "GlobalSetting.h"

namespace GameEngine
{
	using namespace std;


	void Transform::SetSize(int _width, int _height)
	{
		width = (float)_width;
		height = (float)_height;
		SetPivot(pivotFlags);
	}

	void Transform::Rotate(const Vector3& axis, float angle)
	{
		auto& q = Quaternion::AxisAngle(axis, MathUtil::ToRadians(angle));
		rotation *= q;
		changed = true;
	}

	void Transform::Rotate(const Quaternion& quaternion)
	{
		rotation *= quaternion;
		changed = true;
	}

	void Transform::RotateSelf(const Vector3& axis, float angle)
	{
		Vector3 _axis;
		Vector3::Rotate(axis, rotation, _axis);
		auto& q = Quaternion::AxisAngle(_axis, MathUtil::ToRadians(angle));
		rotation *= q;
		changed = true;
	}

	void Transform::SetRotation(Vector3& euler)
	{
		auto& qx = Quaternion::AxisAngle(Vector3::X, MathUtil::ToRadians(euler.x));
		auto& qy = Quaternion::AxisAngle(Vector3::Y, MathUtil::ToRadians(euler.y));
		auto& qz = Quaternion::AxisAngle(Vector3::Z, MathUtil::ToRadians(euler.z));

		rotation = qx * qy * qz;

		changed = true;
	}

	void Transform::RotateSelf(float x, float y, float z)
	{
		auto& qx = Quaternion::AxisAngle(Vector3::X, MathUtil::ToRadians(x));
		auto& qy = Quaternion::AxisAngle(Vector3::Y, MathUtil::ToRadians(y));
		auto& qz = Quaternion::AxisAngle(Vector3::Z, MathUtil::ToRadians(z));

		rotation *= (qx * qy * qz);

		changed = true;
	}

	void Transform::Translate(Vector3& v)
	{
		position += v;
		changed = true;
	}
	void Transform::Translate(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
		changed = true;
	}

	void Transform::TranslateSelf(float x, float y, float z)
	{
		Vector3 m = { x, y, z };
		Vector3::Rotate(m, rotation, m);

		position += m;
		changed = true;
	}

	void Transform::Scale(const Vector3& s)
	{
		scale = scale * s;
		changed = true;
	}

	void Transform::SetLocalTransform(const Matrix& mat)
	{
		Matrix::Decompose(mat, position, rotation, scale);
	}

	inline Vector3 Transform::up()
	{
		updateSelf();
		return worldMatrix.Up();
	}

	inline Vector3 Transform::forward()
	{
		updateSelf();
		return worldMatrix.Forward();
	}

	inline Vector3 Transform::right()
	{
		updateSelf();
		return worldMatrix.Right();
	}

	void Transform::updateSelf()
	{
		worldChanged = false;
		if(changed) {
			Matrix::CreateTransform(localMatrix, position - pivot, rotation, scale, pivot);
			changed = false;
			if(parent)
				Matrix::Multiply(localMatrix, parent->worldMatrix, worldMatrix);
			else
				worldMatrix = localMatrix;
			worldChanged = true;
		}
		else {
			if(parent && parent->worldChanged) {
				Matrix::Multiply(localMatrix, parent->worldMatrix, worldMatrix);
				worldChanged = true;
			}
			else if(parent == nullptr) {
				worldMatrix = localMatrix;
				worldChanged = true;
			}
		}
	}

	void Transform::Update()
	{
		updateSelf();
		for(auto child : children) {
			child->Update();
		}
	}


	Transform::Transform()
	{
		if(this != &root) {
			root.children.push_back(this);
			parent = nullptr;
		}
	}

	Transform::Transform(const Transform & rhs)
	{
		position = rhs.position;
		rotation = rhs.rotation;
		scale = rhs.scale;
		width = rhs.width;
		height = rhs.height;
		pivot = rhs.pivot;
		pivotFlags = rhs.pivotFlags;
		localMatrix = rhs.localMatrix;
	}

	void Transform::SetPivot(Align pivot)
	{
		pivotFlags = pivot;
		float x = 0;
		float y = 0;

		// horizontal
		if(pivot & Align::eLeft)// left
			x = -(width * 0.5f);
		else if(pivot & Align::eRight) // right
			x = (width * 0.5f);
		else // center
			x = 0;

		//vertical
		if(pivot & Align::eTop) // top
			y = height * 0.5f;
		else if(pivot & Align::eBottom)
			y = -(height * 0.5f);
		else
			y = 0;

		this->pivot = Vector3(x, y, 0);
		changed = true;
	}

	void Transform::LookAt(const Vector3& p)
	{
		if(p == position)
			return;
		Matrix m, parentMatrix, inv;

		updateSelf();

		Matrix::LookAtLH(position, p, up(), m);
		m.Inverse(inv);
		Matrix::Multiply(inv, worldMatrix, worldMatrix);
		parentMatrix.Inverse(inv);
		Matrix::Multiply(inv, worldMatrix, localMatrix);
		SetLocalTransform(localMatrix);
	}

	void Transform::LookAt(const shared_ptr<Transform>& t)
	{
		LookAt(t->position);
	}


#pragma region Hierarchy
	Transform Transform::root;
	void Transform::removeChild(Transform* child)
	{
		if(child) {
			children.remove(child);
		}
	}

	void Transform::SetParent(Transform* _parent)
	{
		if(parent)
			parent->removeChild(this);
		if(_parent) {
			_parent->children.push_back(this);
			parent = _parent;
		}
		else {
			root.children.push_back(this);
			this->parent = nullptr;
		}
	}

	Transform* Transform::GetChild(int n)
	{
		if(n < children.size()) {
			auto iter = children.begin();
			for(int i = 0; i < n; ++i)
				++iter;
			return *iter;
		}
		else
			return nullptr;
	}
#pragma endregion

}


