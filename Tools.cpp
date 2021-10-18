#include "Tools.h"
void cTools::SaveClipboard(char* text)
{
	HGLOBAL global = GlobalAlloc(GMEM_FIXED, strlen(text) + 1); //text size + \0 character
	if (!global)return;
	memcpy(global, text, strlen(text));  //text size + \0 character
	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_TEXT, global);
		CloseClipboard();
	}
}

void cTools::WriteToMemory(DWORD64 addressToWrite, char* valueToWrite, int byteNum)
{
	unsigned long OldProtection;
	VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_WRITECOPY, &OldProtection);
	memcpy((LPVOID)(addressToWrite), valueToWrite, byteNum);
	VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, &OldProtection);
}

bool cTools::isMouseinRegion(float x1, float y1, float x2, float y2) {
	if (pPoint.x > x1 && pPoint.x < x2 && pPoint.y > y1 && pPoint.y < y2) {
		return true;
	}
	else {
		return false;
	}
	return true;
}


MODULEINFO cTools::GetModuleInfoX(int ScanFor) {
	MODULEINFO modinfo;
	HMODULE hModule = NULL;
	if (ScanFor == 0)hModule = reinterpret_cast<HMODULE>(GetModuleHandle(NULL));
	if (ScanFor == 4)hModule = reinterpret_cast<HMODULE>(GetModuleHandle(("MultiPlayerL.dll")));
	if (ScanFor == 3)hModule = reinterpret_cast<HMODULE>(GetModuleHandle(("medal-hook64.dll")));
	if (ScanFor == 2)hModule = reinterpret_cast<HMODULE>(GetModuleHandle(("graphics-hook64.dll")));
	if (ScanFor == 1)hModule = reinterpret_cast<HMODULE>(GetModuleHandle(("DiscordHook64.dll")));

	//if (hModule == 0)
	//	return modinfo;

	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}

DWORD cTools::FindPattern(char* pattern, char* mask, int ScanFor) {
	//Get all module related information
	MODULEINFO mInfo = (MODULEINFO)this->GetModuleInfoX(ScanFor);

	//Assign our base anaasd module size
	//Having the values right is ESSENTIAL, this makes sure
	//that we don't scan unwanted memory and leading our game to crash
	DWORD64 base = (DWORD64)mInfo.lpBaseOfDll;
	DWORD size = (DWORD)mInfo.SizeOfImage;

	//Get length for our mask, this will allow us to loop through our array
	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++)
	{
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++)
		{
			//if we have a ? in our mask then we have true by default, 
			//or if the bytes match then we keep searching until finding it or not
			if (mask[j] != '?' && pattern[j] != *((char*)base + i + j))
			{
				found = false;
				break;
			}
		}
		//found = true, our entire pattern was found
		//return the memory addy so we can write to it
		if (found)
		{
			return i;
		}
	}
	return true;
}

typedef struct _PEB_LDR_DATA {
	UINT8 _PADDING_[12];
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
#ifdef _WIN64
	UINT8 _PADDING_[24];
#else
	UINT8 _PADDING_[12];
#endif
	PEB_LDR_DATA* Ldr;
} PEB, * PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	VOID* DllBase;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _UNLINKED_MODULE
{
	HMODULE hModule;
	PLIST_ENTRY RealInLoadOrderLinks;
	PLIST_ENTRY RealInMemoryOrderLinks;
	PLIST_ENTRY RealInInitializationOrderLinks;
	PLDR_DATA_TABLE_ENTRY Entry;
} UNLINKED_MODULE;

#define UNLINK(x)					\
	(x).Flink->Blink = (x).Blink;	\
	(x).Blink->Flink = (x).Flink;

#define RELINK(x, real)			\
	(x).Flink->Blink = (real);	\
	(x).Blink->Flink = (real);	\
	(real)->Blink = (x).Blink;	\
	(real)->Flink = (x).Flink;

std::vector<UNLINKED_MODULE> UnlinkedModules;

struct FindModuleHandle
{
	HMODULE m_hModule;
	FindModuleHandle(HMODULE hModule) : m_hModule(hModule)
	{
	}
	bool operator() (UNLINKED_MODULE const& Module) const
	{
		return (Module.hModule == m_hModule);
	}
};

#include <algorithm>
void cTools::RelinkModuleToPEB(HMODULE hModule)
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));

	if (it == UnlinkedModules.end())
	{
		//DBGOUT(TEXT("Module Not Unlinked Yet!"));
		return;
	}

	RELINK((*it).Entry->InLoadOrderLinks, (*it).RealInLoadOrderLinks);
	RELINK((*it).Entry->InInitializationOrderLinks, (*it).RealInInitializationOrderLinks);
	RELINK((*it).Entry->InMemoryOrderLinks, (*it).RealInMemoryOrderLinks);
	UnlinkedModules.erase(it);
}

void cTools::UnlinkModuleFromPEB(HMODULE hModule)
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));
	if (it != UnlinkedModules.end())
	{
		//DBGOUT(TEXT("Module Already Unlinked!"));
		return;
	}

#ifdef _WIN64
	PPEB pPEB = (PPEB)__readgsqword(0x60);
#else
	PPEB pPEB = (PPEB)__readfsdword(0x30);
#endif

	PLIST_ENTRY CurrentEntry = pPEB->Ldr->InLoadOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY Current = NULL;

	while (CurrentEntry != &pPEB->Ldr->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		Current = CONTAINING_RECORD(CurrentEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (Current->DllBase == hModule)
		{
			UNLINKED_MODULE CurrentModule = { 0 };
			CurrentModule.hModule = hModule;
			CurrentModule.RealInLoadOrderLinks = Current->InLoadOrderLinks.Blink->Flink;
			CurrentModule.RealInInitializationOrderLinks = Current->InInitializationOrderLinks.Blink->Flink;
			CurrentModule.RealInMemoryOrderLinks = Current->InMemoryOrderLinks.Blink->Flink;
			CurrentModule.Entry = Current;
			UnlinkedModules.push_back(CurrentModule);

			UNLINK(Current->InLoadOrderLinks);
			UNLINK(Current->InInitializationOrderLinks);
			UNLINK(Current->InMemoryOrderLinks);

			break;
		}

		CurrentEntry = CurrentEntry->Flink;
	}
}

float cTools::Get3dDistance(Vector3 to, Vector3 from)
{
	return (float)((Vector3)(to - from)).Length();
}

std::wstring cTools::asciiDecode(const std::string& s)
{
	//std::wostringstream w;
	//wchar_t c;
	//for (size_t i = 0; i < s.length(); i++) {
	//	mbtowc(&c, &s[i], 1);
	//	w << c;
	//}
	//return w.str();
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//std::wstring wide = converter.from_bytes(s);
	//return wide.c_str();
	std::wstring wsTmp(s.begin(), s.end());
	return wsTmp;
}

//#ifdef _WIN64
//#define PTRMAXVAL (0x000F000000000000)
//#else
//#define PTRMAXVAL ((PVOID)0xFFF00000)
//#endif
BOOL cTools::IsValid(PVOID64 ptr)
{
	return (BOOL)(ptr != NULL);
}

float cTools::GetDistance(Vector2 From, Vector2 To)
{
	return (float)((Vector3)(To - From)).Length();
}

std::unique_ptr<cTools> Tools = NULL;