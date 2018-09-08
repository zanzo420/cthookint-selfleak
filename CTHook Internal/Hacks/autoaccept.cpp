#include "autoaccept.h"

bool Settings::AutoAccept::enabled = true;

void AutoAccept::PlaySound(const char* fileName) {
	if (!Settings::AutoAccept::enabled)
		return;

	if (pEngine->IsInGame())
		return;
						
	//static auto IsReady = reinterpret_cast<void(__cdecl*)()>((DWORD)(FindPattern("client.dll", "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x08\x56\x8B\x35\x00\x00\x00\x00\x57\x83\xBE", "xxxxxxxxxxxx????xxx")));
	//static auto beepSound = *reinterpret_cast<char**>((DWORD)FindPattern("client.dll", "\x68\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x8B\x01\xFF\x90\x00\x00\x00\x00\x89\xB7", "x????xx????xxxx????xx") + 1);
	//
	//if (fileName == beepSound)
	//	IsReady();
}