#include "hooks.h"

bool SetKeyCodeState::shouldListen = false;
ButtonCode_t* SetKeyCodeState::keyOutput = nullptr;

C_BasePlayer* Globals::pLocal = nullptr;
Vector Globals::eyePos = Vector(0, 0, 0);
Vector Globals::absOrigin = Vector(0, 0, 0);
float Globals::fixedCurTime = 0.f;
float Globals::oldCurTime = 0.f;
float Globals::fakeWalkTime = 0.f;
Vector Globals::unpredictedVelocity = Vector(0, 0, 0);

namespace Hooks
{
	HRESULT WINAPI hPresent(IDirect3DDevice9* pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion) {

		if (GetAsyncKeyState(VK_SNAPSHOT) || pEngine->IsTakingScreenshot()) {
			return Present(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		}

		DWORD dwOld_D3DRS_COLORWRITEENABLE;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);

		return Present(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}
	HRESULT WINAPI hReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static Reset_t oReset = D3D9Hook->GetOriginalFunction<Reset_t>(16);

		if (!renderer.IsReady() || !pDevice)
			return oReset(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();

		HRESULT result = oReset(pDevice, pPresentationParameters);

		ImGui_ImplDX9_CreateDeviceObjects();

		return result;
	}
	void __stdcall hPaintTraverse(unsigned int VGUIPanel, bool forcerepaint, bool allowforce)
	{
		//if (ESP::PrePaintTraverse(VGUIPanel, forcerepaint, allowforce))
		//	PanelHook->GetOriginalFunction<PaintTraverseFn>(42)(pPanel, VGUIPanel, forcerepaint, allowforce);
		//else

		if (Settings::ESP::enabled && Settings::NoScopeBorder::enabled && !strcmp("HudZoom", pPanel->GetName(VGUIPanel)))
			return;

		PanelHook->GetOriginalFunction<PaintTraverseFn>(41)(pPanel, VGUIPanel, forcerepaint, allowforce);
		static unsigned int drawPanel;

		if (!drawPanel)
			if (strstr(pPanel->GetName(VGUIPanel), "MatSystemTopPanel"))
				drawPanel = VGUIPanel;

		if (VGUIPanel != drawPanel)
			return;

		//Paint
		ESP::Paint();
		Info::Paint();
		EventLogger::Paint();
	}
	float GetCurTime(CUserCmd* ucmd) {
		//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());

		static int g_tick = 0;
		static CUserCmd* g_pLastCmd = nullptr;
		if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
			g_tick = Globals::pLocal->GetTickBase();
		}
		else {
			// Required because prediction only runs on frames, not ticks
			// So if your framerate goes below tickrate, m_nTickBase won't update every tick
			++g_tick;
		}
		g_pLastCmd = ucmd;
		float curtime = g_tick * pGlobalVars->interval_per_tick;
		return curtime;
	}
	bool __stdcall hCreateMove(float frametime, CUserCmd* cmd)
	{
		ClientModeHook->GetOriginalFunction<CreateMoveFn>(24)(pClientMode, frametime, cmd);
		if (cmd && cmd->command_number)
		{
			NameChanger::CreateMove(cmd);
			
			PDWORD pEBP;
			__asm mov pEBP, ebp;
			bool& bSendPacket = *(bool*)(*pEBP - 0x1C);

			bSendPacket = SendPacket;
			bSendPackett = bSendPacket;
			SendPacket = true;

			Globals::pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
			Globals::fixedCurTime = GetCurTime(cmd);

			BHop::CreateMove(cmd);

			//Vector wantedAngle = cmd->viewangles;

			//CreateMove
			Prediction::Start(cmd);
			BackTrack::CreateMove(cmd);
			AimBot::CreateMove(cmd);
			FakeLag::CreateMove(cmd);
			AntiAim::CreateMove(cmd);
			//Math::FixMovement(cmd, wantedAngle);

			/*
			Br0k3n
			for (int i = 0; i <= pEngine->GetMaxClients(); i++) {
				C_BasePlayer* pTarget = (C_BasePlayer*)pEntityList->GetClientEntity(i);

				if (!pTarget
					|| !pTarget->GetAlive()
					|| pTarget->GetDormant())
					continue;

				pTarget->pAnimFix(pGlobalVars->curtime);
			}*/

			Prediction::End();
		}
		else
			return ClientModeHook->GetOriginalFunction<CreateMoveFn>(24)(pClientMode, frametime, cmd);

		return false;
	}
	void __stdcall hDrawModelExecute(IMatRenderContext* matctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld)
	{
		//ModelRenderHook->UnhookFunction(21);
		//DME
		Chams::DrawModelExecute(matctx, state, pInfo, pCustomBoneToWorld);

		ModelRenderHook->GetOriginalFunction<DrawModelExecuteFn>(21)(pModelRender, matctx, state, pInfo, pCustomBoneToWorld);
		pModelRender->ForcedMaterialOverride(nullptr);
		//ModelRenderHook->HookFunction(hDrawModelExecute, 21);-
	}

