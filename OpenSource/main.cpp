#include <Windows.h>

void _declspec(naked) HPBarAsm()
{
	_asm
	{
		mov eax, [esp + 0x04]
		push esi
		add esi, 0x58
		mov esi, dword ptr[esi]
		cmp esi, 15
		ja pHPBarContinue

		//FirstTeam:
		cmp esi, 5
		ja SecondTeam
		mov esi, 0xFF32CD32
		jmp SetColor
	SecondTeam :
		cmp esi, 11
		ja Neutral
		mov esi, 0xFF8B0000
		jmp SetColor
	Neutral :
		cmp esi, 12
		jb SetColor
		mov esi, 0xFF1EB194

	SetColor :
		mov dword ptr[eax], esi

	pHPBarContinue :
		pop esi
		movzx edx, byte ptr[eax + 0x03]
		mov[ecx + 0x68], dl
		movzx edx, byte ptr[eax + 0x02]
		mov[ecx + 0x6B], dl
		movzx edx, byte ptr[eax + 0x01]
		mov[ecx + 0x6A], dl
		mov al, [eax]
		mov edx, [ecx]
		mov[ecx + 0x69], al
		mov eax, [edx + 0x24]
		call eax
		ret 0004
	}
}

bool Patch(LPVOID lpBaseAddress, LPVOID lpData, size_t nSize)
{
	if (!lpBaseAddress)
		return false;

	DWORD dwOldProtect;
	if (VirtualProtect(lpBaseAddress, nSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		memcpy(lpBaseAddress, lpData, nSize);
		VirtualProtect(lpBaseAddress, nSize, dwOldProtect, nullptr);

		return true;
	}

	return false;
}

bool Patch(uintptr_t nBaseAddress, DWORD dwBytes, size_t nSize)
{
	return Patch((LPVOID)nBaseAddress, (LPVOID)&dwBytes, nSize);
}

void Call(uintptr_t nBaseAddress, uintptr_t nProcAddress)
{
	Patch(nBaseAddress, 0xE8, 1);
	Patch(nBaseAddress + 1, nProcAddress - (nBaseAddress + 5), 4);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DWORD dwGameDll = (DWORD)GetModuleHandle("Game.dll");

		if (!dwGameDll)
			return TRUE;

		Call(dwGameDll + 0x364BF2, (int)HPBarAsm);
	}
	return TRUE;
}