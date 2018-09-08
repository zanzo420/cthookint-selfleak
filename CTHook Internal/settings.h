#pragma once

#include <Windows.h>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "SDK/SDK.h"
#include "SDK\Definitions.h"
#include "ImGui\imgui.h"
#include "Util.h"
#include "Interfaces.h"

enum class AntiAimYType : int {
	BACKWARDS,
	LBY,
	FREESTANDING,
	RANDOMSPIN,
	MASTERLOOSER,
	AUTISM,
};

enum class AntiAimXType : int {
	DOWN,
	FAKEUP,
};

enum class AntiAimLBYType : int {
	REAL,
	FAKE,
	BACK,
};

enum class ClanTagType : int {
	CTHOOK,
	CTHOOKANIM,
};

enum class FakeLagType :int {
	LUNICO,
	KMETH,
	REACTIVE,
};

enum class WeaponType : int {
	NAME,
	CS16,
	CSGO,
};

class ColorVar
{
public:
	ImColor color;
	bool rainbow;
	float rainbowSpeed;

	ColorVar() {}

	ColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
	}

	ImColor Color()
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : this->color;
		result.Value.w = this->color.Value.w;
		return result;
	}
};

class HealthColorVar : public ColorVar
{
public:
	bool hp;

	HealthColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
		this->hp = false;
	}

	ImColor Color(C_BasePlayer* player)
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : (this->hp ? Color::ToImColor(Util::GetHealthColor(player)) : this->color);
		result.Value.w = this->color.Value.w;
		return result;
	}
};

namespace Settings {
	namespace UI
	{
		extern ColorVar mainColor;
		extern ColorVar bodyColor;
		extern ColorVar fontColor;

		namespace Fonts
		{
			namespace ESP
			{
				//extern char* family;
				//extern int size;
				//extern int flags;
			}
		}

		extern bool waterMark;

		namespace EventLogger {
			extern bool enabled;
			extern float fadeOut;
			// UNKNOWN, ITEMPURCHASE, OBJECTIVE, PLAYERHURT, RESOLVER
			extern bool filters[];
		}
	}

	namespace BHop {
		extern bool enabled;
		extern bool autoStrafe;
	}

	namespace Aimbot {
		extern bool enabled;
		extern bool silent;
		extern bool rcs;
		extern bool autoRevolver;
		extern bool friendly;
		extern bool autowall;
		extern float mindmg;
		extern bool hitboxes[];
		extern bool autoPistol;
		extern bool autoShoot;
		extern bool autoScope;
		extern bool forwardtrack;
		extern bool backtrack;
		extern float pointscale;
		extern bool multipoint;
		extern float pointscaleBaim;

		namespace HitChance {
			extern bool enabled;
			extern int hitRays;

			namespace value {
				extern float scout;
				extern float autoSniper;
				extern float awp;
				extern float r8;
				extern float other;
			}
		}
	}

	namespace HitMarker {
		extern bool enabled;
	}

	namespace Resolver {
		extern bool enabled;
		extern int baimAfterXShots;
		extern bool override;
		extern ButtonCode_t overrideKey;
	}

	namespace ESP {
		extern bool enabled;
		extern bool healthBar;
		extern bool ammobar;
		extern bool armorbar2;

		extern bool flag;
		extern bool health;
		extern bool resolverInfo;
		extern bool players;
		extern bool enemiesOnly;
		extern bool boxes;
		extern bool name;
		extern bool c4;
		extern bool plantedC4;
		extern bool nades;
		extern bool weapons;
		extern WeaponType weaponsType;
		extern bool bullettracer;
		extern bool droppedWeapons;
		extern WeaponType droppedWeaponsType;
		extern bool hostage;
		extern bool other;
		extern bool defuseKit;
		extern bool skeleton;
		extern bool drawLocal;
		extern bool chams;
		extern bool backtrack;
		extern bool forwardtrack;
		extern bool backtrackHitboxes;

		extern bool aaArrows;
		extern bool spreadCrosshair;
	}

	namespace AntiAim {
		namespace Yaw {
			extern bool enabled;
			extern AntiAimYType real;
			extern float realAdd;
			extern AntiAimYType fake;
			extern float fakeAdd;
			extern int spinSpeed;
			extern bool antiResolver;
		}

		namespace LBY {
			extern AntiAimLBYType type;
			extern float add;
			extern bool preBreak;
			extern AntiAimLBYType preBreakType;
			extern float preBreakAdd;
		}

		namespace AntiLastMove {
			extern bool enabled;
			extern AntiAimLBYType type;
			extern float add;
		}

		namespace Pitch {
			extern bool enabled;
			extern AntiAimXType pitch;
			extern bool flickUp;
		}

		namespace Roll {
			extern bool enabled;
			extern float angle;
		}

		extern bool antiUt;
	}

	namespace ThirdPerson {
		extern bool enabled;
		extern float distance;
		extern bool realAngles;
		extern bool fakeChams;
	}

	namespace FakeLag {
		extern bool enabled;
		extern bool manualChoke;
		extern int value;
		extern FakeLagType type;
	}

	namespace FakeWalk {
		extern bool enabled;
		extern ButtonCode_t key;
	}

	namespace ClanTag {
		extern bool enabled;
		extern ClanTagType type;
	}

	namespace AutoAccept {
		extern bool enabled;
	}

	namespace NightMode {
		extern bool enabled;
	}

	namespace NoSmoke {
		extern bool enabled;
	}

	namespace FOVChanger {
		extern bool enabled;
		extern bool ignoreScope;
		extern float value;

		extern bool viewmodelEnabled;
		extern float viewmodelValue;
	}

	namespace NoScopeBorder {
		extern bool enabled;
	}

	namespace Info {
		extern bool lbyBroken;
		extern bool lcBroken;
	}
}
