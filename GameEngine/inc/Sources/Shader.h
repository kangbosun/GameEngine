#pragma once

#pragma warning(push)
#pragma warning(disable:4251)

struct ID3D11InputLayout;
struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectVariable;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3DX11EffectShaderResourceVariable;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVectorVariable;
struct ID3DX11EffectInterfaceVariable;
struct ID3DX11EffectClassInstanceVariable;

namespace GameEngine
{
	class DXCBuffer;
	class Material;
	class GameObject;
	struct MaterialData;
	class Texture2D;
	struct LightData;
	struct CameraData;
	struct GraphicSetting;

	class GAMEENGINE_API IShader abstract : public Object
	{
	public:
		virtual void Render(const CComPtr<ID3D11DeviceContext>& context, int n, int offset = 0) = 0;
		virtual void SetVariable(const std::string& name, void* pData, size_t size) = 0;
		virtual void SetShaderResource(const std::string& name, ID3D11ShaderResourceView* resource) = 0;
		virtual void SetMatrix(const std::string& name, const Math::Matrix& matrix) = 0;
		virtual void SetVector(const std::string& name, float* pData) = 0;
		virtual void SetMatrixArray(const std::string& name, Math::Matrix* matrices, int cnt) = 0;
	};

	class GAMEENGINE_API Shader : public IShader
	{
	protected:
		CComPtr<ID3DX11Effect> effect;
		CComPtr<ID3DX11EffectTechnique> tech;

		struct
		{
			CComPtr<ID3DX11EffectShaderResourceVariable> diffuseMap;
			CComPtr<ID3DX11EffectShaderResourceVariable> normalMap;
			CComPtr<ID3DX11EffectShaderResourceVariable> specularMap;
			CComPtr<ID3DX11EffectShaderResourceVariable> shadowMap;
			CComPtr<ID3DX11EffectVariable> material;
			CComPtr<ID3DX11EffectVariable> lightData;
			CComPtr<ID3DX11EffectMatrixVariable> bones;
			CComPtr<ID3DX11EffectVariable> cameraData;
			CComPtr<ID3DX11EffectMatrixVariable> worldMatrix;
			CComPtr<ID3DX11EffectMatrixVariable> shadowMatrix;
			CComPtr<ID3DX11EffectVariable> globalSetting;
		} shaderVariables;

		struct
		{
			CComPtr<ID3DX11EffectInterfaceVariable> diffuseMapping;
			CComPtr<ID3DX11EffectInterfaceVariable> normalMapping;
			CComPtr<ID3DX11EffectInterfaceVariable> shadowMapping;
			CComPtr<ID3DX11EffectInterfaceVariable> lighting;
			CComPtr<ID3DX11EffectInterfaceVariable> skinning;
			CComPtr<ID3DX11EffectInterfaceVariable> specularMapping;
		} interfaces;

		struct
		{
			CComPtr<ID3DX11EffectClassInstanceVariable> diffuseMapEnable;
			CComPtr<ID3DX11EffectClassInstanceVariable> diffuseMapDisable;
			CComPtr<ID3DX11EffectClassInstanceVariable> normalMapEnable;
			CComPtr<ID3DX11EffectClassInstanceVariable> normalMapDisable;
			CComPtr<ID3DX11EffectClassInstanceVariable> shadowMapEnable;
			CComPtr<ID3DX11EffectClassInstanceVariable> shadowMapDisable;
			CComPtr<ID3DX11EffectClassInstanceVariable> lightingEnable;
			CComPtr<ID3DX11EffectClassInstanceVariable> lightingDisable;
			CComPtr<ID3DX11EffectClassInstanceVariable> skinningEnable;
			CComPtr<ID3DX11EffectClassInstanceVariable> skinningDisable;
			CComPtr<ID3DX11EffectClassInstanceVariable> specularMapEnable;
			CComPtr<ID3DX11EffectClassInstanceVariable> specularMapDisable;
		} classInstances;
		
		CComPtr<ID3D11InputLayout> layout;		
		std::unordered_map<std::string, CComPtr<ID3DX11EffectVariable>> variables;
	protected:
		ID3DX11EffectVariable* const GetVariable(std::string name) const;

	public:
		std::string name;
		Shader();
		~Shader();
		virtual bool InitShader(const std::wstring& filename);

		virtual void Render(const CComPtr<ID3D11DeviceContext>& context, int n, int offset = 0) override;

		void SetVariable(const std::string& name, void* pData, size_t size) override;
		void SetShaderResource(const std::string& name, ID3D11ShaderResourceView* resource) override;
		void SetMatrix(const std::string& name, const Math::Matrix& matrix) override;
		void SetVector(const std::string& name, float* pData) override;
		void SetMatrixArray(const std::string& name, Math::Matrix* matrices, int cnt) override;

		void SetDiffuseMap(const Texture2D* const diffuseMap);
		void SetNormalMap(const Texture2D* const normalMap);
		void SetSpecularMap(const Texture2D* const specularMap);
		void SetShadowMap(const Texture2D* const shadowMap, const Math::Matrix* const shadowMatrix);

		void SetLight(const LightData* const lightData);
		void SetMaterial(const MaterialData* const material);
		void SetCamera(const CameraData* const camera);
		void SetGlobalSetting(const GraphicSetting* const globalSetting);

		void SetWorldMatrix(const Math::Matrix* const world);
		void SetBoneMatrices(const Math::Matrix* const matrices, int cnt);
	};
}
#pragma warning(pop)