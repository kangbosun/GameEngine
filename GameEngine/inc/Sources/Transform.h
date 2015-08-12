#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

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

	class GAMEENGINE_API Transform final : public Object
	{
		friend class Scene;
		friend class GameObject;
	public :		
		float width = 0;
		float height = 0;

	private :
		GameObject* gameObject;
		//local
		Vector3 position = { 0, 0, 0 };
		//local
		Quaternion rotation = { 0, 0, 0, 1 };
		//local
		Vector3 scale = { 1, 1, 1 };
		Vector3 pivot = { 0, 0, 0 };

	public :
		auto GetGameObject() { return gameObject; }

		void SetPosition(const Vector3& p) { position = p; localChanged = true; }
		void SetRotation(const Quaternion& q) { rotation = q; localChanged = true; }
		void SetRotation(Vector3& euler);
		void SetScale(const Vector3& s) { scale = s; localChanged = true; }
		void SetPivot(const Vector3& pv) { pivot = pv; localChanged = true; }

		auto GetPosition() { return position; }
		auto GetRotation() { return rotation; }
		auto GetScale() { return scale; }
		auto GetPivot() { return pivot; }

	private:
		Matrix localMatrix;
		Matrix worldMatrix;
		
		bool localChanged = true;
		bool worldChanged = true;

		Pivot pivotFlags = Pivot(eCenter | eCenter);

	public:
		Vector3 up();
		Vector3 forward();
		Vector3 right();

		Vector3 worldPosition() { return worldMatrix.GetT(); }
		Quaternion worldRotation() { return worldMatrix.GetQ(); }
		Vector3 worldScale() { return worldMatrix.GetS(); }
	
	private :
		void updateSelf();
	public:
		void Update();
	public:
		Transform();
		Transform(const Transform& rhs);
		void SetPivot(Align _pivot);

		void Rotate(const Vector3& axis, float angle);
		void Rotate(const Quaternion& quaternion);
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

#pragma region Hierarchy
	private :
		static Transform root;
		Transform* parent = nullptr;
		std::list<Transform*> children;

	private :
		void removeChild(Transform* child);

	public :
		void SetParent(Transform* _parent);
		inline Transform* GetParent() { return parent; }
		inline size_t GetChildCount() { return children.size(); }
		Transform* GetChild(int n);

#pragma endregion
	};
}

#pragma warning(pop)