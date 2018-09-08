#pragma once

#include "SDK.h"

struct CTickRecord;

struct CValidTick2 {
	explicit operator CTickRecord() const;

	explicit operator bool() const noexcept {
		return m_flSimulationTime > 0.f;
	}

	float m_flPitch = 0.f;
	float m_flYaw = 0.f;
	float m_flSimulationTime = 0.f;
	C_BasePlayer* m_pEntity = nullptr;
	int tickcount = 0;
};

struct CTickRecord {
	CTickRecord() {
	}

	CTickRecord(C_BasePlayer* player, int tickCount, float correctTime) {
		m_flLowerBodyYawTarget = *player->GetLowerBodyYawTarget();
		m_angEyeAngles = *player->GetEyeAngles();
		m_flCycle = player->GetCycle();
		m_flSimulationTime = correctTime;
		m_nSequence = player->GetSequence();
		m_vecOrigin = player->GetVecOrigin();
		m_vecVelocity = player->GetVelocity();
		m_flPoseParameter = player->GetPoseParameter();
		headPos = player->GetBonePosition((int)Bone::BONE_HEAD);
		pelvisPos = player->GetBonePosition((int)Bone::BONE_PELVIS);
		absOrigin = player->GetAbsOriginal2();
		flags = player->GetFlags();
		tickcount = tickCount;
		lbyUpdated = true;
		matrixBuilt = player->SetupBones(matrix, 128, 256, m_flSimulationTime);
		pModel = player->GetModel();
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	CTickRecord(C_BasePlayer* player, int tickCount, bool lby = false) {
		m_flLowerBodyYawTarget = *player->GetLowerBodyYawTarget();
		m_angEyeAngles = *player->GetEyeAngles();
		m_flCycle = player->GetCycle();
		m_flSimulationTime = player->GetSimulationTime();
		m_nSequence = player->GetSequence();
		m_vecOrigin = player->GetVecOrigin();
		m_vecVelocity = player->GetVelocity();
		m_flPoseParameter = player->GetPoseParameter();
		headPos = player->GetBonePosition((int)Bone::BONE_HEAD);
		pelvisPos = player->GetBonePosition((int)Bone::BONE_PELVIS);
		absOrigin = player->GetAbsOriginal2();
		flags = player->GetFlags();
		tickcount = tickCount;
		lbyUpdated = lby;
		matrixBuilt = player->SetupBones(matrix, 128, 256, m_flSimulationTime);
		pModel = player->GetModel();
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	CTickRecord(C_BasePlayer* player, float correctTime, int priorit, int tickCount, bool lby = false) {
		m_flLowerBodyYawTarget = *player->GetLowerBodyYawTarget();
		m_angEyeAngles = *player->GetEyeAngles();
		m_flCycle = player->GetCycle();
		m_flSimulationTime = player->GetSimulationTime();
		m_nSequence = player->GetSequence();
		m_vecOrigin = player->GetVecOrigin();
		m_vecVelocity = player->GetVelocity();
		m_flPoseParameter = player->GetPoseParameter();
		headPos = player->GetBonePosition((int)Bone::BONE_HEAD);
		pelvisPos = player->GetBonePosition((int)Bone::BONE_PELVIS);
		absOrigin = player->GetAbsOriginal2();
		flags = player->GetFlags();
		priority = priorit;
		tickcount = correctTime;
		lbyUpdated = lby;
		matrixBuilt = player->SetupBones(matrix, 128, 256, m_flSimulationTime);
		pModel = player->GetModel();
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	explicit operator bool() const noexcept {
		return m_flSimulationTime > 0.f;
	}

	bool operator>(const CTickRecord& others) {
		return (m_flSimulationTime > others.m_flSimulationTime);
	}

	bool operator>=(const CTickRecord& others) {
		return (m_flSimulationTime >= others.m_flSimulationTime);
	}

	bool operator<(const CTickRecord& others) {
		return (m_flSimulationTime < others.m_flSimulationTime);
	}

	bool operator<=(const CTickRecord& others) {
		return (m_flSimulationTime <= others.m_flSimulationTime);
	}

	bool operator==(const CTickRecord& others) {
		return (m_flSimulationTime == others.m_flSimulationTime);
	}

	float m_flLowerBodyYawTarget = 0.f;
	Vector m_angEyeAngles = Vector(0, 0, 0);
	float m_flCycle = 0.f;
	float m_flSimulationTime = 0.f;
	int m_nSequence = 0;
	Vector m_vecOrigin = Vector(0, 0, 0);
	Vector m_vecVelocity = Vector(0, 0, 0);
	std::array<float, 24> m_flPoseParameter = {};
	Vector headPos = { 0, 0, 0 };
	Vector pelvisPos;
	Vector absOrigin;
	CValidTick2 validtick;
	int flags;
	int priority = 0;
	int tickcount = 0;
	matrix3x4_t	matrix[128];
	bool matrixBuilt;
	bool remove = false;
	bool lbyUpdated = false;
	model_t* pModel = nullptr;
	float calcDmg = 0.f;
	Vector calcPos = Vector(0, 0, 0);
	Vector calcBestPos = Vector(0, 0, 0);
	Vector mins = Vector(0, 0, 0);
	Vector maxs = Vector(0, 0, 0);
};

inline CValidTick2::operator CTickRecord() const {
	CTickRecord rec(m_pEntity, tickcount);
	rec.m_angEyeAngles.x = this->m_flPitch;
	rec.m_angEyeAngles.y = this->m_flYaw;
	rec.m_flSimulationTime = this->m_flSimulationTime;
	rec.tickcount = this->tickcount;
	return rec;
}