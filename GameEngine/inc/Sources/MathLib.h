
#pragma once

#ifdef _WIN64
#include <DirectXMath.h>
#endif

#include <vector>
#include <string>

#pragma warning(push)
#pragma warning(disable:4251)
#pragma warning(disable:4275)

namespace GameEngine
{
	struct Rect
	{
		int width;
		int height;
		int left;
		int top;
	};

	struct Rectf
	{
		float width;
		float height;
		float left;
		float top;
	};

	namespace Math
	{
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
		//forward
		struct Vector2;
		struct Vector3;
		struct Vector4;
		struct Color;
		struct Quaternion;
		struct Matrix;

		

		typedef unsigned char Byte;
		struct GAMEENGINE_API Byte4
		{
			Byte m[4];
			Byte operator[](int i);
		};

#ifdef _WIN64
#define GAMEMATH_DEFINED
		struct GAMEENGINE_API Vector2 : public DirectX::XMFLOAT2
		{
			Vector2() {}
			Vector2(float _x, float _y) : XMFLOAT2(_x, _y) {}
			Vector2(const float* pArray) : XMFLOAT2(pArray) {}

			Vector2 operator+(const Vector2& rhs);
			Vector2 operator-(const Vector2& rhs);
			friend Vector2 operator*(float scalar, const Vector2& v1);
			friend Vector2 operator*(const Vector2& v1, float scalar);

			bool operator==(const Vector2& rhs);

			float Length();
		};

		// Vector3
		struct GAMEENGINE_API Vector3 : public DirectX::XMFLOAT3
		{
			Vector3() {}
			Vector3(float _x, float _y, float _z) : XMFLOAT3(_x, _y, _z) {}
			Vector3(const float* pArray) : XMFLOAT3(pArray) {}
			
			Vector3 operator+(const Vector3& rhs);
			Vector3 operator-(const Vector3& rhs);
			Vector3 operator*(const Vector3& rhs);
			Vector3 operator-() { return Vector3(-x, -y, -z); }

			friend Vector3 operator*(float scalar, const Vector3& v1);
			friend Vector3 operator*(const Vector3& v1, float scalar);
			
			Vector3& operator+=(const Vector3& rhs);
			Vector3& operator-=(const Vector3& rhs);
			Vector3& operator*=(float scalar);

			static float Dot(const Vector3& v1, const Vector3& v2);
			static void Cross(const Vector3& v1, const Vector3& v2, Vector3& out);
			static void Rotate(const Vector3& v, const Quaternion& q, Vector3& out);

			bool operator==(const Vector3& rhs) const;

			float Length();
			float LengthSq();
			Vector3 Normal();
			void Normalize();

			static const Vector3 Zero;
			static const Vector3 Y;
			static const Vector3 X;
			static const Vector3 Z;
		};

		// Vector4
		struct GAMEENGINE_API Vector4 : public DirectX::XMFLOAT4
		{
			Vector4() {}
			Vector4(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
			Vector4(const float* pArray) : XMFLOAT4(pArray) {}

			Vector4 operator+(const Vector4& rhs);
			Vector4 operator-(const Vector4& rhs);
			friend Vector4 operator*(float scalar, const Vector4& v1);
			friend Vector4 operator*(const Vector4& v1, float scalar);

			bool operator==(const Vector4& rhs);
			float& operator[](int i);
			float operator[](int i) const;
			float Length();
		};

		// Quaternion
		struct GAMEENGINE_API Quaternion : public Vector4
		{
			static const Quaternion Identity;

			Quaternion() {}
			Quaternion(float _x, float _y, float _z, float _w) : Vector4(_x, _y, _z, _w) {}
			Quaternion(const float* pArray) : Vector4(pArray) {}

			static Quaternion AxisAngle(const Vector3& axis, float angle);
			Quaternion operator*(const Quaternion& rhs);
			Quaternion& operator*=(const Quaternion& rhs);
			Quaternion Normal();
			void Normalize();
			Quaternion Inverse();
		};

		struct GAMEENGINE_API Color : public Vector4
		{
			static const Color White;
			static const Color Black;
			static const Color Red;
			static const Color Green;
			static const Color Blue;
			static const Color Yellow;
			static const Color CornflowerBlue;
			static const Color Silver;

			Color() {}
			Color(float _x, float _y, float _z, float _w) : Vector4(_x, _y, _z, _w) {}
			Color(const float* pArray) : Vector4(pArray) {}