	void __fastcall hFrameStageNotify(void* ecx, void* edx, ClientFrameStage_t stage)
	{
		// PVS Fix
		if (stage == ClientFrameStage_t::FRAME_RENDER_START)
		{
			for (int i = 1; i <= pGlobalVars->maxClients; i++)
			{
				if (i == pEngine->GetLocalPlayer()) continue;

				IClientEntity* pCurEntity = pEntityList->GetClientEntity(i);
				if (!pCurEntity) continue;

				*(int*)((uintptr_t)pCurEntity + 0xA30) = pGlobalVars->framecount; //we'll skip occlusion checks now
				*(int*)((uintptr_t)pCurEntity + 0xA28) = 0;//clear occlusion flags
			}
		}

		static bool Rekt = false;
		if (!Rekt)
		{
			ConVar* sv_cheats = pCvar->FindVar("sv_cheats");
			SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
			sv_cheats_spoofed->SetInt(1);

			ConVar* ST = pCvar->FindVar("r_DrawSpecificStaticProp");
			//	SpoofedConvar* sST = new SpoofedConvar(sv_cheats);
			//ST->
			ST->SetValue(0);

			Rekt = true;
		}

		//FSN
		Resolver::FrameStageNotify(stage);
		ThirdPerson::FrameStageNotify(stage);
		NightMode::FrameStageNotify(stage);
		NoSmoke::FrameStageNotify(stage);
		HitMarker::FrameStageNotify();
		//BackTrack::FrameStageNotify(stage);

		/*if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_END) {
			for (int i = 0; i <= 64; i++) {
				C_BasePlayer* pTarget = (C_BasePlayer*)pEntityList->GetClientEntity(i);

				if (!pTarget
					|| !pTarget->GetAlive()
					|| pTarget->GetDormant()
					|| !Globals::pLocal
					|| !Globals::pLocal->GetAlive()
					|| pTarget->GetTeam() == Globals::pLocal->GetTeam())
					continue;

				pTarget->set_interpolation_flags(0);
			}
		}*/

		ClientHook->GetOriginalFunction<FrameStageNotifyFn>(37)(ecx, stage); // 36
		//DrawModelExecute + PostFSN
	}
	HRESULT WINAPI hkEndScene(IDirect3DDevice9* device)
	{
		static EndScene_t oEndScene = D3D9Hook->GetOriginalFunction<EndScene_t>(42);

		/*DWORD dwOld_D3DRS_COLORWRITEENABLE;
		device->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);

		//MenuDraw
		Menu::Draw();

		device->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);*/

		//backup render states
		DWORD colorwrite, srgbwrite;
		device->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		device->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		//fix drawing without calling engine functons/cl_showpos
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		//draw here
		Menu::Draw();

		device->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		device->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);

