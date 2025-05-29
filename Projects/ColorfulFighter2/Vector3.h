#pragma once
#include<math.h>
using namespace std;
struct Vector3
{
	float x;
	float y;
	float z;
	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vector3(float X,float Y,float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	//ベクトルの長さを取得
	float Length()const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	//正規化
	Vector3 Normalize()
	{
		Vector3 ans;
		ans = (*this) / Length();
		return ans;
	}

	//演算子オーバーロード
	//+
	Vector3 operator+(const Vector3& right)
	{
		Vector3 ans;
		ans.x = x + right.x;
		ans.y = y + right.y;
		ans.z = z + right.z;
		return ans;
	}
	//-
	Vector3 operator-(const Vector3& right)
	{
		Vector3 ans;
		ans.x = x - right.x;
		ans.y = y - right.y;
		ans.z = z - right.z;
		return ans;
	}
	//+=
	Vector3 operator+=(const Vector3& right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		return *this;
	}
	//+=
	Vector3 operator-=(const Vector3& right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
		return *this;
	}

	// *　スカラー倍
	Vector3 operator*(const float& right)
	{
		Vector3 ans;
		ans.x = x * right;
		ans.y = y * right;
		ans.z = z * right;
		return ans;
	}
	Vector3 operator*=(const float& right)
	{
		x *= right;
		y *= right;
		z *= right;
		return *this;
	}
	 ///　スカラー分割る
	Vector3 operator/(const float& right)
	{
		Vector3 ans;
		ans.x = x / right;
		ans.y = y / right;
		ans.z = z / right;
		return ans;
	}
	 //*=
	// /=
	Vector3 operator/=(const float& right)
	{
		x /= right;
		y /= right;
		z /= right;
		return *this;
	}
};