			Color operator+(const Color& rhs) { return{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
			Color operator-(const Color& rhs) { return{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }

			friend Color operator*(float scalar, const Color& v1) { return{ v1.x * scalar, v1.y * scalar, v1.z * scalar, v1.w * scalar }; }
			friend Color operator*(const Color& v1, float scalar) { return{ v1.x * scalar, v1.y * scalar, v1.z * scalar, v1.w * scalar }; }
		};


		// 4x4 Matrix 
		struct GAMEENGINE_API Matrix : public DirectX::XMFLOAT4X4
		{
			Matrix() : XMFLOAT4X4() {}
			Matrix(const float* pArray) : XMFLOAT4X4(pArray) {}
			Matrix(float m00, float m01, float m02, float m03,
				   float m10, float m11, float m12, float m13,
				   float m20, float m21, float m22, float m23,
				   float m30, float m31, float m32, float m33) :
				   XMFLOAT4X4(
				   m00, m01, m02, m03,
				   m10, m11, m12, m13,
				   m20, m21, m22, m23,
				   m30, m31, m32, m33
				   )
			{
			}
			Matrix(const Vector3& row1, const Vector3& row2, const Vector3& row3) :
				Matrix(
				row1.x, row1.y, row1.z, 0,
				row2.x, row2.y, row2.z, 0,
				row3.x, row3.y, row3.z, 0,
				0, 0, 0, 1)
			{
			}

			Matrix(const Matrix& mat) : XMFLOAT4X4(mat) {}

			Matrix operator+(const Matrix& rhs);
			Matrix operator-(const Matrix& rhs);
			Matrix operator*(const Matrix& rhs) const;

			//Matrix& operator+=(const Matrix& rhs);
			//Matrix& operator-=(const Matrix& rhs);
			Matrix& operator*=(const Matrix& rhs);

			void Inverse(Matrix& out);
			void Transpose(Matrix& out);
			void SetIdentity();

			Vector3 Up() { return Vector3(_21, _22, _23).Normal(); }
			Vector3 Right() { return Vector3(_11, _12, _13).Normal(); }
			Vector3 Forward() { return Vector3(_31, _32, _33).Normal(); }

			static void Multiply(const Matrix& m1, const Matrix& m2, Matrix& out);

			static void CreateTransform(Matrix& out, const Vector3& T, const Quaternion& Q, const Vector3& S, const Vector3& Pivot = Vector3::Zero);
			static void CreateAxisAngle(const Vector3& axis, float angle, Matrix& out);

			static void Decompose(const Matrix& in, Vector3& outT, Quaternion& outQ, Vector3& outS);
			void Decompose(Vector3& outT, Quaternion& outQ, Vector3& outS);
			Vector3 GetT() const;
			Quaternion GetQ() const;
			Vector3 GetS() const;

			void SetT(const Vector3& t);
			void SetQ(const Vector3& q);
			void SetS(const Vector3& s);

			Vector4 GetRow(int n) const;
			Vector4 GetColumn(int n) const;

			static void CreateTranslate(const Vector3& T, Matrix& out);
			static void CreateRotation(const Quaternion& Q, Matrix& out);
			static void CreateScaling(const Vector3& S, Matrix& out);
			static void CreateRoatationX(float angle, Matrix& out);
			static void CreateRoatationY(float angle, Matrix& out);
			static void CreateRoatationZ(float angle, Matrix& out);

			static void LookAtLH(const Vector3& eyePos, const Vector3& focusPos, const Vector3& up, Matrix& out);
			static void LookAtRH(const Vector3& eyePos, const Vector3& focusPos, const Vector3& up, Matrix& out);

			static void PerspectiveFovLH(float fov, float aspect, float nearZ, float farZ, Matrix& out);
			static void PerspectiveFovRH(float fov, float aspect, float nearZ, float farZ, Matrix& out);

			static void OrthographicOffCenterLH(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ, Matrix& out);
			static void OrthographicOffCenterRH(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ, Matrix& out);

			static const Matrix Identity;
		};
#else
#endif

#ifdef GAMEMATH_DEFINED
		struct GAMEENGINE_API MathUtil
		{
		public :
			static const float PI;
			static const float PIOVER180;
			static const float PIUNDER180;

		public :
			static float ToRadians(float degrees);
			static float ToDegrees(float radians);
			static int ToPiexels(int pt, int dpi) { return (int)(pt * dpi / 72); }
		};


		struct GAMEENGINE_API Vertex
		{
			Vector3 pos;
			Vector2 tex;
			Vector3 normal;
			Vector3 tangent;
			Byte4 blendIndices;
			Vector4 blendWeights;
		};

		struct GAMEENGINE_API Bone
		{
			std::string name;
			int index = 0;
			Matrix bindPose;
			Matrix bindPoseInverse;
			std::vector<Bone*> children;
			~Bone();
		};


		struct GAMEENGINE_API Keyframe
		{
			float leftTangent = 0;
			float rightTangent = 0;
			float time = 0;
			float value = 0;
		};

		struct GAMEENGINE_API AnimCurve
		{
			enum AnimCurveType
			{
				eTranslation, eScaling, eRotation,
			};
			AnimCurveType type;
			char componentName = 'E';

			std::string propName;
			std::vector<Keyframe*> keyframes;
			int length = 0;

			float Evaluate(float time, bool angle = false);
			Keyframe* operator[](int n);
			~AnimCurve();
		};

		struct GAMEENGINE_API AnimCurve3
		{
			AnimCurve* curves[3];
		public:

			AnimCurve3();
			~AnimCurve3();
			AnimCurve* operator [](int i);

			Vector3 Evaluate(float time, bool degree);
		};

		struct GAMEENGINE_API AnimCurveContainer abstract
		{
		};


		struct GAMEENGINE_API AnimTransformCurve : public AnimCurveContainer
		{
			std::string boneName;
			AnimCurve3* translation = new AnimCurve3();
			AnimCurve3* rotation = new AnimCurve3();
			AnimCurve3* scaling = new AnimCurve3();

			Vector3 EvaluateT(float time);
			Vector3 EvaluateR(float time);
			Vector3 EvaluateS(float time);
			~AnimTransformCurve();
		};


		struct GAMEENGINE_API AnimClip
		{
			std::string name;
			float lengthInSeconds = 0;
			int lengthInFrames = 0;
			float startTime = 0;
			std::vector<AnimTransformCurve*> transformCurves;
			~AnimClip();
		};
#endif
	}
}


#pragma warning(pop)