#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include "DxINC\CommonStates.h"
#include "DxINC\DDSTextureLoader.h"
#include "DxINC\DirectXHelpers.h"
#include "DxINC\Effects.h"
#include "DxINC\GamePad.h"
#include "DxINC\GeometricPrimitive.h"
#include "DxINC\Model.h"
#include "DxINC\PrimitiveBatch.h"
#include "DxINC\ScreenGrab.h"
#include "DxINC\SimpleMath.h"
#include "DxINC\SpriteBatch.h"
#include "DxINC\SpriteFont.h"
#include "DxINC\VertexTypes.h"
#include "DxINC\WICTextureLoader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
class cTools {
public:
	Vector2 pPoint;
	MODULEINFO GetModuleInfoX(int ScanFor);
	DWORD FindPattern(char* pattern, char* mask, int ScanFor = 0);
	bool isMouseinRegion(float x1, float y1, float x2, float y2);
	void WriteToMemory(DWORD64 addressToWrite, char* valueToWrite, int byteNum);
	//const void* DetourFuncVTable(SIZE_T* src, const BYTE* dest, const DWORD index);
	//const unsigned int DisasmLengthCheck(const SIZE_T address, const unsigned int jumplength);
	//const void* DetourFunc64(BYTE* const src, const BYTE* dest, const unsigned int jumplength);
	void UnlinkModuleFromPEB(HMODULE hModule);
	void RelinkModuleToPEB(HMODULE hModule);
	void SaveClipboard(char* text);
	std::wstring asciiDecode(const std::string& s);
	float Get3dDistance(Vector3 to, Vector3 from);
	BOOL IsValid(PVOID64 ptr);
	float GetDistance(Vector2 From, Vector2 To);
};
extern std::unique_ptr<cTools> Tools;

