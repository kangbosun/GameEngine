
#include "enginepch.h"
#include "MathLib.h"

namespace GameEngine
{
	namespace Math
	{
		unsigned char Byte4::operator[](int i)
		{
			return m[min(i, 3)];
		}

#ifdef _WIN64
		// Vector2
		Vector2 Vector2::operator+(const Vector2& rhs)
		{
			return Vector2(x + rhs.x, y + rhs.y);
		}
		Vector2 Vector2::operator-(const Vector2& rhs)
		{
			return Vector2(x - rhs.x, y - rhs.y);
		}

		Vector2 operator*(float scalar, const Vector2& v1)
		{
			return Vector2(v1.x * scalar, v1.y * scalar);
		}
		Vector2 operator*(const Vector2& v1, float scalar)
		{
			return Vector2(v1.x * scalar, v1.y * scalar);
		}

		bool Vector2::operator==(const Vector2& rhs)
		{
			return (x == rhs.x) && (y = rhs.y);
		}

		float Vector2::Length()
		{
			return DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMLoadFloat2(this)));
		}

		// Vector3
		Vector3 Vector3::operator+(const Vector3& rhs)
		{
			return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
		}
		Vector3 Vector3::operator-(const Vector3& rhs)
		{
			return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
		}

		Vector3 Vector3::operator*(const Vector3& rhs)
		{
			return Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
		}

		Vector3 operator*(float scalar, const Vector3& v1)
		{
			return Vector3(v1.x * scalar, v1.y * scalar, v1.z * scalar);
		}

		Vector3 operator*(const Vector3& v1, float scalar)
		{
			return Vector3(v1.x * scalar, v1.y * scalar, v1.z * scalar);
		}

		Vector3& Vector3::operator+=(const Vector3& rhs)
		{
			x += rhs.x; y += rhs.y; z += rhs.z;
			return *this;
		}

		Vector3& Vector3::operator-=(const Vector3& rhs)
		{
			x -= rhs.x; y -= rhs.y; z -= rhs.z;
			return *this;
		}

		Vector3& Vector3::operator*=(float scalar)
		{
			x *= scalar; y *= scalar; z *= scalar;
			return *this;
		}

		float Vector3::Dot(const Vector3& v1, const Vector3& v2)
		{
			DirectX::XMVECTOR v = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2));
			return DirectX::XMVectorGetX(v);
		}

		void Vector3::Cross(const Vector3& v1, const Vector3& v2, Vector3& out)
		{
			DirectX::XMVECTOR v = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2));
			DirectX::XMStoreFloat3(&out, v);
		}

		bool Vector3::operator==(const Vector3& rhs) const
		{
			return DirectX::XMVector3Equal(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&rhs));
		}

		float Vector3::Length()
		{
			return DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(this)));
		}

		float Vector3::LengthSq()
		{
			return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(this)));
		}

		Vector3 Vector3::Normal()
		{
			Vector3 normal;
			DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(this)));
			return normal;
		}

		void Vector3::Normalize()
		{
			DirectX::XMStoreFloat3(this, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(this)));
		}

		void Vector3::Rotate(const Vector3& v, const Quaternion& q, Vector3& out)
		{
			DirectX::XMVECTOR& Q = DirectX::XMLoadFloat4(&q);
			DirectX::XMStoreFloat3(&out, DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&v), DirectX::XMLoadFloat4(&q)));
		}

		const Vector3 Vector3::Zero = { 0, 0, 0 };
		const Vector3 Vector3::X = { 1, 0, 0 };
		const Vector3 Vector3::Y = { 0, 1, 0 };
		const Vector3 Vector3::Z = { 0, 0, 1 };


		// Vector4
		Vector4 Vector4::operator+(const Vector4& rhs)
		{
			return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}
		Vector4 Vector4::operator-(const Vector4& rhs)
		{
			return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}

		Vector4 operator*(float scalar, const Vector4& v1)
		{
			return Vector4(v1.x * scalar, v1.y * scalar, v1.z * scalar, v1.w * scalar);
		}
		Vector4 operator*(const Vector4& v1, float scalar)
		{
			return Vector4(v1.x * scalar, v1.y * scalar, v1.z * scalar, v1.w * scalar);
		}

		bool Vector4::operator==(const Vector4& rhs)
		{
			return DirectX::XMVector4Equal(DirectX::XMLoadFloat4(this), DirectX::XMLoadFloat4(&rhs));
		}

		float& Vector4::operator[](int i)
		{
			if(i == 0) return x;
			if(i == 1) return y;
			if(i == 2) return z;
			if(i == 3) return w;
			return x;
		}

		float Vector4::operator[](int i) const
		{
			if(i == 0) return x;
			if(i == 1) return y;
			if(i == 2) return z;
			if(i == 3) return w;
			return x;
		}

		float Vector4::Length()
		{
			return DirectX::XMVectorGetX(DirectX::XMVector4Length(DirectX::XMLoadFloat4(this)));
		}

		//Quaternion

		Quaternion Quaternion::AxisAngle(const Vector3& axis, float angle)
		{
			Quaternion q;
			DirectX::XMVECTOR _axis = DirectX::XMLoadFloat3(&axis);
			DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionRotationAxis(_axis, angle));
			return q;
		}

		Quaternion Quaternion::operator*(const Quaternion& rhs)
		{
			Quaternion q;
			DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&rhs);
			DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionMultiply(q1, q2));
			return q;
		}

		Quaternion& Quaternion::operator*=(const Quaternion& rhs)
		{
			DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(this);
			DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&rhs);
			DirectX::XMStoreFloat4(this, DirectX::XMQuaternionMultiply(q1, q2));
			return *this;
		}

		Quaternion Quaternion::Normal()
		{
			Quaternion q;
			DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(this)));
			return q;
		}
		void Quaternion::Normalize()
		{
			DirectX::XMStoreFloat4(this, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(this)));
		}

		Quaternion Quaternion::Inverse()
		{
			Quaternion q;
			DirectX::XMStoreFloat4(&q, DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(this)));
			return q;
		}

		const Quaternion Quaternion::Identity = { 0, 0, 0, 1 };

		// Color
		const Color Color::White = { 1.0f, 1.0f, 1.0f, 1.0f };
		const Color Color::Black = { 0.0f, 0.0f, 0.0f, 1.0f };
		const Color Color::Red = { 1.0f, 0.0f, 0.0f, 1.0f };
		const Color Color::Green = { 0.0f, 1.0f, 0.0f, 1.0f };
		const Color Color::Blue = { 0.0f, 0.0f, 1.0f, 1.0f };;
		const Color Color::Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
		const Color Color::CornflowerBlue = { 0.39f, 0.58f, 0.92f, 1.0f };
		const Color Color::Silver = { 0.75f, 0.75f, 0.75f, 1.0f };

		//Matrix
		Matrix Matrix::operator+(const Matrix& rhs)
		{
			DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(this);
			DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&rhs);
			Matrix m;
			DirectX::XMStoreFloat4x4(&m, m1 + m2);
			return m;
		}

		Matrix Matrix::operator-(const Matrix& rhs)
		{
			DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(this);
			DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&rhs);
			Matrix m;
			DirectX::XMStoreFloat4x4(&m, m1 - m2);
			return m;
		}

		Matrix Matrix::operator*(const Matrix& rhs) const
		{
			DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(this);
			DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&rhs);
			Matrix m;
			DirectX::XMStoreFloat4x4(&m, m1 * m2);
			return m;
		}

		Matrix& Matrix::operator*=(const Matrix& rhs)
		{
			DirectX::XMMATRIX m1 = DirectX::XMLoadFloat4x4(this);
			DirectX::XMMATRIX m2 = DirectX::XMLoadFloat4x4(&rhs);
			DirectX::XMStoreFloat4x4(this, m1 * m2);
			return *this;
		}

		void Matrix::Inverse(Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(this)));
		}

		void Matrix::Transpose(Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(this)));
		}

		void Matrix::Multiply(const Matrix& m1, const Matrix& m2, Matrix& out)
		{
			DirectX::XMMATRIX _m1 = DirectX::XMLoadFloat4x4(&m1);
			DirectX::XMMATRIX _m2 = DirectX::XMLoadFloat4x4(&m2);
			DirectX::XMStoreFloat4x4(&out, _m1 * _m2);
		}


		void Matrix::CreateTransform(Matrix& out, const Vector3& T, const Quaternion& Q, const Vector3& S, const Vector3& Pivot)
		{
			DirectX::XMVECTOR _T = DirectX::XMLoadFloat3(&T);
			DirectX::XMVECTOR _Q = DirectX::XMLoadFloat4(&Q);
			DirectX::XMVECTOR _S = DirectX::XMLoadFloat3(&S);
			DirectX::XMVECTOR _P = DirectX::XMLoadFloat3(&Pivot);
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixAffineTransformation(_S, _P, _Q, _T));
		}

		void Matrix::CreateAxisAngle(const Vector3& axis, float angle, Matrix& out)
		{
			DirectX::XMVECTOR _axis = DirectX::XMLoadFloat3(&axis);
			DirectX::XMMATRIX m = DirectX::XMMatrixRotationAxis(_axis, angle);
			DirectX::XMStoreFloat4x4(&out, m);
		}

		void Matrix::Decompose(const Matrix& in, Vector3& outT, Quaternion& outQ, Vector3& outS)
		{
			DirectX::XMVECTOR T, S, Q;
			DirectX::XMMatrixDecompose(&S, &Q, &T, DirectX::XMLoadFloat4x4(&in));
			DirectX::XMStoreFloat3(&outT, T);
			DirectX::XMStoreFloat4(&outQ, Q);
			DirectX::XMStoreFloat3(&outS, S);
		}

		void Matrix::Decompose(Vector3& outT, Quaternion& outQ, Vector3& outS)
		{
			DirectX::XMVECTOR T, S, Q;
			DirectX::XMMatrixDecompose(&S, &Q, &T, DirectX::XMLoadFloat4x4(this));
			DirectX::XMStoreFloat3(&outT, T);
			DirectX::XMStoreFloat4(&outQ, Q);
			DirectX::XMStoreFloat3(&outS, S);
		}

		Vector3 Matrix::GetT() const
		{
			return Vector3(_41, _42, _43);
		}

		Quaternion Matrix::GetQ() const
		{
			using namespace DirectX;
			XMVECTOR v = XMQuaternionRotationMatrix(XMLoadFloat4x4(this));
			return{ v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3] };
		}

		Vector3 Matrix::GetS() const
		{
			using namespace DirectX;
			Vector4 x = GetRow(0);
			Vector4 y = GetRow(1);
			Vector4 z = GetRow(2);

			Vector3 vx = { x[0], x[1], x[2] };
			Vector3 vy = { y[0], y[1], y[2] };
			Vector3 vz = { z[0], z[1], z[2] };

			return{ vx.Length(), vy.Length(), vz.Length() };
		}

		void Matrix::SetIdentity()
		{
			memset(m, 0, sizeof(float) * 16);
			m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
		}

		Vector4 Matrix::GetRow(int n) const
		{
			n = max(min(n, 3), 0);
			return{ m[n][0], m[n][1], m[n][2], m[n][3] };
		}

		Vector4 Matrix::GetColumn(int n) const
		{
			n = max(min(n, 3), 0);
			return{ m[0][n], m[1][n], m[2][n], m[3][n] };
		}

		void Matrix::CreateTranslate(const Vector3& T, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixTranslation(T.x, T.y, T.z));
		}

		void Matrix::CreateRotation(const Quaternion& Q, Matrix& out)
		{
			DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&Q);
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixRotationQuaternion(q));
		}

		void Matrix::CreateRoatationX(float angle, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixRotationX(angle));
		}

		void Matrix::CreateRoatationY(float angle, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixRotationY(angle));
		}

		void Matrix::CreateRoatationZ(float angle, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixRotationZ(angle));
		}

		void Matrix::CreateScaling(const Vector3& S, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixScaling(S.x, S.y, S.z));
		}

		void Matrix::LookAtLH(const Vector3& eyePos, const Vector3& focusPos, const Vector3& up, Matrix& out)
		{
			DirectX::XMVECTOR _eyePos = DirectX::XMLoadFloat3(&eyePos);
			DirectX::XMVECTOR _focusPos = DirectX::XMLoadFloat3(&focusPos);
			DirectX::XMVECTOR _up = DirectX::XMLoadFloat3(&up);
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixLookAtLH(_eyePos, _focusPos, _up));
		}

		void Matrix::LookAtRH(const Vector3& eyePos, const Vector3& focusPos, const Vector3& up, Matrix& out)
		{
			DirectX::XMVECTOR _eyePos = DirectX::XMLoadFloat3(&eyePos);
			DirectX::XMVECTOR _focusPos = DirectX::XMLoadFloat3(&focusPos);
			DirectX::XMVECTOR _up = DirectX::XMLoadFloat3(&up);
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixLookAtRH(_eyePos, _focusPos, _up));
		}

		void Matrix::PerspectiveFovLH(float fov, float aspect, float nearZ, float farZ, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ));
		}

		void Matrix::PerspectiveFovRH(float fov, float aspect, float nearZ, float farZ, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixPerspectiveFovRH(fov, aspect, nearZ, farZ));
		}

		void Matrix::OrthographicOffCenterLH(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixOrthographicOffCenterLH(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ));
		}

		void Matrix::OrthographicOffCenterRH(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ, Matrix& out)
		{
			DirectX::XMStoreFloat4x4(&out, DirectX::XMMatrixOrthographicOffCenterRH(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ));
		}

		const Matrix Matrix::Identity = { 
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1};

