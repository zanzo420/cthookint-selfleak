#pragma once

#include "SDK/SDK.h"

namespace Math {
	void inline SinCos(float radians, float *sine, float *cosine);
	void AngleVector(const Vector &angles, Vector &forward);
	void AngleVectors(const Vector &angles, Vector& forward, Vector& right, Vector& up);
	void NormalizeAngles(Vector& angle);
	void NormalizeVector(Vector& vec);
	void ClampAngles(Vector& angle);
	void VectorAngles(const Vector &forward, Vector &angles);
	Vector CalcAngle(Vector src, Vector dst);
	void CorrectMovement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	float GetFov(const Vector& viewAngle, const Vector& aimAngle);
	float DotProduct(Vector& v1, const float* v2);
	void VectorTransform(Vector& in1, const matrix3x4_t& in2, Vector& out);
	float DistanceToRay(const Vector &pos, const Vector &rayStart, const Vector &rayEnd, float *along = nullptr, Vector *pointOnRay = nullptr);

	void FixMovement(CUserCmd *pCmd, Vector &wish_angle);

	inline float ClampYaw(float yaw)
	{
		while (yaw > 180.f)
			yaw -= 360.f;

		while (yaw < -180.f)
			yaw += 360.f;

		return yaw;
	}

	inline float ClampPitch(float pitch) {
		while (pitch > 89.0f)
			pitch -= 180.f;

		while (pitch < -89.0f)
			pitch += 180.f;

		return pitch;
	}

	/*inline float YawDelta(float yaw, float yaw2) {
		if (yaw == yaw2)
			return 0.f;

		bool oppositeSides = false;

		if (yaw > 0 && yaw2 < 0)
			oppositeSides = true;
		else if (yaw < 0 && yaw2 > 0)
			oppositeSides = true;

		if (!oppositeSides)
			return fabs(yaw - yaw2);

		bool past90 = false;

		if (yaw > 90 && yaw2 < -90)
		{
			yaw -= (yaw - 90);
			yaw2 += (yaw2 + 90);
		}
		else if (yaw < -90 && yaw2 > 90)
		{
			yaw += (yaw + 90);
			yaw2 -= (yaw2 - 90);
		}

		float oneTwo;

		oneTwo = fabs(yaw - yaw2);

		return oneTwo;
	}*/

	inline float YawDelta(float destAngle, float srcAngle)
	{
		float delta;

		delta = fmodf(destAngle - srcAngle, 360.0f);
		if (destAngle > srcAngle)
		{
			if (delta >= 180)
				delta -= 360;
		}
		else
		{
			if (delta <= -180)
				delta += 360;
		}
		return fabsf(delta);
	}

	inline void MatrixAngles(const matrix3x4_t& matrix, float *angles)
	{
		float forward[3];
		float left[3];
		float up[3];

		//
		// Extract the basis vectors from the matrix. Since we only need the Z
		// component of the up vector, we don't get X and Y.
		//
		forward[0] = matrix[0][0];
		forward[1] = matrix[1][0];
		forward[2] = matrix[2][0];
		left[0] = matrix[0][1];
		left[1] = matrix[1][1];
		left[2] = matrix[2][1];
		up[2] = matrix[2][2];

		float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

		// enough here to get angles?
		if (xyDist > 0.001f)
		{
			// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
			angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

			// (roll)	z = ATAN( left.z, up.z );
			angles[2] = RAD2DEG(atan2f(left[2], up[2]));
		}
		else	// forward is mostly Z, gimbal lock-
		{
			// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
			angles[1] = RAD2DEG(atan2f(-left[0], left[1]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

			// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
			angles[2] = 0;
		}
	}

	inline void MatrixGetColumn(const matrix3x4_t &src, int nCol, Vector& pColumn)
	{
		pColumn.x = src[0][nCol];
		pColumn.y = src[1][nCol];
		pColumn.z = src[2][nCol];
	}

	inline void MatrixPosition(const matrix3x4_t &matrix, Vector &position)
	{
		MatrixGetColumn(matrix, 3, position);
	}

	inline void MatrixAngles(const matrix3x4_t &matrix, QAngle &angles)
	{
		MatrixAngles(matrix, &angles.x);
	}

	inline void MatrixAngles(const matrix3x4_t &matrix, QAngle &angles, Vector &position)
	{
		MatrixAngles(matrix, angles);
		MatrixPosition(matrix, position);
	}
}

template<class T>
const T& clamp(const T& x, const T& lower, const T& upper) {
	return min(upper, max(x, lower));
}
