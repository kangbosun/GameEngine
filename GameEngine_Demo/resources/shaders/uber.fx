
//======================================================================
// Define bitflags
//======================================================================
// maximum flag number is (1<<31)
#define FLAG_LIGHTING 1
#define FLAG_DIFFUSEMAP 2
#define FLAG_SPECULARMAP 4
#define FLAG_NORMALMAP 8
#define FLAG_SKINNING 16
#define FLAG_SHADOWMAP 32
#define FLAG_BUILD_SHADOWMAP 64

#if SHADER_FLAG & FLAG_LIGHTING
#define USE_LIGHTING
#endif

#if SHADER_FLAG & FLAG_DIFFUSEMAP
#define USE_DIFFUSEMAP
#endif

#if SHADER_FLAG & FLAG_SPECULARMAP
#define USE_SPECULARMAP
#endif

#if SHADER_FLAG & FLAG_NORMALMAP
#define USE_NORMALMAP
#endif

#if SHADER_FLAG & FLAG_SKINNING
#define USE_SKINNING
#endif

#if SHADER_FLAG & FLAG_SHADOWMAP
#define USE_SHADOWMAP
#endif

#if SHADER_FLAG & FLAG_BUILD_SHADOWMAP
#define BUILD_SHADOWMAP
#endif


//======================================================================
// Declare functions and structs
//======================================================================
#define MAX_BONE 100
//
#ifdef USE_LIGHTING
struct ShaderMaterial
{
	float4 color;
	float metallic;
	float roughness;
};

struct LightData
{
	float4 color;
	float3 dir;	
	float lightType;
	float3 position;
};

// physically based rendering
float Schlick(float NdotL, float NdotV, float roughness)
{
	float k = roughness * sqrt(2 / 3.14159265358979);
	float oneMinusK = 1 - k;
	return (NdotL / ((NdotL * oneMinusK) + k)) * 
		(NdotV / ((NdotV * oneMinusK) + k));
}

float calcBRDF(float metalic, float roughness, float3 N, float3 V, float3 L, float3 H)
{
	float F, G, D;
	float NdotL = saturate(dot(N, L));
	float NdotV = saturate(dot(N, V));
	
	//Fresnel Term
	F = metalic + ((1 - metalic) * pow((1 - dot(L, H)), 5));
	
	//Distribution Term(BlinnPhong)
	float a = (2.0f /(roughness * roughness)) - 2;
	D = (a + 2) * pow(max(dot(N, H), 0), a) / (2 * 3.14159265358979);
	
	//Geometry Term(schlick's approximation)
	G = Schlick(NdotL, NdotV, roughness);
	
	//final
	float spec = (F * G * D) / (4 * NdotL * NdotV);	
	float diff = NdotL * (1 - metalic);
	return saturate(spec + diff);
}
#endif 
//
// for calculate shadow contribution
#ifdef USE_SHADOWMAP
float gMinVariance = 0.000001f;

float ChebyshevUpperBound(float2 moment, float t)
{
	float p = (t <= moment.x);
	float variance = moment.y - (moment.x*moment.x);
	variance = max(variance, gMinVariance);
	
	float d = t - moment.x;
	float pMax = variance / (variance + (d*d));
	return smoothstep(0, 1, pMax);
}

float CalcShadowFactor(Texture2D shadowMap, SamplerState shadowSampler, float2 lightTexCoord, float distanceToLight)
{
	float2 moment = shadowMap.Sample(shadowSampler, lightTexCoord).xy;
	return ChebyshevUpperBound(moment, distanceToLight);
}
#endif 

struct VertexInput
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	uint4   boneIndices : BLENDINDICES;
	float4 weights : BLENDWEIGHTS;
};

struct GlobalSetting
{
	float4 ambient;
	float shadowMapSize;
};

//======================================================================
// VertexShader 
//======================================================================

// VertexShader variables
float4x4 worldMatrix;
float4x4 viewMatrix;
float4x4 projectionMatrix;

#ifdef USE_SHADOWMAP
float4x4 shadowTransform;
#endif
#ifdef USE_LIGHTING
float3 cameraPos;
#endif
#ifdef USE_SKINNING
float4x4 gBones[MAX_BONE];
#endif


//======================================================================
// VertexShader for build a shadowmap
//=======================================================================
#ifdef BUILD_SHADOWMAP
struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(0, 0, 0, 0);
	
#ifdef USE_SKINNING
	[unroll]
	for(int i = 0; i < 4; i++) 
		pos += input.weights[i] * mul(float4(input.position, 1), gBones[input.boneIndices[i]]);
#else 
	pos = float4(input.position, 1);	
#endif	
	output.position = mul(pos, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.texcoord = input.texcoord;
	return output;
}
//=======================================================================
#else 


struct PixelInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;	
#ifdef USE_NORMALMAP
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
#endif
	float4 worldPos : TEXCOORD1;
#ifdef USE_SHADOWMAP
	float4 shadowPos : TEXCOORD2;