#else

#endif

		const float MathUtil::PI = 3.14159265358979f;
		const float MathUtil::PIOVER180 = 3.14159265358979f / 180.0f;
		const float MathUtil::PIUNDER180 = 180.0f / 3.14159265358979f;

		float MathUtil::ToRadians(float degrees)
		{
			return degrees * PIOVER180;
		}

		float MathUtil::ToDegrees(float radians)
		{
			return radians * PIUNDER180;
		}

		//
		Bone::~Bone()
		{
			for(auto& bone : children)
				delete bone;
		}

		//
		Keyframe* AnimCurve::operator[](int n)
		{
			return keyframes[min(length, n)];
		}

		float AnimCurve::Evaluate(float time, bool angle)
		{
			Keyframe* start = nullptr;
			Keyframe* end = nullptr;

			if(keyframes.size() < 1)
				return 0;

			for(int i = 0; i < keyframes.size(); ++i) {
				auto keyframe = keyframes[i];
				if(keyframe->time <= time) {
					start = keyframe;
					if((i + 1) < keyframes.size())
						end = keyframes[i + 1];
					else
						return start->value;
				}
			}
			float t = (time - start->time) / (end->time - start->time);
			float p0 = start->value;
			float p1 = end->value;

			if(angle) {
				float d = abs(p1 - p0);
				if(d > 180) {
					if(p1 > p0)
						p0 += 360;
					else
						p1 += 360;
				}
			}

			// linear interpolation
			return (p0 + (p1 - p0) * t);
		}

		AnimCurve::~AnimCurve()
		{

			for(auto key : keyframes)
				delete key;
		}

		// 
		AnimCurve3::AnimCurve3()
		{
			for(auto& curve : curves)
				curve = new AnimCurve();
		}

		AnimCurve3::~AnimCurve3()
		{
			for(auto& curve : curves)
				delete curve;
		}

		AnimCurve* AnimCurve3::operator [](int i)
		{
			return curves[min(i, 2)];
		}

		Vector3 AnimCurve3::Evaluate(float time, bool degree)
		{
			return{
				curves[0]->Evaluate(time, degree),
				curves[1]->Evaluate(time, degree),
				curves[2]->Evaluate(time, degree)
			};
		}

		AnimTransformCurve::~AnimTransformCurve()
		{
			delete translation;
			delete rotation;
			delete scaling;
		}

		Vector3 AnimTransformCurve::EvaluateT(float time)
		{
			return translation->Evaluate(time, false);
		}
		Vector3 AnimTransformCurve::EvaluateR(float time)
		{
			return rotation->Evaluate(time, true);
		}
		Vector3 AnimTransformCurve::EvaluateS(float time)
		{
			return scaling->Evaluate(time, false);
		}

		//
		AnimClip::~AnimClip()
		{
			for(auto& curve : transformCurves)
				delete curve;
		}
	}
}