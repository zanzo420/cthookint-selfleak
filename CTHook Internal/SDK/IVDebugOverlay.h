#pragma once
namespace VMT
{
	class VMTHookManager
	{
	public:
		VMTHookManager() {}

		VMTHookManager(void* instance);
		~VMTHookManager();
		void Init(void* instance);
		void Restore();

		template <class T>
		T HookFunction(int index, void* func)
		{
			/// index is out of range or null function pointer
			if (index < 0 || index >= m_size || !func)
				return reinterpret_cast<T>(nullptr);

			/// get the original (this assumes that you dont try to hook the function twice)
			auto original = reinterpret_cast<T>(m_vtable[index]);
			m_original_funcs[index] = original;

			unsigned long old_protection, tmp;
			VProtect(&m_vtable[index], sizeof(void*), 0x40, &old_protection); /// 0x40 = PAGE_EXECUTE_READWRITE
			m_vtable[index] = func;
			VProtect(&m_vtable[index], sizeof(void*), old_protection, &tmp);

			return original;
		}
		void UnHookFunction(int index);

		template <class T>
		T GetOriginalFunction(int index)
		{
			/// index is out of range
			if (index < 0 || index >= m_size)
				return reinterpret_cast<T>(nullptr);

			/// function isn't even hooked - just return the one in the vtable
			if (m_original_funcs.find(index) == m_original_funcs.end())
				return reinterpret_cast<T>(m_vtable[index]);

			return reinterpret_cast<T>(m_original_funcs[index]);
		}

		template <class T>
		static T GetFunction(void* instance, int index)
		{
			const auto vtable = *static_cast<void***>(instance);
			return reinterpret_cast<T>(vtable[index]);
		}

	private:
		std::unordered_map<int, void*> m_original_funcs;
		void* m_instance = nullptr;
		void** m_vtable = nullptr;
		int m_size = 0;

	private:
		void VProtect(void* address, unsigned int size, unsigned long new_protect, unsigned long* old_protect);
	};

}
/*class IVDebugOverlay
{
public:
	bool ScreenPosition(const Vector& vIn, Vector& vOut)
	{
		typedef bool(__thiscall* oScreenPosition)(void*, const Vector&, Vector&);
		return getvfunc<oScreenPosition>(this, 13)(this, vIn, vOut);
	}
	void DrawPill(const Vector& mins, const Vector& max, float& diameter, int r, int g, int b, int a, float duration)
	{
		typedef void(__thiscall* OriginalFn)(void*, const Vector&, const Vector&, float&, int, int, int, int, float);
		getvfunc<OriginalFn>(this, 23)(this, mins, max, diameter, r, g, b, a, duration);
	}
	void AddCapsuleOverlay(Vector& mins, Vector& maxs, float pillradius, int r, int g, int b, int a, float duration)
	{
		VMT::VMTHookManager::GetFunction<void(__thiscall*)(void*, Vector&, Vector&, float&, int, int, int, int, float)>(this, 24)(this, mins, maxs, pillradius, r, g, b, a, duration);
	}
};*/

class OverlayText_t;

class IVDebugOverlay
{
public:
	virtual void            AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
	virtual void            AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void            AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void            AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void            __unkn() = 0;
	virtual void            AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void            AddTextOverlay(const Vector& origin, float duration, const char *format, ...) = 0;
	virtual void            AddTextOverlay(const Vector& origin, int line_offset, float duration, const char *format, ...) = 0;
	virtual void            AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void            AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void            AddGridOverlay(const Vector& origin) = 0;
	virtual void            AddCoordFrameOverlay(const matrix3x4_t& frame, float flScale, int vColorTable[3][3] = NULL) = 0;
	virtual int             ScreenPosition(const Vector& point, Vector& screen) = 0;
	virtual int             ScreenPosition(float flXPos, float flYPos, Vector& screen) = 0;
	virtual OverlayText_t*  GetFirst(void) = 0;
	virtual OverlayText_t*  GetNext(OverlayText_t *current) = 0;
	virtual void            ClearDeadOverlays(void) = 0;
	virtual void            ClearAllOverlays() = 0;
	virtual void            AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ...) = 0;
	virtual void            AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
	virtual void            AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void            AddBoxOverlay2(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, const uint8_t* faceColor, const uint8_t* edgeColor, float duration) = 0;
	virtual void            PurgeTextOverlays() = 0;
	virtual void            DrawPill(const Vector& mins, const Vector& max, float& diameter, int r, int g, int b, int a, float duration) = 0;
	void AddCapsuleOverlay(Vector& mins, Vector& maxs, float pillradius, int r, int g, int b, int a, float duration)
	{
		getvfunc<void(__thiscall*)(void*, Vector&, Vector&, float&, int, int, int, int, float)>(this, 24)(this, mins, maxs, pillradius, r, g, b, a, duration);
	}

private:
	inline void AddTextOverlay(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) {} /* catch improper use of bad interface. Needed because '0' duration can be resolved by compiler to NULL format string (i.e., compiles but calls wrong function) */
};