#endif	
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	float4 pos = float4(0, 0, 0, 0);
	float4 worldPos;
	float4 worldViewPos;
	float3 normal = float3(0, 0, 0);	
		
#ifdef USE_SKINNING		
	[unroll]
	for(int i = 0; i < 4; ++i) {
		pos += input.weights[i] * mul(float4(input.position, 1), gBones[input.boneIndices[i]]);
		normal += input.weights[i] * mul(input.normal, (float3x3)gBones[input.boneIndices[i]]);
	}
#else
	pos = float4(input.position, 1);
	normal = input.normal;
#endif
	worldPos = mul(pos, worldMatrix);
	worldViewPos = mul(worldPos, viewMatrix);
	float4 worldViewProjPos = mul(worldViewPos, projectionMatrix);
	
	output.position = worldViewProjPos;
	output.normal = normalize(mul(normal, (float3x3)worldMatrix));
	output.texcoord = input.texcoord;
	output.worldPos = worldPos;

#ifdef USE_NORMALMAP
	#ifndef USE_SKINNING
	float3 tangent = input.tangent;
	float3 binormal = input.binormal;
	#else
		float3 tangent = float3(0, 0, 0);
		float3 binormal = float3(0, 0, 0);
		[unroll]
		for(int j = 0; j < 4; ++j) {
			tangent += input.weights[j] * mul(float4(input.tangent, 1), gBones[input.boneIndices[j]]).xyz;
			binormal += input.weights[j] * mul(float4(input.binormal, 1), gBones[input.boneIndices[j]]).xyz;
		}
	#endif
	// for normalMap;
	output.tangent = normalize(mul(float4(tangent, 1), worldMatrix)).xyz;
	output.binormal = normalize(mul(float4(binormal, 1), worldMatrix).xyz);
#endif

	
#ifdef USE_SHADOWMAP	
	// for shadow mapping
	float4 shadowPos = mul(worldPos, shadowTransform);
	output.shadowPos = shadowPos;
#endif
	
	return output;
}
#endif



//======================================================================
// PixelShader
//======================================================================
SamplerState samplerStateLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState samplerStatePoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerComparisonState samplerComparisonState
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	ComparisonFunc = LESS_EQUAL;
};


#ifdef USE_DIFFUSEMAP
Texture2D diffuseMap;
#endif

float4 color;

#ifndef BUILD_SHADOWMAP
	#ifdef USE_SPECULARMAP
	Texture2D specularMap;
	#endif
	#ifdef USE_NORMALMAP
	Texture2D normalMap;
	#endif
	#ifdef USE_SHADOWMAP
	Texture2D shadowMap;
	#endif

	#ifdef USE_LIGHTING
	LightData light;
	ShaderMaterial material;
	#endif
#endif

GlobalSetting globalSetting;

#ifdef BUILD_SHADOWMAP
//=================================================================
	float2 PS(PixelInput input) : SV_Target
	{
	#ifdef USE_DIFFUSEMAP
		float4 color = diffuseMap.Sample(samplerStateLinear, input.texcoord);
	#endif
		float depth = input.position.z ;
		
		float2 moment;
		moment.x = depth;
		float dx = ddx(depth);
		float dy = ddy(depth);
		moment.y = (depth*depth) + (0.25*((dx*dx) + (dy*dy)));
		return moment;
	}
//=================================================================
#else

	float4 PS(PixelInput input) : SV_TARGET
	{
		float4 colorFinal = float4(1, 1, 1, 1);
			
	#ifdef USE_DIFFUSEMAP
		float4 diffuseMapColor = diffuseMap.Sample(samplerStateLinear, input.texcoord);
		colorFinal *= diffuseMapColor * color;
	#endif

	#ifdef USE_LIGHTING
		#ifdef USE_NORMALMAP	
			float4 bump = normalMap.Sample(samplerStateLinear, input.texcoord);
			bump = (bump * 2.0f) - 1.0f;
			float3 normal = input.normal + (bump.x * input.tangent) + (bump.y * input.binormal);
		#else 
			float3 normal = input.normal;	
		#endif
		// calc light 
		float3 N = normalize(normal);
		float3 L = -normalize(light.dir);
		float3 V = normalize(cameraPos - input.worldPos.xyz);
		float3 H = normalize(L + V);
		
		float NdotL = saturate(dot(N, L));
		float BRDF = calcBRDF(material.metallic, material.roughness, N, V, L, H);
		float4 lightColor = float4(light.color.rgb * BRDF, light.color.a);
		colorFinal *= lightColor;	
	#endif

	#ifdef USE_SHADOWMAP
		// shadow
		float shadowFactor = 1.0f;	
		float3 shadowPos = (input.shadowPos.xyz / input.shadowPos.w);
		shadowFactor = CalcShadowFactor(shadowMap, samplerStateLinear, shadowPos.xy, shadowPos.z);	
		colorFinal = float4(colorFinal.rgb * shadowFactor, colorFinal.a);
	#endif
				
		return saturate(colorFinal);
	}
#endif 

technique11 Uber
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}



