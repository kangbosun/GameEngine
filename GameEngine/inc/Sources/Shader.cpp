#include "enginepch.h"
#include "Shader.h"
#include "GraphicDevice.h"
#include "Debug.h"
#include "Material.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Transform.h"
#include "Texture2D.h"
#include "Light.h"
#include "Component.h"
#include "GlobalSetting.h"

namespace GameEngine
{
	using namespace std;
	

	ID3DX11EffectVariable* const Shader::GetVariable(std::string name) const
	{
		auto iter = variables.find(name);
		if(iter != variables.cend())
			return iter->second.p;
		else
			return nullptr;
	}

	Shader::Shader()
	{
	}

	inline Shader::~Shader() 
	{ 

	}

	void Shader::Render(const CComPtr<ID3D11DeviceContext>& context, int n, int offset)
	{
		context->IASetInputLayout(layout);

		D3DX11_TECHNIQUE_DESC techDesc;
		tech->GetDesc(&techDesc);
		for(UINT i = 0; i < techDesc.Passes; i++) {
			tech->GetPassByIndex(i)->Apply(0, context.p);
			context->DrawIndexed(n, offset, 0);
		}
	}

	bool Shader::InitShader(const wstring& filename)
	{
		DWORD shaderFlags = 0;
#if defined(_DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		CComPtr<ID3DBlob> compileShader, compilationMsgs;

		HRESULT result = D3DX11CompileFromFile(filename.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &compileShader.p, &compilationMsgs.p, 0);
		if(compilationMsgs.p) {
			Debug::Log((char*)compilationMsgs->GetBufferPointer());
			compileShader.Release();
			return false;
		}

		if(FAILED(result)) {
			Debug::Log(L"Failed to Compile " + wstring(filename));
			return false;
		}

		auto dx = GraphicDevice::Instance();
		if(!dx)
			return false;

		auto device = dx->device;

		result = D3DX11CreateEffectFromMemory(compileShader->GetBufferPointer(), compileShader->GetBufferSize(), 0, device, &effect.p);
		if(FAILED(result)) {
			Debug::Log("Failed to D3DX11CreateEffectFromMemory");
			return false;
		}

		D3DX11_EFFECT_DESC effectDesc;
		effect->GetDesc(&effectDesc);
		const int nTechs = effectDesc.Techniques;

		tech = effect->GetTechniqueByIndex(0);

		const int nVars = effectDesc.GlobalVariables;
		const int nInterface = effectDesc.InterfaceVariables;

		shaderVariables.diffuseMap = effect->GetVariableByName("diffuseMap")->AsShaderResource();
		shaderVariables.normalMap = effect->GetVariableByName("normalMap")->AsShaderResource();
		shaderVariables.bones = effect->GetVariableByName("gBones")->AsMatrix();
		shaderVariables.cameraData = effect->GetVariableByName("camera");
		shaderVariables.globalSetting = effect->GetVariableByName("globalSetting");
		shaderVariables.lightData = effect->GetVariableByName("light");
		shaderVariables.material = effect->GetVariableByName("material");
		shaderVariables.shadowMap = effect->GetVariableByName("shadowMap")->AsShaderResource();
		shaderVariables.shadowMatrix = effect->GetVariableByName("shadowTransform")->AsMatrix();
		shaderVariables.specularMap = effect->GetVariableByName("specularMap")->AsShaderResource();
		shaderVariables.worldMatrix = effect->GetVariableByName("worldMatrix")->AsMatrix();

		interfaces.diffuseMapping = effect->GetVariableByName("diffuseMapping")->AsInterface();
		interfaces.lighting = effect->GetVariableByName("lighting")->AsInterface();
		interfaces.normalMapping = effect->GetVariableByName("normalMapping")->AsInterface();
		interfaces.shadowMapping = effect->GetVariableByName("shadowMapping")->AsInterface();
		interfaces.skinning = effect->GetVariableByName("skinning")->AsInterface();
		interfaces.specularMapping = effect->GetVariableByName("specularMapping")->AsInterface();

		classInstances.diffuseMapEnable = effect->GetVariableByName("diffuseMapEnable")->AsClassInstance();
		classInstances.diffuseMapDisable = effect->GetVariableByName("diffuseMapDisable")->AsClassInstance();

		classInstances.normalMapEnable = effect->GetVariableByName("normalMapEnable")->AsClassInstance();
		classInstances.normalMapDisable = effect->GetVariableByName("normalMapDisable")->AsClassInstance();

		classInstances.shadowMapEnable = effect->GetVariableByName("shadowMapEnable")->AsClassInstance();
		classInstances.shadowMapDisable = effect->GetVariableByName("shadowMapDisable")->AsClassInstance();

		classInstances.lightingEnable = effect->GetVariableByName("lightingEnable")->AsClassInstance();
		classInstances.lightingDisable = effect->GetVariableByName("lightingDisable")->AsClassInstance();

		classInstances.skinningEnable = effect->GetVariableByName("skinningEnable")->AsClassInstance();
		classInstances.skinningDisable = effect->GetVariableByName("skinningDisable")->AsClassInstance();

		for(int i = 0; i < nVars; ++i) {
			D3DX11_EFFECT_VARIABLE_DESC varDesc;
			auto var = effect->GetVariableByIndex(i);
			var->GetDesc(&varDesc);
			//variables[varDesc.Name] = var;
		}

		//for(int i = 0; i < nInterface; ++i) {
		//	D3DX11_EFFECT_VARIABLE_DESC varDesc;
		//}

		auto pass = tech->GetPassByIndex(0);

		if(!pass) {
			Debug::Log("Failed to GetPassByName");
			return false;
		}
		D3DX11_PASS_DESC passDesc;
		result = pass->GetDesc(&passDesc);

		if(FAILED(result)) {
			Debug::Log("Failed to GetDesc");
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC* pDesc = new D3D11_INPUT_ELEMENT_DESC[6];

		tech->GetPassByIndex(0)->GetDesc(&passDesc);

		ZeroMemory(&pDesc[0], sizeof(D3D11_INPUT_ELEMENT_DESC));
		ZeroMemory(&pDesc[1], sizeof(D3D11_INPUT_ELEMENT_DESC));
		ZeroMemory(&pDesc[2], sizeof(D3D11_INPUT_ELEMENT_DESC));
		ZeroMemory(&pDesc[3], sizeof(D3D11_INPUT_ELEMENT_DESC));
		ZeroMemory(&pDesc[4], sizeof(D3D11_INPUT_ELEMENT_DESC));
		ZeroMemory(&pDesc[5], sizeof(D3D11_INPUT_ELEMENT_DESC));

		pDesc[0].SemanticName = "POSITION";
		pDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		pDesc[1].SemanticName = "TEXCOORD";
		pDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		pDesc[1].AlignedByteOffset = 12;
		pDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		pDesc[2].SemanticName = "NORMAL";
		pDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pDesc[2].AlignedByteOffset = 20;
		pDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		pDesc[3].SemanticName = "TANGENT";
		pDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pDesc[3].AlignedByteOffset = 32;
		pDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		pDesc[4].SemanticName = "BLENDINDICES";
		pDesc[4].Format = DXGI_FORMAT_R8G8B8A8_UINT;
		pDesc[4].AlignedByteOffset = 44;
		pDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		pDesc[5].SemanticName = "BLENDWEIGHTS";
		pDesc[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pDesc[5].AlignedByteOffset = 48;
		pDesc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		result = device->CreateInputLayout(pDesc, 6, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &layout.p);

		if(FAILED(result)) {
			Debug::Log("Failed to create input vertex layout");
			layout = nullptr;
			return false;
		}

		delete[] pDesc;
		return true;
	}

	void Shader::SetVariable(const string& name, void* pData, size_t size)
	{
		auto var = GetVariable(name);
		if(var && var->IsValid())
			var->SetRawValue(pData, 0, (uint32_t)size);
	}

	void Shader::SetVector(const string& name, float* pData)
	{
		auto var = GetVariable(name);
		if(var && var->IsValid())
			var->AsVector()->SetFloatVector(pData);
	}

	void Shader::SetShaderResource(const string& name, ID3D11ShaderResourceView* resource)
	{
		auto var = GetVariable(name);
		if(var && var->IsValid())
			var->AsShaderResource()->SetResource(resource);
	}

	void Shader::SetMatrix(const string& name, const Matrix& matrix)
	{
		auto var = GetVariable(name);
		if(var && var->IsValid())
			var->AsMatrix()->SetMatrix((float*)&matrix);
	}

	void Shader::SetMatrixArray(const string& name, Matrix* matrices, int cnt)
	{
		auto var = GetVariable(name);
		if(var && var->IsValid())
			var->AsMatrix()->SetMatrixArray((const float*)matrices, 0, cnt);
	}

	void Shader::SetDiffuseMap(const Texture2D * const diffuseMap)
	{
		if(shaderVariables.diffuseMap && shaderVariables.diffuseMap->IsValid()) {
			if(diffuseMap && diffuseMap->IsValid()) {
				shaderVariables.diffuseMap->SetResource(diffuseMap->srv.p);
				interfaces.diffuseMapping->SetClassInstance(classInstances.diffuseMapEnable);
			}
			else {
				interfaces.diffuseMapping->SetClassInstance(classInstances.diffuseMapDisable);
			}
		}
	}
	void Shader::SetNormalMap(const Texture2D * const normalMap)
	{
		if(shaderVariables.normalMap && shaderVariables.normalMap->IsValid()) {
			if(normalMap && normalMap->IsValid()) {
				shaderVariables.normalMap->SetResource(normalMap->srv.p);
				interfaces.normalMapping->SetClassInstance(classInstances.normalMapEnable);
			}
			else {
				interfaces.normalMapping->SetClassInstance(classInstances.normalMapDisable);
			}
		}
	}
	void Shader::SetSpecularMap(const Texture2D * const specularMap)
	{
		if(shaderVariables.specularMap && shaderVariables.specularMap->IsValid()) {
			if(specularMap && specularMap->IsValid()) {
				shaderVariables.specularMap->SetResource(specularMap->srv.p);
				interfaces.specularMapping->SetClassInstance(classInstances.specularMapEnable);
			}
			else {
				interfaces.specularMapping->SetClassInstance(classInstances.specularMapDisable);
			}
		}
	}
	void Shader::SetShadowMap(const Texture2D * const shadowMap, const Matrix* const shadowMatrix)
	{
		if(shaderVariables.shadowMap && shaderVariables.shadowMap->IsValid()) {
			if(shadowMap && shadowMap->IsValid()) {
				if(shaderVariables.shadowMatrix && shaderVariables.shadowMatrix->IsValid()) {
					if(shadowMatrix) {
						shaderVariables.shadowMap->SetResource(shadowMap->srv.p);
						shaderVariables.shadowMatrix->SetMatrix((const float*)shadowMatrix);
						interfaces.shadowMapping->SetClassInstance(classInstances.shadowMapEnable);
						return;
					}
				}
			}
		}
		interfaces.shadowMapping->SetClassInstance(classInstances.shadowMapDisable);
	}
	void Shader::SetLight(const LightData* const lightData)
	{
		if(shaderVariables.lightData && shaderVariables.lightData->IsValid()) {
			if(lightData) {
				shaderVariables.lightData->SetRawValue(lightData, 0, sizeof(LightData));
				interfaces.lighting->SetClassInstance(classInstances.lightingEnable);
			}
			else
				interfaces.lighting->SetClassInstance(classInstances.lightingDisable);
		}
	}
	void Shader::SetMaterial(const MaterialData * const material)
	{
		if(shaderVariables.material && shaderVariables.material->IsValid()) {
			if(material) {
				shaderVariables.material->SetRawValue(material, 0, sizeof(MaterialData));
			}
		}
	}
	void Shader::SetCamera(const CameraData * const camera)
	{
		if(shaderVariables.cameraData && shaderVariables.cameraData->IsValid()) {
			if(camera)
				shaderVariables.cameraData->SetRawValue(camera, 0, sizeof(CameraData));
		}
	}
	void Shader::SetGlobalSetting(const GraphicSetting * const globalSetting)
	{
		if(shaderVariables.globalSetting && shaderVariables.globalSetting->IsValid()) {
			if(globalSetting)
				shaderVariables.globalSetting->SetRawValue(globalSetting, 0, sizeof(GraphicSetting));
		}
	}
	void Shader::SetWorldMatrix(const Matrix * const world)
	{
		if(shaderVariables.worldMatrix && shaderVariables.worldMatrix->IsValid()) {
			if(world)
				shaderVariables.worldMatrix->SetMatrix((const float*)world);
		}
	}

	void Shader::SetBoneMatrices(const Matrix * const matrices, uint32_t cnt)
	{
		auto& var = shaderVariables.bones;
		if(var && var->IsValid()) {
			if(matrices) {
				var->SetMatrixArray((const float*)matrices, 0, cnt);
				interfaces.skinning->SetClassInstance(classInstances.skinningEnable);
			}
			else
				interfaces.skinning->SetClassInstance(classInstances.skinningDisable);
		}
	}
}