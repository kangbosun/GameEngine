#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	namespace Math
	{
		struct AnimClip;
		struct AnimCurve;
	}

	class SkinnedMeshRenderer;
	class Transform;

	class GAMEENGINE_API Animation : public ClonableObject<Component, Animation>
	{
	private:
		float elaspedTime = 0;
		std::vector<Transform*> transforms;
		bool isValid = false;

	public:
		std::unordered_map<std::string, std::shared_ptr<Math::AnimClip>> clips;
		std::shared_ptr<Math::AnimClip> clip = nullptr;

		float speed = 1.0f;
		bool loop = true;
		bool run = false;

		void Play(const std::string& clipName);
		void Update();
	};
}

#pragma warning(pop)