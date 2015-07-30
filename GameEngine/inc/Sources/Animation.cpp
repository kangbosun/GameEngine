#include "enginepch.h"
#include "Animation.h"
#include "GameObject.h"
#include "Renderer.h"
#include "GameTime.h"

#include "Transform.h"

namespace GameEngine
{
	using namespace std;
	using namespace Math;

	void Animation::Play(const string& clipName)
	{
		if(!gameObject)
			return;

		auto pair = clips.find(clipName);
		if(pair != clips.cend()) {
			elaspedTime = 0;
			clip = pair->second;
			// has transform curves;
			if(clip->transformCurves.size() > 0) {
				//find top object
				auto root = transform();
				while(root->node.GetParent())
					root = root->node.GetParent();
				//done

				transforms.reserve(clip->transformCurves.size());
				for(auto& curve : clip->transformCurves) {
					auto g = root->gameObject->FindGameObjectInChildren(curve->boneName);
					transforms.push_back(&g->transform);
				}
			}
			run = true;
		}
	}

	void Animation::Update()
	{
		if(!clip || !run)
			return;

		elaspedTime += GameTime::deltaTime * speed;
		float endTime = clip->lengthInSeconds;
		float startTime = clip->startTime;

		while(elaspedTime >= endTime) {
			if(!loop) {
				elaspedTime = 0;
				run = false;
				return;
			}
			elaspedTime -= endTime;
		}

		if(elaspedTime < startTime)
			return;

		const size_t nBones = transforms.size();
		const size_t nClusters = clip->transformCurves.size();
		if(nBones != nClusters) {
			run = false;
			return;
		}

		//elaspedTime = 0.0f;

		for(int i = 0; i < nBones; ++i) {
			auto& transformCurve = clip->transformCurves[i];
			auto& transform = transforms[i];
			//translation
			transform->position = transformCurve->EvaluateT(elaspedTime);
			//scaling
			transform->scale = transformCurve->EvaluateS(elaspedTime);
			//rotation
			transform->SetRotation(transformCurve->EvaluateR(elaspedTime));
		}
	}
}