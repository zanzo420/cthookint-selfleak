#include "math.h"

void inline Math::SinCos(float x, float* s, float* c)
{
	__asm
	{
		fld dword ptr[x]
		fsincos
		mov edx, dword ptr[c]
		mov eax, dword ptr[s]
		fstp dword ptr[edx]
		fstp dword ptr[eax]
	}
}

void Math::AngleVector(const Vector &angles, Vector& forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float sp, sy, cp, cy;

	Math::SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	Math::SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

void Math::AngleVectors(const Vector &angles, Vector& forward, Vector& right, Vector& up)
{
	float sr, sp, sy, cr, cp, cy;

	Math::SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	Math::SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	Math::SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);

	forward.x = (cp * cy);
	forward.y = (cp * sy);
	forward.z = (-sp);
	right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
	right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
	right.z = (-1 * sr * cp);
	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = (cr * cp);
}

void Math::NormalizeAngles(Vector& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

void Math::NormalizeVector(Vector& vec)
{
	for (auto i = 0; i < 3; i++) {
		while (vec[i] < -180.0f) vec[i] += 360.0f;
		while (vec[i] >  180.0f) vec[i] -= 360.0f;
	}
	vec[2] = 0.f;
}

void Math::ClampAngles(Vector& angle)
{
	if (angle.y > 180.0f)
		angle.y = 180.0f;
	else if (angle.y < -180.0f)
		angle.y = -180.0f;

	if (angle.x > 89.0f)
		angle.x = 89.0f;
	else if (angle.x < -89.0f)
		angle.x = -89.0f;

	angle.z = 0;
}

void Math::VectorAngles(const Vector& forward, Vector &angles)
{
	if (forward[1] == 0.0f && forward[0] == 0.0f)
	{
		angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
		angles[1] = 0.0f;  //yaw left/right
	}
	else
	{
		angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / M_PI;
		angles[1] = atan2(forward[1], forward[0]) * 180 / M_PI;

		if (angles[1] > 90)
			angles[1] -= 180;
		else if (angles[1] < 90)
			angles[1] += 180;
		else if (angles[1] == 90)
			angles[1] = 0;
	}

	angles[2] = 0.0f;
}

Vector Math::CalcAngle(Vector src, Vector dst)
{
	Vector angles;
	Vector delta = src - dst;

	Math::VectorAngles(delta, angles);

	delta.Normalize();

	return angles;
}

void Math::CorrectMovement(Vector vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	// side/forward move correction
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

void Math::FixMovement(CUserCmd *pCmd, Vector &wish_angle)
{
	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = pCmd->viewangles;
	viewangles.Normalize();

	Math::AngleVectors(wish_angle, view_fwd, view_right, view_up);
	Math::AngleVectors(viewangles, cmd_fwd, cmd_right, cmd_up);

	const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const float v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const float v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const float v22 = norm_view_fwd.x * pCmd->forwardmove;
	const float v26 = norm_view_fwd.y * pCmd->forwardmove;
	const float v28 = norm_view_fwd.z * pCmd->forwardmove;
	const float v24 = norm_view_right.x * pCmd->sidemove;
	const float v23 = norm_view_right.y * pCmd->sidemove;
	const float v25 = norm_view_right.z * pCmd->sidemove;
	const float v30 = norm_view_up.x * pCmd->upmove;
	const float v27 = norm_view_up.z * pCmd->upmove;
	const float v29 = norm_view_up.y * pCmd->upmove;

	pCmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	pCmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	pCmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	pCmd->forwardmove = clamp(pCmd->forwardmove, -450.f, 450.f);
	pCmd->sidemove = clamp(pCmd->sidemove, -450.f, 450.f);
	pCmd->upmove = clamp(pCmd->upmove, -320.f, 320.f);
}


float Math::GetFov(const Vector& viewAngle, const Vector& aimAngle) {
	Vector delta = aimAngle - viewAngle;
	Math::NormalizeAngles(delta);

	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

float Math::DotProduct(Vector& v1, const float* v2) {
	return v1.x * v2[0] + v1.y * v2[1] + v1.z * v2[2];
}

void Math::VectorTransform(Vector& in1, const matrix3x4_t& in2, Vector& out) {
	out.x = DotProduct(in1, in2[0]) + in2[0][3];
	out.y = DotProduct(in1, in2[1]) + in2[1][3];
	out.z = DotProduct(in1, in2[2]) + in2[2][3];
}

Vector Multiply(Vector v, float fl) {
	return Vector(v.x * fl, v.y * fl, v.z * fl);
}

float Math::DistanceToRay(const Vector &pos, const Vector &rayStart, const Vector &rayEnd, float *along, Vector *pointOnRay)
{
	Vector to = pos - rayStart;
	Vector dir = rayEnd - rayStart;
	float length = dir.NormalizeInPlace();

	float rangeAlong = dir.Dot(to);
	if (along)
		*along = rangeAlong;

	float range;

	if (rangeAlong < 0.0f)
	{
		range = -(pos - rayStart).Length();

		if (pointOnRay)
			*pointOnRay = rayStart;
	}
	else if (rangeAlong > length)
	{
		range = -(pos - rayEnd).Length();

		if (pointOnRay)
			*pointOnRay = rayEnd;
	}
	else
	{
		Vector onRay = rayStart + Multiply(dir, rangeAlong);
		range = (pos - onRay).Length();

		if (pointOnRay)
			*pointOnRay = onRay;
	}

	return range;
}
