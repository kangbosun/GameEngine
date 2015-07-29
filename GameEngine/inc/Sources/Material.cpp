
#include "enginepch.h"
#include "Material.h"
#include "Resource.h"

#include "Shader.h"

namespace GameEngine
{
	std::shared_ptr<Material> Material::Clone()
	{
		auto mat = std::make_shared<Material>();
		mat->name = name;
		mat->data = data;
		mat->diffuseMap = diffuseMap;
		mat->specularMap = specularMap;
		mat->normalMap = normalMap;
		mat->shader = shader;
		return mat;
	}
}