		return oEndScene(device);
	}
	/*bool __fastcall hFireEventClientSide(void* ecx, void* edx, IGameEvent* pEvent)
	{
		//FireGameEvent
		//HitMarker::FireGameEvent(pEvent);
		//Resolver::FireGameEvent(pEvent);
		return FireEventHook->GetOriginalFunction<FireEventClientSideFn>(9)(ecx, pEvent);
	}*/
	void __stdcall hBeginFrame(float frameTime)
	{
		//NameChanger::BeginFrame(frameTime);
		return MaterialHook->GetOriginalFunction<BeginFrameFn>(42)(pMaterial, frameTime);
	}
	void __fastcall hEmitSound1(IEngineSound* thisptr, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, float iSoundlevel, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity)
	{
		if (strstr(pSample, "null"))
			pSample = "";

		return SoundHook->GetOriginalFunction<EmitSound1Fn>(5)(thisptr, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, iSoundlevel, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);
	}
	void __fastcall hEmitSound2(IEngineSound* thisptr, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, soundlevel_t flAttenuation, int nSeed, int iFlags, int iPitch, const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity)
	{
		if (strstr(pSample, "null"))
			pSample = "";
		
		return SoundHook->GetOriginalFunction<EmitSound2Fn>(6)(thisptr, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);
	}
	void __stdcall hPlaySounds(const char* fileName)
	{
		AutoAccept::PlaySound(fileName);
		SurfaceHook->GetOriginalFunction<PlaySoundFn>(82)(pSurface, fileName);
	}
	void __fastcall hRenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
	{		
		RenderViewHook->GetOriginalFunction<RenderViewFn>(6)(ecx, setup, hudViewSetup, nClearFlags, whatToDraw);
	}
	void __fastcall hOverrideView(void* _this, void* _edx, ViewSetup* setup)
	{
		FOVChanger::OverrideView(setup);
		
		ClientModeHook->GetOriginalFunction<OverreideViewFn>(18)(_this, setup);
	}
	float __stdcall hGetViewModelFOV()
	{
		float fov = ClientModeHook->GetOriginalFunction<GetViewModelFOVFn>(35)();

		FOVChanger::GetViewModelFOV(fov);

		return fov;
	}
	void SetKeyCodeState(ButtonCode_t code, bool bPressed) {
	#ifdef EXPERIMENTAL_SETTINGS
			if (inputSystem->IsButtonDown(ButtonCode_t::KEY_LALT) && code == ButtonCode_t::KEY_F && bPressed)
				Settings::ThirdPerson::enabled = !Settings::ThirdPerson::enabled;
	#endif
	}
	void Hooks::SetKeyCodeState(void* thisptr, ButtonCode_t code, bool bPressed) {
		if (SetKeyCodeState::shouldListen && bPressed) {
			SetKeyCodeState::shouldListen = false;
			*SetKeyCodeState::keyOutput = code;
			//UI::UpdateWeaponSettings();
		}

		if (!SetKeyCodeState::shouldListen)
			SetKeyCodeState(code, bPressed);

		pInputInternalHook->GetOriginalFunction<SetKeyCodeStateFn>(92)(thisptr, code, bPressed);
	}
	void Hooks::SetMouseCodeState(void* thisptr, ButtonCode_t code, MouseCodeState_t state) {
		if (SetKeyCodeState::shouldListen && state == MouseCodeState_t::BUTTON_PRESSED) {
			SetKeyCodeState::shouldListen = false;
			*SetKeyCodeState::keyOutput = code;
			//UI::UpdateWeaponSettings();
		}

		pInputInternalHook->GetOriginalFunction<SetMouseCodeStateFn>(93)(thisptr, code, state);
	}

	void __fastcall SceneEnd(void* thisptr, void* edx) {
		SceneEndHook->GetOriginalFunction<SceneEndFn>(9)(thisptr, edx);
		ThirdPerson::SceneEnd();
		//Chams::SceneEnd();
	}

	void __stdcall hLockCursor()
	{
		if (Menu::render) 
			pSurface->UnlockCursor();
		else
			SurfaceHook->GetOriginalFunction<LockCursorFn>(67)(pSurface);
	}
}
