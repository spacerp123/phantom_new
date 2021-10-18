#include "D3D11Renderer.h"
#include "Tools.h"
#include "Menu.h"
#include "Game.h"

DWORD WINAPI dwddwdw(LPVOID lpcst);
std::unique_ptr<D3DMenu> pMenu = NULL;
std::unique_ptr<Options> Hacks = NULL;

using namespace std::chrono;
using OriginalFn = int(*)(void*, void*, bool);
OriginalFn oReturn;

typedef DWORD64(__fastcall* OriginalFnGame)(DWORD64 a1);
OriginalFnGame oOriginalFnGame = NULL;

typedef HRESULT(__stdcall* OriginalFnPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
OriginalFnPresent oReturnPresent = NULL;

HWND TopWindow = NULL;

HWND ForGroundWindow = NULL;
bool Player_Hacks = false;
bool Vehicle_Menu = false;
bool Npc_Menu = false;
bool Weapon_Menu = false;
bool Aimbot_Menu = false;
bool Radar_Menu = false;
bool Other_Menu = false;
bool Players_Menu = false;
bool Water_Menu = false;
bool Save = false;
bool Load = false;
bool Save2 = false;
bool Load2 = false;
bool Save3 = false;
bool Load3 = false;

uintptr_t* pSwapChainVtable = NULL;
uintptr_t* pDeviceContextVTable = NULL;
std::unique_ptr<SpriteFont> m_font = NULL;
std::unique_ptr<SpriteBatch> DX11SpriteBatch = NULL;
std::unique_ptr<BasicEffect> DX11BatchEffects = NULL;
std::unique_ptr<PrimitiveBatch<VertexPositionColor>> DX11Batch = NULL;

using VertexType = DirectX::VertexPositionColor;

std::unique_ptr<DirectX::CommonStates> m_states;
std::unique_ptr<DirectX::BasicEffect> m_effect;
std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

#include "Font.h"
void DrawHelthEsp(float w_health, CObject* tmpObject, Vector3 Top2d, float BoxWidth, float BoxHight, bool Side = 0) {
	if (!Side) {
		renderer->DrawHealthBar(Top2d.x - (BoxWidth > w_health ? BoxWidth / 2.0f : (w_health / 2)), Top2d.y - 8.0f, (BoxWidth > w_health ? BoxWidth : w_health), 3, tmpObject->HP - 100.0f, tmpObject->MaxHP - 100.0f, 0);
		if (tmpObject->GetArmour() > 0.0f)renderer->DrawArmorBar(Top2d.x - (BoxWidth > w_health ? BoxWidth / 2.0f : (w_health / 2.0f)), Top2d.y - 16.0f, (BoxWidth > w_health ? BoxWidth : w_health), 3.0f, tmpObject->GetArmour(), 100.0f);
	}
	else {
		renderer->DrawHealthBar(Top2d.x + (BoxWidth / 2) + 4, Top2d.y + 1.0f, 3.0f, BoxHight - 1.0f, tmpObject->HP - 100.0f, tmpObject->MaxHP - 100.0f, 1);
		if (tmpObject->GetArmour() > 0.0f)renderer->DrawArmorBar(Top2d.x + (BoxWidth / 2.0f) + 10.0f, Top2d.y + 1.0f, 3.0f, BoxHight - 1.0f, tmpObject->GetArmour(), 100.0f, 1);
	}
}
bool IsVisPedHead[257];
static __int32 iTeamId[257];
static __int32 LocaliTeamId = 0;
bool EnableFlyRoutineHotKey = false;
bool EnableNoClipRoutineHotKey = false;
static bool TeleportToPlayer = false;
static Vector3 TeleportToPlayerPos(0.0f, 0.0f, 0.0f);
static bool KillPlayerTarget = false;
static bool ResetSpectatePlayer = false;
static bool KillPlayerTargetSilent = false;
static Vector3 PlayerTargetPos;
static Vector3 KillTargetSilentPos;
static string FreezeTarget[257];
static string SpecateTarget[257];
static bool AimOnTarget = false;
static bool pTeleportToWaypointHotKey = false;
static bool VK_RBUTTON_PRESS = false;
static bool VK_ResetHealth_PRESS = false;
static bool VK_ResetArmour_PRESS = false;
static bool VK_FlipVeh_PRESS = false;
static bool VK_Delete_PRESS = false;
const std::string MEP0 = "Animal";
const std::string MEP1 = "Male";
const std::string MEP2 = "Female";
const std::string MEP3 = "Cop";
const std::string MEP4 = "Swat";
const std::string MEP5 = "Army";
const std::string MEP6 = "Fireman";
const std::string sDistance = "%1.0fm";
const std::string sTeleportTo = "[Teleport To]";
const std::string sRemoteKill = "[Remote Kill]";
Vector3 BoneSpeed(0.0f, 0.0f, 0.0f);
static CObject* AimCrossTarget = NULL;
static CObject* AimCrossTargetLoop = NULL;
CObject* StealVehTemp = NULL;
CObject* StealVehTempPassanger = NULL;

std::string repeat(const std::string& input, unsigned num)
{
	std::string ret;
	ret.reserve(input.size() * num);
	while (num--)
		ret += input;
	return ret;
}
int PlayerCount = 0;

string removeZero(string str) {
	int i = 0;
	while (str[i] == '0')
		i++;
	str.erase(0, i);
	return str;
}

std::string Names[255];
CObject* NamesPedObj[256];
string LocalPlayerName = "";

void DrawPeds() {
	float curdistance = 250.0f;
	static float distance = 0.0f;
	int NpcCount = 0;
	PlayerCount = 0;
	float curdistanceCross = Hacks->Aimbot_Fov;
	string NameEsp = "None";
	static Color ColorEsp;

	for (int i = 0; i < 255; i++) {
		if (!Hacks->UseRayCast)IsVisPedHead[i] = true;
		CObject* tmpObject = addy.ReplayInterface->ped_interface->get_ped(i);

		if (Tools->IsValid(tmpObject) && Tools->IsValid(tmpObject->_ObjectNavigation)) {

			if (tmpObject == addy.localPlayer && !Hacks->LocalPlayerDisplay)
				continue;

			Vector3 Top2d = Vector3(0.0f, 0.0f, 0.0f), Bottom2d = Vector3(0.0f, 0.0f, 0.0f);
			float BoxHight = 0.0f, BoxWidth = 0.0f;
			float pDistance = Tools->Get3dDistance(tmpObject->fPosition, addy.localPlayer->fPosition);

			if (tmpObject->HP <= 100.0f)
				continue;

			if (pDistance > Hacks->Npc_Max_Distance) //Check for Max Distance
				continue;

			//if (!Tools->IsValid(tmpObject->gPlayerInfo())) {
			if (!Hacks->ClassicGangWarMode) {
				if (tmpObject->PedType() == ePedType::PEDTYPE_ANIMAL)
					ColorEsp = !IsVisPedHead[i] ? Colors::YellowGreen.v : Colors::Yellow.v, NameEsp.assign(MEP0.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_CIVMALE)
					ColorEsp = !IsVisPedHead[i] ? Colors::Green.v : Colors::Lime.v, NameEsp.assign(MEP1.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_CIVFEMALE)
					ColorEsp = !IsVisPedHead[i] ? Colors::Green.v : Colors::Lime.v, NameEsp.assign(MEP2.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_COP)
					ColorEsp = !IsVisPedHead[i] ? Colors::Blue.v : Colors::Cyan.v, NameEsp.assign(MEP3.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_SWAT)
					ColorEsp = !IsVisPedHead[i] ? Colors::DarkViolet.v : Colors::Magenta.v, NameEsp.assign(MEP4.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_ARMY)
					ColorEsp = !IsVisPedHead[i] ? Colors::BlueViolet.v : Colors::Violet.v, NameEsp.assign(MEP5.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_FIREMAN)
					ColorEsp = !IsVisPedHead[i] ? Colors::Red.v : Colors::PaleVioletRed.v, NameEsp.assign(MEP6.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_PROSTITUTE)
					ColorEsp = !IsVisPedHead[i] ? Colors::Pink.v : Colors::LightPink.v, NameEsp.assign(MEP6.c_str());
			}
			else {
				if (tmpObject->PedType() == ePedType::PEDTYPE_CIVMALE)
					ColorEsp = !IsVisPedHead[i] ? Colors::Green.v : Colors::Lime.v, NameEsp.assign(MEP1.c_str());
				if (tmpObject->PedType() == ePedType::PEDTYPE_CIVFEMALE)
					ColorEsp = !IsVisPedHead[i] ? Colors::Green.v : Colors::Lime.v, NameEsp.assign(MEP2.c_str());
			}

			if (!tmpObject->IsVisible())ColorEsp = Colors::White;

			Vector3 Head2d(0.0f, 0.0f, 0.0f);
			Vector3 AimBone3D(0.0f, 0.0f, 0.0f);

			if (Hacks->Aimbot_Bone == 0 || Hacks->Aimbot_Bone == 1)AimBone3D = NPC_Bones[i][pSKEL_Head];
			if (Hacks->Aimbot_Bone == 2)AimBone3D = NPC_Bones[i][pSKEL_Neck_1];
			if (Hacks->Aimbot_Bone == 3)AimBone3D = NPC_Bones[i][pSTOMACH];

			bool FoundAimBone = W2S(AimBone3D, Head2d);
			float Distance = Tools->Get3dDistance(ToVector3(pLocalCamPos), AimBone3D);

			float CrossHairDistance = Tools->GetDistance(Vector2(Head2d.x, Head2d.y), Vector2(ScreenCenterX, ScreenCenterY));
			if (CrossHairDistance < Hacks->Aimbot_Fov && Hacks->Aimbot_Toggle > 0 && tmpObject == AimCrossTarget && tmpObject->HP >= 10.0f && IsVisPedHead[i] && tmpObject->IsVisible255() && (AimOnTarget || Hacks->Aimbot_Toggle == 1) && (Hacks->Aimbot_Mode > 0 || Hacks->Aimbot_Silent))ColorEsp = Colors::DarkMagenta;

			Vector3 Top = Vector3(tmpObject->fPosition.x, tmpObject->fPosition.y, tmpObject->fPosition.z + 0.85f);
			Vector3 Bottom = Vector3(tmpObject->fPosition.x, tmpObject->fPosition.y, tmpObject->fPosition.z - 1.0f);

			if ((Hacks->TargetSelectionRadar == 1 ? !Tools->IsValid(tmpObject->gPlayerInfo()) : Hacks->TargetSelectionRadar == 2 ? Tools->IsValid(tmpObject->gPlayerInfo()) : Hacks->TargetSelectionRadar == 3 ? true : false)) {

				if (Hacks->Toggle_2D_Radar == 1) {
					float RadarSize = (125.0f + (Hacks->RadarSize * 1.0f) * 25.0f);
					float RadarZoom = (Hacks->RadarZoom * 1.0f) * 0.2f;

					Vector2 RadarPos = WorldToRadar(tmpObject->fPosition, Hacks->RadarLoc.x, Hacks->RadarLoc.y, RadarSize, RadarZoom);
					renderer->DrawDot(RadarPos.x, RadarPos.y, 4.0f, 4.0f, ColorEsp, true);
					if (Hacks->Health2dRadar)renderer->DrawHealthBar(RadarPos.x - 10.0f, RadarPos.y + 5.0f, 20.0f, 3.0f, tmpObject->HP - 100.0f, tmpObject->MaxHP - 100.0f, Hacks->Engine_Draw);
				}
			}
			if ((Hacks->TargetSelection == 1 ? !Tools->IsValid(tmpObject->gPlayerInfo()) : Hacks->TargetSelection == 2 ? Tools->IsValid(tmpObject->gPlayerInfo()) : Hacks->TargetSelection == 3 ? true : false)) {
				if (W2S(Top, Top2d) && W2S(Bottom, Bottom2d) && tmpObject->fPosition.x != 0.0f) {
					BoxHight = fabsf(Top2d.y - Bottom2d.y);
					BoxWidth = (BoxHight / 2.0f);

					if (Hacks->RageNames) {
						for (int iFriends = 0; iFriends < Hacks->ArrayFriends.size(); iFriends++)
							if (Names[i]._Equal(Hacks->ArrayFriends[iFriends]))
								ColorEsp = Colors::OrangeRed;

						if (!pMenu->visible && Hacks->Npc_Name_Esp)renderer->Draw2dText(Top2d.x, Top2d.y - (tmpObject->GetArmour() < 1.0f ? 18.0f : 24.0f), ColorEsp, Hacks->Esp_Font_Size, true, Names[i].c_str());
						//if (!pMenu->visible && Hacks->Npc_Distance_Esp)renderer->Draw2dText(Top2d.x, Top2d.y - (tmpObject->GetArmour() < 1.0f ? 18.0f : 24.0f), ColorEsp, Hacks->Esp_Font_Size, true, "[%p]", NamesPedObj[i]);
					}
					else {
						if (Tools->IsValid(tmpObject->gPlayerInfo()))
							if (!pMenu->visible && Hacks->Npc_Name_Esp)renderer->Draw2dText(Top2d.x, Top2d.y - (tmpObject->GetArmour() < 1.0f ? 18.0f : 24.0f), ColorEsp, Hacks->Esp_Font_Size, true, tmpObject->gPlayerInfo()->GetPlayerName2().c_str());

						/*if (Hacks->Npc_Name_Esp && (!pMenu->visible || (pMenu->visible && Players_Menu))) {
							if (Hacks->Npc_Health_Esp == 1)renderer->Draw2dText(Top2d.x, Top2d.y - (tmpObject->GetArmour() < 1.0f ? 18.0f : 24.0f), ColorEsp, Hacks->Esp_Font_Size, true, "%s", Hacks->Npc_Name_Esp == 1 ? NameEsp : sNameBuf.c_str());
							if (Hacks->Npc_Health_Esp == 0 || Hacks->Npc_Health_Esp == 2)renderer->Draw2dText(Top2d.x, Top2d.y - 8.0f, ColorEsp, Hacks->Esp_Font_Size, true, "%s", Hacks->Npc_Name_Esp == 1 ? NameEsp : sNameBuf.c_str());
							if (Tools->IsValid(tmpObject->gPlayerInfo()) && Hacks->Npc_Wanted_Esp)renderer->Draw2dText(Top2d.x - 0.65f, Top2d.y - (Hacks->Npc_Health_Esp == 1 ? (tmpObject->GetArmour() < 1.0f ? 24.0f : 32.0f) : 17.0f), ColorEsp, Hacks->Esp_Font_Size + 0.4f, true, repeat("*", tmpObject->gPlayerInfo()->GetWanted()).c_str());
						}*/
					}

					if (Hacks->Npc_HeadDot_Esp == 1 && BoxWidth > 2.0f && FoundAimBone)renderer->DrawDot(Head2d.x, Head2d.y, Hacks->Npc_HeadDot_Size, Hacks->Npc_HeadDot_Size, ColorEsp, true);
					if (Hacks->Npc_Barrel_Esp)
						Barrel(AimBone3D, tmpObject->_ObjectNavigation->Rotation, ColorEsp, 5.0f);
					if (Hacks->Npc_Bone_Esp)BoneEsp(i, ColorEsp);
					float w_health = 50.0f;
					if (Hacks->Npc_Box_Esp_Type == 2 && BoxWidth > 2.0f)renderer->Draw3DBox(tmpObject->fPosition, tmpObject->_ObjectNavigation->Rotation, ColorEsp); //3D Npc Box Esp
					if (Hacks->Npc_Box_Esp_Type == 3 && BoxWidth > 2.0f)renderer->FillRect(Top2d.x - (BoxWidth / 2.0f), Top2d.y, BoxWidth, BoxHight, Color(ColorEsp.R(), ColorEsp.G(), ColorEsp.B(), 0.275f)); //Fill Npc Box Esp
					if (Hacks->Npc_Box_Esp_Type == 1 && BoxWidth > 2.0f)DrawBoundingBox(Top2d/*, Bottom2d*/, BoxHight, ColorEsp, Hacks->Npc_Line_Size); //2D Npc Box Esp

					if (Hacks->Npc_Health_Esp == 1 || Hacks->Npc_Health_Esp == 2)
						DrawHelthEsp(w_health, tmpObject, Top2d, BoxWidth, BoxHight, Hacks->Npc_Health_Esp == 1 ? false : true); //Top Health

					if (Hacks->Npc_Health_Esp == 3 && !pMenu->visible)
						renderer->Draw2dText(Top2d.x, Top2d.y - 8.0f, ColorEsp, Hacks->Esp_Font_Size, true, "%1.0f ~ %1.0f", tmpObject->HP, tmpObject->GetArmour());

					if (Hacks->Npc_Weapon_Esp && Tools->IsValid(tmpObject->gWeaponManager()) && Tools->IsValid(tmpObject->gWeaponManager()->_WeaponInfo) && !pMenu->visible)
						renderer->Draw2dText(Top2d.x, Bottom2d.y + 7.5f, ColorEsp, Hacks->Esp_Font_Size, true, tmpObject->gWeaponManager()->_WeaponInfo->GetSzWeaponName().c_str());

					if (Hacks->Npc_Distance_Esp && !pMenu->visible)
						renderer->Draw2dText(Top2d.x, Bottom2d.y + (Hacks->Npc_Weapon_Esp ? 17.5f : 7.5f), ColorEsp, Hacks->Esp_Font_Size, true, "%1.0fm", pDistance);
				}
			}
		}
	}
}

void FPSCheck(std::string& str) {
	static int FpsCount = 0;
	static high_resolution_clock::time_point lastTime;
	static int ticks = 0;
	auto now = high_resolution_clock::now();
	auto secs = duration_cast<seconds>(now - lastTime);
	ticks++;
	if (secs.count() >= 1)
	{
		str = std::to_string((ticks / secs.count()) / 2);
		FpsCount = (ticks / (int)secs.count()) / 2;
		ticks = 0;
		lastTime = now;
	}
}

void DrawFpsAndCpu() {
	static string Fps = "";
	FPSCheck(Fps);
	renderer->Draw2dText(10.0f, 12.0f, Colors::Red.v, 0.55f, false, "D3D11 FPS: %s", Fps.c_str()); //Draw Fps
}

DWORD64 GameThread_Pattern = 0;
float MouseWheel = 0.0f;
WNDPROC OldWndProc = NULL;
bool RapideFireKey = false;
bool VK_Shift_PRESS = false;
bool handle_unload = false;

LPARAM aKeyPressed = NULL;

LRESULT WINAPI HookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_MOUSEMOVE:
		Tools->pPoint.x = (float)(short)LOWORD(lParam);// horizontal position 
		Tools->pPoint.y = (float)(short)HIWORD(lParam);// vertical position 
		break;
	case WM_LBUTTONDOWN:
		GetlButton = 1;
		RapideFireKey = 1;
		break;
	case WM_MOUSEWHEEL:
		MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1.0f : +1.0f;
		break;
	case WM_LBUTTONUP:
		GetlButton = 0;
		RapideFireKey = 0;
		break;
	case WM_RBUTTONUP:
		aKeyPressed = NULL;
		break;
	case WM_RBUTTONDOWN:
		aKeyPressed = wParam;
		break;
	case WM_SYSKEYUP:
		aKeyPressed = NULL;
		break;
	case WM_SYSKEYDOWN:
		aKeyPressed = wParam;
		break;
	case WM_XBUTTONUP:
		aKeyPressed = NULL;
		break;
	case WM_XBUTTONDOWN:
		aKeyPressed = GET_XBUTTON_WPARAM(wParam);
		break;
	case WM_KEYUP:
		aKeyPressed = NULL;
		break;
	case WM_KEYDOWN:
		aKeyPressed = wParam;
		if (wParam == Hacks->OpenMenuKey) {
			pMenu->visible = !pMenu->visible;
			return 0L;
		}
		if (wParam == VK_F12) {
			handle_unload = true;
			return 0L;
		}
		break;
	}
	if (GetKeyState(Hacks->HotKeyVehImpulseIncreace) & 0x8000)
		Key_VK_ADD = true;
	else
		Key_VK_ADD = false;

	if (GetKeyState(Hacks->HotKeyVehImpulseDecrease) & 0x8000)
		Key_VK_SUBTRACT = true;
	else
		Key_VK_SUBTRACT = false;

	if (GetKeyState(Hacks->HotKeyVehImpulseConstant) & 0x8000)
		Key_VK_MULTIPLY = true;
	else
		Key_VK_MULTIPLY = false;

	if (GetKeyState(Hacks->HotKeyFly) & 0x8000)
		EnableFlyRoutineHotKey = true;
	else
		EnableFlyRoutineHotKey = false;

	if (GetKeyState(Hacks->HotKeyNoClip) & 0x8000)
		EnableNoClipRoutineHotKey = true;
	else
		EnableNoClipRoutineHotKey = false;

	if (GetKeyState(Hacks->HotKeyAim) & 0x8000 || GetKeyState(Hacks->HotKeyAim2) & 0x8000)
		AimOnTarget = true;
	else
		AimOnTarget = false;

	if (GetKeyState(Hacks->HotKeyResetHealth) & 0x8000)
		VK_ResetHealth_PRESS = true;
	else
		VK_ResetHealth_PRESS = false;

	if (GetKeyState(Hacks->HotKeyResetArmour) & 0x8000)
		VK_ResetArmour_PRESS = true;
	else
		VK_ResetArmour_PRESS = false;

	if (GetKeyState(VK_LSHIFT) & 0x8000)
		VK_Shift_PRESS = true;
	else
		VK_Shift_PRESS = false;

	if (pMenu->visible && ForGroundWindow == GtaHWND)return true;
	return CallWindowProc(OldWndProc, hWnd, uMsg, wParam, lParam);
}

bool Reset_Health = false;
bool Reset_Armor = false;
bool Take_Sucide = false;
bool GiveCoustumWeaponB = false;
bool GiveWeaponB = false;
bool SetPedAmmo = false;
bool FixCar = false;
bool UpgradeCar = false;
bool TpToMe = false;
bool StartEngine = false;
bool SpawnVeh = false;
bool TpInNearVeh_Passanger = false;
bool FlipVeh = false;
bool TpToWay = false;
bool SpawnVeh1 = false;
bool SpawnVeh2 = false;
bool SaveCurrentPosVeh = false;
Vector3 SavedPosForVehs(0.0f, 0.0f, 0.0f);
bool EnterNearVeh = false;
bool FixNearVeh = false;
bool Shift_KeyO = false;
DWORD64 FlyPatt = 0x0;
static DWORD64 CTickRapidF = 0x0;
static DWORD64 LTickRapidF = 0x0;
Vector3 LocalPlayerCamLoc(0.0f, 0.0f, 0.0f);

DWORD LocalPlayerHandleFP = NULL;


void onTickInit() {
	DWORD64 CurTime = GetTickCount64();
	if (Tools->IsValid(addy.localPlayer) && Tools->IsValid(addy.ReplayInterface) && Tools->IsValid(addy.ReplayInterface->ped_interface)) {

		if (VK_ResetHealth_PRESS) {
			addy.localPlayer->ResetHealth();
		}
		if (VK_ResetArmour_PRESS) {
			addy.localPlayer->ResetArmour();
		}

		addy.localPlayer->NoPlayerCollision(Hacks->NoPlayerCollision);

		if (Hacks->NoRagdoll) {
			addy.localPlayer->NoRagDoll();
		}

		if (Take_Sucide) {
			addy.localPlayer->HP = -0.0f;
			Take_Sucide = false;
		}

		if (Tools->IsValid(GetGameplayCamCoord))GetGameplayCamCoord(&pLocalCamPos);
		if (Hacks->NoClip) {
			CLEAR_PED_TASKS(LocalPlayerHandle);
			oCLEAR_PED_TASKS_IMMEDIATELY(LocalPlayerHandle);
			NoClip(Hacks->FlySpeed);
		}

		if (Hacks->FlyHack && !Hacks->NoClip) {
			if (EnableFlyRoutineHotKey) {
				if (*(BYTE*)(FlyPatt) != 0xEB)
					*(BYTE*)(FlyPatt) = 0xEB;
				//pSetEntityVelocity(LocalPlayerHandle, &ToPVector3(transformRotToDir(ToVector3(CamRotLocal)) * (Hacks->FlySpeed * 10.0f)));
				addy.localPlayer->v3Velocity = CamRotLocal * (Hacks->FlySpeed * 10.0f);
			}
			else {
				if (*(BYTE*)(FlyPatt) != 0x74)
					*(BYTE*)(FlyPatt) = 0x74;
			}
		}
		static DWORD64 CTickOthers = 0x0;
		static DWORD64 LTickOthers = 0x0;

		if ((CTickOthers - LTickOthers) > 500) {

			if (Tools->IsValid(StealVehTemp)) {
				DWORD StealHandle = centity_to_handle(StealVehTemp);
				pNetworkRequestControlOfEntity(StealHandle);
				CLEAR_PED_TASKS(StealHandle);
				oCLEAR_PED_TASKS_IMMEDIATELY(StealHandle);
				if (Tools->IsValid(StealVehTemp->gVehicleManager()))SET_PED_INTO_VEH(addy.localPlayer, StealVehTemp->gVehicleManager(), 0, 1);
				StealVehTemp = NULL;
			}

			if (Tools->IsValid(StealVehTempPassanger)) {
				if (Tools->IsValid(StealVehTempPassanger->gVehicleManager()))SET_PED_INTO_VEH(addy.localPlayer, StealVehTempPassanger->gVehicleManager(), 1, 1);
				StealVehTempPassanger = NULL;
			}


			if (!Hacks->Save_Mode) {
				if (Remove_All)RemoveAllWeapons(), Remove_All = false;
				if (Remove_InHand && Tools->IsValid(addy.localPlayer->gWeaponManager()))
					RemoveWeaponFromPed(LocalPlayerHandle, addy.localPlayer->gWeaponManager()->GetHash()), Remove_InHand = false;
				if (GiveWeaponB)GiveAllWeapons(), GiveWeaponB = false;
				if (GiveCoustumWeaponB)GiveSelectedWeapons(), GiveCoustumWeaponB = false;
				if (SetPedAmmo)vSetPedAmmo(Hacks->Set_Ammo), SetPedAmmo = false;
			}

			if (Tools->IsValid(addy.localPlayer->gVehicleManager())) {
				if (Hacks->FixOnDmg) {
					if (addy.localPlayer->gVehicleManager()->fHealth < addy.localPlayer->gVehicleManager()->fHealthMax - 10.0f) {
						//DWORD v = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
						DWORD v = centity_to_handle_veh(addy.localPlayer->gVehicleManager());
						//if (pNetworkRequestControlOfEntity(v)) {
						pSetVehicleFixed(v);
						//pSetVehicleOnGroundProperly(v, 0);
					//}
					}
				}

				if (VK_FlipVeh_PRESS) {
					DWORD v = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
					if (pNetworkRequestControlOfEntity(v)) {
						pSetVehicleOnGroundProperly(v, 0);
					}
				}

				if (FlipVeh) {
					//DWORD v = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
					DWORD v = centity_to_handle_veh(addy.localPlayer->gVehicleManager());
					if (pNetworkRequestControlOfEntity(v)) {
						pSetVehicleOnGroundProperly(v, 0);
					}
					FlipVeh = false;
				}


				if (FixCar) {
					DWORD v = centity_to_handle_veh(addy.localPlayer->gVehicleManager());
					if (pNetworkRequestControlOfEntity(v)) {
						if (addy.localPlayer->gVehicleManager()->fHealth < addy.localPlayer->gVehicleManager()->fHealthMax) {
							pSetVehicleFixed(v);
							pSetVehicleOnGroundProperly(v, 0);
							FixCar = false;
						}
					}
				}

				if (StartEngine) {
					if (Tools->IsValid(addy.localPlayer->gVehicleManager())) {
						DWORD HandleOveh = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
						if (pNetworkRequestControlOfEntity(HandleOveh)) {
							pSetVehicleEngineOn(HandleOveh, true, true, true);
							StartEngine = false;
						}
						else {
							pSetVehicleEngineOn(HandleOveh, true, true, true);
							StartEngine = false;
						}
					}
				}

				if (UpgradeCar) {
					DWORD v = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
					if (pNetworkRequestControlOfEntity(v)) {
						//pDecorSetInt(v, "MPBitset", (1 << 10));
						pSetVehicleModKit(v, 0);
						pToggleVehicleMod(v, VehicleModTypes::MOD_TURBO, true);
						pToggleVehicleMod(v, VehicleModTypes::MOD_XENONLIGHTS, true);
						pSetVehicleMod(v, 0, pGetNumVehicleMod(v, 0) - 2, false);		//biggest spoiler = ugly
						for (int i = 1; i < 0x30; ++i)
						{
							if (i > 22 && i < 25)
							{
								//if (wheels)
								pSetVehicleMod(v, i, WHEEL_HIGHEND_CARBONSHADOW, false);	//WHEEL_HIGHEND_CARBONSHADOW
								continue;
							}
							pSetVehicleMod(v, i, pGetNumVehicleMod(v, i) - 1, false);
						}
						//pSetVehiclePlateText(v, "Phantom");
						UpgradeCar = false;
					}
				}

				if (TpToMe && Tools->IsValid(addy.localPlayer->_ObjectNavigation)) {
					DWORD v = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
					//if (pNetworkRequestControlOfEntity(v)) {
					SetVehicleForwardSpeed(addy.localPlayer->gVehicleManager(), 0.0f);
					addy.localPlayer->gVehicleManager()->SetPosClass(GetCoordsInfrontOfCam(ToVector3(pLocalCamPos), CamRotLocal, 10.0f));
					pSetVehicleOnGroundProperly(v, 0);
					TpToMe = false;
					//}
				}
			}
			if (Tools->IsValid(addy.localPlayer->gPlayerInfo())) {
				VehSpeedHack();
				if (Hacks->NoFallOfVeh) {
					addy.localPlayer->NoFallOfSeatBelt(1);
				}
				else {
					addy.localPlayer->NoFallOfSeatBelt(0);
				}

				WayPointPos = *reinterpret_cast<Vector2*>(addy.WayPointRead);

				static bool pFallBack = false;
				if (Hacks->Invisible) {
					for (int i = 0; i < 2; i++)
						SetEntityVisible(addy.localPlayer, i, false, true);
					pFallBack = true;
				}
				else {
					if (pFallBack == true)for (int i = 0; i < 2; i++)
						SetEntityVisible(addy.localPlayer, i, false, true), pFallBack = false;
				}

				LocalPlayerHandle = centity_to_handle(addy.localPlayer);

				if (Tools->IsValid(addy.localPlayer->gWeaponManager()) && Tools->IsValid(addy.localPlayer->gWeaponManager()->_WeaponInfo) && !pMenu->visible) {
					if (Hacks->No_Recoil == 1) { //No Recoil
						addy.localPlayer->gWeaponManager()->_WeaponInfo->RecoilSet(0.0f);
					}
					if (Hacks->No_Recoil == 2) { //Counter the recoil
						addy.localPlayer->gWeaponManager()->_WeaponInfo->RecoilSet(0.0f, true);
					}
					if (Hacks->No_Recoil == 3) { //Set coustum recoil
						addy.localPlayer->gWeaponManager()->_WeaponInfo->RecoilSet(Hacks->No_Recoil_Coustum, false);
					}
					if (Hacks->No_Spread) {
						addy.localPlayer->gWeaponManager()->_WeaponInfo->SpreadSet(Hacks->No_Spread_coustum);
					}
					if (Hacks->Fast_Reload) {
						addy.localPlayer->gWeaponManager()->_WeaponInfo->NoReload();
					}
					if (Hacks->Super_Dmg) {
						addy.localPlayer->gWeaponManager()->_WeaponInfo->SuperDmg();
					}
					if (Hacks->Sniper_Range) {
						addy.localPlayer->gWeaponManager()->_WeaponInfo->SuperRange();
					}
					if (Hacks->Super_Impulse) {
						addy.localPlayer->gWeaponManager()->_WeaponInfo->SuperImpulse();
					}
				}

				//if (Hacks->Veh_God_Mode == 1)addy.localPlayer->NoVehDmg(); //AutoHeal

				if (Hacks->Wanted == 1)addy.localPlayer->gPlayerInfo()->SetWanted(0);
				if (Hacks->Wanted == 2)addy.localPlayer->gPlayerInfo()->SetWanted(5);
			}
			LTickOthers = CTickOthers;
		}
		CTickOthers = CurTime;

		if (VK_ResetHealth_PRESS)addy.localPlayer->ResetHealth();
		if (VK_ResetArmour_PRESS)addy.localPlayer->ResetArmour();

		if (Hacks->ResetHP == true) {
			addy.localPlayer->HP = addy.localPlayer->MaxHP;
			Hacks->ResetHP = false;
		}

		if (Hacks->ResetArmour == true) {
			addy.localPlayer->ResetArmour();
			Hacks->ResetArmour = false;
		}

		if (Hacks->God_Mode == 1 && addy.localPlayer->HP != 0.0f)addy.localPlayer->ResetHealth(); //Auto Heal

		//if (Hacks->God_Mode == 2 && addy.localPlayer->HP != 0.0f)
		//	addy.localPlayer->GodModeToggle = true;
		//else
		//	addy.localPlayer->GodModeToggle = false;


		static DWORD64 CTICKRapid = 0x0;
		static DWORD64 LTICKRapid = 0x0;

		if ((CTICKRapid - LTICKRapid) > Hacks->Fire_Interval)
		{
			DWORD BulletType = 0x0;
			if (Hacks->Bullet_Type == 1)BulletType = 0x1B06D571; if (Hacks->Bullet_Type == 2)BulletType = 0x05FC3C11; if (Hacks->Bullet_Type == 3)BulletType = 0x1D073A89;
			if (Hacks->Bullet_Type == 4)BulletType = 0x93E220BD; if (Hacks->Bullet_Type == 5)BulletType = 0xB1CA77B1; if (Hacks->Bullet_Type == 6)BulletType = 0x7F7497E5;
			if (Hacks->Bullet_Type == 7)BulletType = 0x6D544C99; if (Hacks->Bullet_Type == 8)BulletType = 0x497FACC3; if (Hacks->Bullet_Type == 9)BulletType = 0x24B17070;
			if (Hacks->Bullet_Type == 10)BulletType = 0x787F0BB;

			if (Hacks->Aimbot_Silent && addy.localPlayer->HP > 0.0f && Tools->IsValid(addy.localPlayer->gWeaponManager()) && Tools->IsValid(addy.localPlayer->gWeaponManager()->_WeaponInfo) && AimOnTarget) {
				shoot_bullets(ToPVector3(LocalPlayerCamLoc), PVector3(KillTargetSilentPos.x, KillTargetSilentPos.y, KillTargetSilentPos.z), (int)addy.localPlayer->gWeaponManager()->_WeaponInfo->Damage, (bool)false, Hacks->Bullet_Type > 0 ? BulletType : addy.localPlayer->gWeaponManager()->GetHash(),
					Hacks->No_Blame_Local ? NULL : LocalPlayerHandle, (bool)true, (bool)true, (int)-1, NULL, NULL, NULL, NULL, NULL);
			}

			if (Hacks->Rapide_Fire && addy.localPlayer->HP > 0.0f && Tools->IsValid(addy.localPlayer->gWeaponManager()) && Tools->IsValid(addy.localPlayer->gWeaponManager()->_WeaponInfo) && !pMenu->visible) {
				if (RapideFireKey) {
					shoot_bullets(ToPVector3(LocalPlayerCamLoc), (PVector3)ToPVector3(GetCoordsInfrontOfCam(LocalPlayerCamLoc, (Vector3)CamRotLocal, 100.0f)), (int)addy.localPlayer->gWeaponManager()->_WeaponInfo->Damage, (bool)false, Hacks->Bullet_Type > 0 ? BulletType : addy.localPlayer->gWeaponManager()->GetHash(), LocalPlayerHandle, true, true, -1.0f, 0, 0, 0, 0, 0);
				}
			}
			LTICKRapid = CTICKRapid;
		}
		CTICKRapid = CurTime;

		if (KillPlayerTarget)
			shoot_bullets(PVector3(PlayerTargetPos.x, PlayerTargetPos.y, PlayerTargetPos.z + 0.7f), PVector3(PlayerTargetPos.x, PlayerTargetPos.y, PlayerTargetPos.z - 0.8f), (int)360, (bool)false, (DWORD)0xB1CA77B1, (DWORD)NULL, (bool)true, (bool)true, (float)-1, NULL, NULL, NULL, NULL, NULL), KillPlayerTarget = false;

		if (KillPlayerTargetSilent)
			shoot_bullets(PVector3(PlayerTargetPos.x, PlayerTargetPos.y, PlayerTargetPos.z + 0.7f), PVector3(PlayerTargetPos.x, PlayerTargetPos.y, PlayerTargetPos.z - 0.8f), (int)360, (bool)false, (DWORD)0xB1CA77B1, (DWORD)NULL, (bool)false, (bool)false, (float)-1, NULL, NULL, NULL, NULL, NULL), KillPlayerTargetSilent = false;


		if (Hacks->NoGravity)addy.localPlayer->Gravity(false);

		static DWORD64 CTick23 = 0x0;
		static DWORD64 LTick23 = 0x0;
		if ((CTick23 - LTick23) > 110) {

			if (Hacks->RageNames) {
				addrRagePool = *reinterpret_cast<DWORD64*>(PatternReplayInterfaceRageMP + *reinterpret_cast<DWORD*>(PatternReplayInterfaceRageMP + 3) + 7);
				if (addrRagePool != NULL) {
					for (int i = 0; i < 255; i++) {
						CRemotePlayer* pRagePool = *reinterpret_cast<CRemotePlayer**>((DWORD64)addrRagePool + ((DWORD64)i * 0x8));
						//std::cout << "Name: " << GetNameFromIndex(pRagePool->Id) << " ID: " << pRagePool->Id << " Ped_Link: " << pRagePool->pPedLink << std::endl;
						//if(tmpObject == pRagePool->pPedLink->pPed)
						if (Tools->IsValid(pRagePool) && Tools->IsValid(pRagePool->pPedLink) && Tools->IsValid(pRagePool->pPedLink->pPed))
							for (int i2 = 0; i2 < 255; i2++) {
								CObject* tmpObject = addy.ReplayInterface->ped_interface->get_ped(i2);
								if (Tools->IsValid(tmpObject) && Tools->IsValid(pRagePool) && Tools->IsValid(pRagePool->pPedLink) && Tools->IsValid(pRagePool->pPedLink->pPed)) {
									if (pRagePool->pPedLink->pPed == tmpObject && pRagePool->pPedLink->pPed != addy.localPlayer) {
										//NamesPedObj[i2] = reinterpret_cast<CObject*>(pRagePool->pPedLink->pPed);
										Names[i2].assign(GetNameFromIndex(pRagePool->Id));
									}
								}
							}
					}
				}
			}


			if (Hacks->Save_Mode)Hacks->FlySpeed = 1;

			if (TeleportToPlayerPos != Vector3(0.0f, 0.0f, 0.0f))TeleportToPos(TeleportToPlayerPos), TeleportToPlayerPos = Vector3(0.0f, 0.0f, 0.0f);

			if (TpToWay)TeleportToWaypoint(), TpToWay = false;
			if (pTeleportToWaypointHotKey && !pMenu->visible)TeleportToWaypoint();

			if (Hacks->UseRayCast && Tools->IsValid(addy.ReplayInterface->ped_interface)) {
				DWORD HitAnyWorld = false;
				DWORD HitAnyObj = false;
				DWORD HitAnyVeg = false;
				DWORD HitEnity = 0;
				PVector3 HitCord;
				PVector3 EmtVec;

				for (int i = 0; i < 255; i++) {
					CObject* tmpObject = addy.ReplayInterface->ped_interface->get_ped(i);
					if (Tools->IsValid(tmpObject) && tmpObject->HP > 0.0f) {
						if (Hacks->UseRayCast == 1) {
							auto ShapeTestWorld = _START_SHAPE_TEST_RAY(PVector3(LocalPlayerCamLoc.x, LocalPlayerCamLoc.y, LocalPlayerCamLoc.z),
								ToPVector3(NPC_Bones[i][Hacks->Aimbot_Bone <= 0 ? 0 : (Hacks->Aimbot_Bone - 1)]), IntersectMap, LocalPlayerHandle, NULL, NULL);
							_GET_RAYCAST_RESULT(ShapeTestWorld, &HitAnyWorld, &HitCord, &EmtVec, &HitEnity);

							if (HitAnyWorld)
								IsVisPedHead[i] = false; //Shape test hit an wall.
							else
								IsVisPedHead[i] = true; //Shape test hit NO Wall.
						}
						if (Hacks->UseRayCast == 2) {
							auto ShapeTestObj = _START_SHAPE_TEST_RAY(PVector3(LocalPlayerCamLoc.x, LocalPlayerCamLoc.y, LocalPlayerCamLoc.z),
								ToPVector3(NPC_Bones[i][Hacks->Aimbot_Bone <= 0 ? 0 : (Hacks->Aimbot_Bone - 1)]), IntersectObjects, LocalPlayerHandle, NULL, NULL);
							_GET_RAYCAST_RESULT(ShapeTestObj, &HitAnyObj, &HitCord, &EmtVec, &HitEnity);

							if (HitAnyObj)
								IsVisPedHead[i] = false; //Shape test hit an wall.
							else
								IsVisPedHead[i] = true; //Shape test hit NO Wall.
						}

						if (Hacks->UseRayCast == 3) {
							auto ShapeTestWorld = _START_SHAPE_TEST_RAY(PVector3(LocalPlayerCamLoc.x, LocalPlayerCamLoc.y, LocalPlayerCamLoc.z),
								ToPVector3(NPC_Bones[i][Hacks->Aimbot_Bone <= 0 ? 0 : (Hacks->Aimbot_Bone - 1)]), IntersectMap, LocalPlayerHandle, NULL, NULL);
							auto ShapeTestObjects = _START_SHAPE_TEST_RAY(PVector3(LocalPlayerCamLoc.x, LocalPlayerCamLoc.y, LocalPlayerCamLoc.z),
								ToPVector3(NPC_Bones[i][Hacks->Aimbot_Bone <= 0 ? 0 : (Hacks->Aimbot_Bone - 1)]), IntersectObjects, LocalPlayerHandle, NULL, NULL);
							_GET_RAYCAST_RESULT(ShapeTestWorld, &HitAnyWorld, &HitCord, &EmtVec, &HitEnity);
							_GET_RAYCAST_RESULT(ShapeTestObjects, &HitAnyObj, &HitCord, &EmtVec, &HitEnity);

							if (HitAnyWorld || HitAnyObj)
								IsVisPedHead[i] = false; //Shape test hit an wall.
							else
								IsVisPedHead[i] = true; //Shape test hit NO Wall.
						}

						if (Hacks->UseRayCast == 4) {
							auto ShapeTestWorld = _START_SHAPE_TEST_RAY(PVector3(LocalPlayerCamLoc.x, LocalPlayerCamLoc.y, LocalPlayerCamLoc.z),
								ToPVector3(NPC_Bones[i][Hacks->Aimbot_Bone <= 0 ? 0 : (Hacks->Aimbot_Bone - 1)]), IntersectMap, LocalPlayerHandle, NULL, NULL);
							auto ShapeTestObjects = _START_SHAPE_TEST_RAY(PVector3(LocalPlayerCamLoc.x, LocalPlayerCamLoc.y, LocalPlayerCamLoc.z),
								ToPVector3(NPC_Bones[i][Hacks->Aimbot_Bone <= 0 ? 0 : (Hacks->Aimbot_Bone - 1)]), IntersectObjects, LocalPlayerHandle, NULL, NULL);
							auto ShapeTestVeg = _START_SHAPE_TEST_RAY(PVector3(LocalPlayerCamLoc.x, LocalPlayerCamLoc.y, LocalPlayerCamLoc.z),
								ToPVector3(NPC_Bones[i][Hacks->Aimbot_Bone <= 0 ? 0 : (Hacks->Aimbot_Bone - 1)]), IntersectVegetation, LocalPlayerHandle, NULL, NULL);

							_GET_RAYCAST_RESULT(ShapeTestWorld, &HitAnyWorld, &HitCord, &EmtVec, &HitEnity);
							_GET_RAYCAST_RESULT(ShapeTestObjects, &HitAnyObj, &HitCord, &EmtVec, &HitEnity);
							_GET_RAYCAST_RESULT(ShapeTestVeg, &HitAnyVeg, &HitCord, &EmtVec, &HitEnity);

							if (HitAnyWorld || HitAnyObj || HitAnyVeg)
								IsVisPedHead[i] = false; //Shape test hit an wall.
							else
								IsVisPedHead[i] = true; //Shape test hit NO Wall.
						}
					}
				}
			}
			LTick23 = CTick23;
		}
		CTick23 = CurTime;

		static CVehicleManager* TpToVeh = NULL;
		static CVehicleManager* TpToVehTarget = NULL;
		static CObject* TpToVehPed = NULL;
		static CObject* TpToVehTargetPed = NULL;


		if (Hacks->TpInNearVeh && addy.localPlayer->IsInVehicle())
			Hacks->TpInNearVeh = false;

		if (Hacks->TpInNearVeh_Passanger && addy.localPlayer->IsInVehicle())
			Hacks->TpInNearVeh_Passanger = false;

		if (Hacks->TpInNearVeh && Tools->IsValid(TpToVehTarget)) {
			if (Tools->IsValid(TpToVehTarget->GetDriver())) {
				DWORD HandleDriver = centity_to_handle(TpToVehTarget->GetDriver());
				CLEAR_PED_TASKS(HandleDriver);
				oCLEAR_PED_TASKS_IMMEDIATELY(HandleDriver);
			}
			SET_PED_INTO_VEH(addy.localPlayer, TpToVehTarget, 0, 1);
			Hacks->TpInNearVeh = false;
		}

		if (TpInNearVeh_Passanger && Tools->IsValid(TpToVehTarget)) {
			//DWORD HandleVeh = centity_to_handle(reinterpret_cast<CObject*>(TpToVehTarget));
			if (Tools->IsValid(TpToVehTarget->GetDriver())) {
				DWORD HandleDriver = centity_to_handle(TpToVehTarget->GetDriver());
				CLEAR_PED_TASKS(HandleDriver);
				oCLEAR_PED_TASKS_IMMEDIATELY(HandleDriver);
			}
			SET_PED_INTO_VEH(addy.localPlayer, TpToVehTarget, 1, 1);
			TpInNearVeh_Passanger = false;
		}

		if (Hacks->TpToNearVeh && Tools->IsValid(TpToVehTarget)) {
			TeleportToPos(Vector3(TpToVehTarget->fPosition.x, TpToVehTarget->fPosition.y, TpToVehTarget->fPosition.z + 2.0f));
			Hacks->TpToNearVeh = false;
		}

		float curdistance = 250.0f;
		float curdistanceCross = 450.0f;
		float curdistanceMouse = 450.0f;
		bool SearchNew = true;

		//if (GetKeyState(VK_F3) & 0x800)SetAngi(NULL, Vector3(addy.localPlayer->fPosition.x, addy.localPlayer->fPosition.y, addy.localPlayer->fPosition.z), 10.0f, 0.0f);

		if (Tools->IsValid(addy.pGameCamManager)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer1)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer2)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles)) {
			if (addy.localPlayer->IsInsideFirstPerson()) {
				LocalPlayerCamLoc = addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles->CrosshairLocation;
				CamRotLocal = addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles->CamRotFPS;
			}
			else {
				CamRotLocal = addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles->CamRot;
				LocalPlayerCamLoc = addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer1->CrosshairLocation;
			}
		}

		if (Tools->IsValid(addy.ReplayInterface->ped_interface) && Tools->IsValid(addy.ReplayInterface->vehicle_interface))
			for (int i = 0; i < 255; i++) {
				CObject* tmpObject = addy.ReplayInterface->ped_interface->get_ped(i);
				CVehicleManager* tmpObjectVeh = addy.ReplayInterface->vehicle_interface->get_vehicle(i);
				if (Tools->IsValid(tmpObjectVeh) && Tools->IsValid(tmpObjectVeh->ModelInfo) && Tools->IsValid(tmpObjectVeh->_ObjectNavigation)) {
					if (Hacks->Drop_Vehicles) {
						DWORD ent = centity_to_handle(reinterpret_cast<CObject*>(tmpObjectVeh));
						if (tmpObjectVeh != addy.localPlayer->gVehicleManager())
							pSetEntityCollision(ent, true, true, true);
					}
					if (renderer->Draw3DBoxVehicle(tmpObjectVeh->fPosition, tmpObjectVeh->_ObjectNavigation->Rotation, tmpObjectVeh->ModelInfo->MinDim, tmpObjectVeh->ModelInfo->MaxDim, DirectX::Colors::Red.v, true) && AimMouseTarget == tmpObjectVeh) {
						if (aKeyPressed == VK_F1) {
							if (Tools->IsValid(tmpObjectVeh->GetDriver())) {
								DWORD HandleDriver = centity_to_handle(tmpObjectVeh->GetDriver());
								CLEAR_PED_TASKS(HandleDriver);
								oCLEAR_PED_TASKS_IMMEDIATELY(HandleDriver);
							}
							SET_PED_INTO_VEH(addy.localPlayer, tmpObjectVeh, 0, 1);
						}
					}
					Vector3 ScreenVehPos(0.0f, 0.0f, 0.0f);
					if (W2S(tmpObjectVeh->fPosition, ScreenVehPos)) {
						float MouseToVehDistance = Tools->GetDistance(Vector2(ScreenVehPos.x, ScreenVehPos.y), Vector2((float)Tools->pPoint.x, (float)Tools->pPoint.y));
						if (MouseToVehDistance <= curdistanceMouse) {
							curdistanceMouse = MouseToVehDistance;
							AimMouseTargetLoop = tmpObjectVeh;
						}
					}

					DWORD vVeh = centity_to_handle(reinterpret_cast<CObject*>(tmpObjectVeh));
					float Distance = Tools->Get3dDistance(addy.localPlayer->fPosition, tmpObjectVeh->fPosition);
					if (Distance < curdistance) {
						curdistance = Distance;
						TpToVeh = tmpObjectVeh;
					}

					if (Hacks->UseLockedCars) {
						if (pNetworkRequestControlOfEntity(vVeh)) {
							tmpObjectVeh->UnLockCar();
						}
						else {
							tmpObjectVeh->UnLockCar();
						}
					}
				}

				if (Tools->IsValid(tmpObject) && tmpObject != addy.localPlayer) {
					//Names[i] = GetNameFromIndex(i);
					//WORD pPlayerHandle = (WORD)centity_to_handle(reinterpret_cast<CObject*>(tmpObject));

					DWORD PlayerHandle = 0x0;
					Vector3 Head2d(0.0f, 0.0f, 0.0f);
					Vector3 Head4D(0.0f, 0.0f, 0.0f);

					if (tmpObject->HP <= 10.0f)
						continue;

					if (Tools->IsValid(tmpObject->BoneManager())) {
						for (int CurBone = 0; CurBone < 15; CurBone++) {
							if (Hacks->Npc_Bone_Esp) {
								NPC_Bones[i][CurBone] = GetBonePosition2Vec(tmpObject, EspBoneType[CurBone]); //Others From GetBone2
							}
							else {
								if (CurBone == pSKEL_Head)
									NPC_Bones[i][CurBone] = BoneCalExternal(tmpObject->HeadPos(), tmpObject->BoneManager()); //Vector with correct speed.
								if (CurBone == pSKEL_Neck_1)
									NPC_Bones[i][CurBone] = BoneCalExternal(tmpObject->NeckPos(), tmpObject->BoneManager()); //Vector with correct speed.
								if (CurBone == pSTOMACH)
									NPC_Bones[i][CurBone] = BoneCalExternal(tmpObject->StomachPos(), tmpObject->BoneManager()); //Vector with correct speed.
							}

						}
						if (Hacks->Aimbot_Bone == 0 || Hacks->Aimbot_Bone == 1)Head4D = NPC_Bones[i][pSKEL_Head];
						if (Hacks->Aimbot_Bone == 2)Head4D = NPC_Bones[i][pSKEL_Neck_1];
						if (Hacks->Aimbot_Bone == 3)Head4D = NPC_Bones[i][pSTOMACH];
					}

					auto FoundAimBone = W2S(Head4D, Head2d);

					float Distance = Tools->Get3dDistance((Vector3)ToVector3(pLocalCamPos), (Vector3)Head4D);

					static Vector3 BoneSpeed(0.0f, 0.0f, 0.0f);
					float CrossHairDistance = Tools->GetDistance(Vector2(Head2d.x, Head2d.y), Vector2(ScreenCenterX, ScreenCenterY));

					if (FoundAimBone && CrossHairDistance <= curdistanceCross && Distance < Hacks->Max_Aimbot_Distance && tmpObject->HP >= 10.0f && IsVisPedHead[i]) {
						curdistanceCross = CrossHairDistance;
						AimCrossTargetLoop = tmpObject;
					}

					static DWORD64 CTickWait = 0x0;
					static DWORD64 LTickWait = 0x0;
					if (Hacks->Aimbot_StickTo == 1 ? ((Tools->IsValid(AimCrossTarget) && AimCrossTarget->HP <= 10.0f) || !AimOnTarget || !Tools->IsValid(AimCrossTarget)) : true) {
						SearchNew = true;
					}
					else {
						SearchNew = false;
					}

					if (Hacks->ClassicGangWarMode && (addy.localPlayer->GetPedClothing() == tmpObject->GetPedClothing() && addy.localPlayer->GetPedClothing2() == tmpObject->GetPedClothing2()))
						continue;
					bool IsFriend = false;
					for (int iFriends = 0; iFriends < Hacks->ArrayFriends.size(); ++iFriends) {
						for (int iFriends = 0; iFriends < Hacks->ArrayFriends.size(); iFriends++)
							if (Names[i]._Equal(Hacks->ArrayFriends[iFriends]))
								IsFriend = true;
					}
					if (!IsFriend && (Hacks->TargetSelectionAim == 1 ? !Tools->IsValid(tmpObject->gPlayerInfo()) : Hacks->TargetSelectionAim == 2 ? Tools->IsValid(tmpObject->gPlayerInfo()) : Hacks->TargetSelectionAim == 3 ? true : false)) {
						if (FoundAimBone && CrossHairDistance >= 3.5f && CrossHairDistance <= Hacks->Aimbot_Fov && IsVisPedHead[i] && AimCrossTarget == tmpObject && tmpObject->IsVisible() && AimCrossTarget->HP >= 10.0f) {
							if (Hacks->Aimbot_Toggle == 1) { //Check if HotKey is enabled
								/*if (Hacks->Aimbot_Silent <= 1)
									SetAngi(tmpObject, Head4D, Distance, CrossHairDistance, true);
								else
									SetAngi(tmpObject, Head4D, Distance, CrossHairDistance, false);*/

								if (Hacks->Aimbot_Silent <= 1)
									AimVec(Head4D, tmpObject, true);
								else
									AimVec(Head4D, tmpObject, false);
								KillTargetSilentPos = Head4D;
								if (Hacks->InstantKillPed)tmpObject->HP = 0.0f;
							}

							if (Hacks->Aimbot_Toggle == 2 && AimOnTarget) { //Check if HotKey is enabled
								if (Hacks->Aimbot_Silent <= 1)
									AimVec(Head4D, tmpObject, true);
								else
									AimVec(Head4D, tmpObject, false);
								KillTargetSilentPos = Head4D;
								if (Hacks->InstantKillPed)tmpObject->HP = 0.0f;
							}
						}
					}
				}
			}
		static DWORD64 CTickTarget = 0x0;
		static DWORD64 LTickTarget = 0x0;
		if ((CTickTarget - LTickTarget) > Hacks->Wait) {
			if (SearchNew || !AimOnTarget)AimCrossTarget = AimCrossTargetLoop;
			AimMouseTarget = AimMouseTargetLoop;
			LTickTarget = CTickTarget;
		}
		CTickTarget = CurTime;
		TpToVehTarget = TpToVeh;
	}
}

DWORD64 __fastcall hookIs_Game_Thread(DWORD64 a1) {
	static uint64_t FrameCounta = 0x0;
	if (FrameCounta != *(uint64_t*)FrameCount && ForGroundWindow == GtaHWND)
	{
		FrameCounta = *(uint64_t*)FrameCount;
		onTickInit();
	}
	return oOriginalFnGame(a1);
}

void InitializeWnd() {
	if (GtaHWND)OldWndProc = (WNDPROC)SetWindowLongPtrA(GtaHWND, GWLP_WNDPROC, (DWORD_PTR)HookedWndProc);
}
void RestoreWnd() {
	SetWindowLongPtr(GtaHWND, GWLP_WNDPROC, (DWORD_PTR)OldWndProc);
}

#include "detours.h"
bool ptrswapPresent = false;
bool OBShook = false;
DWORD_PTR SwapChainBackUp = 0x0;

const char* P1 = "Player";
const char* P2 = "Aimbot";;
const char* P3 = "Weapon";
const char* P4 = "Vehicle";
const char* P5 = "3D Radar";
const char* P6 = "2D Radar";
const char* P7 = "Other";
const char* P8 = "Hotkeys";
string SaveExePath = "none";

#include <exception>
namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}

string LoaderSettingsPath = "C:\\";
bool firstTime = true;
int ppStatus = -100;

HRESULT WINAPI hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (firstTime) {
		renderer = std::make_unique<D3D11Renderer>((IDXGISwapChain*)pSwapChain);
		renderer->Initialize();
		m_font = std::make_unique<SpriteFont>(renderer->device, FA1, std::size(FA1));
		DX11SpriteBatch = std::make_unique<SpriteBatch>(renderer->deviceContext);
		firstTime = false;
	}
	renderer->BeginScene();

	if (Hacks->PlayerInfoCart)
		DrawFpsAndCpu();

	if (Hacks->DrawFov)renderer->DrawCircle(ScreenCenterX, ScreenCenterY, (float)Hacks->Aimbot_Fov, 0.5f, Colors::DeepSkyBlue.v);

	if (Tools->IsValid(addy.localPlayer) && Tools->IsValid(addy.ReplayInterface)) {

		if (Hacks->DrawLastVeh) {
			if (Tools->IsValid(addy.localPlayer->gVehicleManager())) {
				static Vector3 ScreenLastVeh(0.0f, 0.0f, 0.0f);
				if (W2S(Vector3(addy.localPlayer->gVehicleManager()->fPosition.x, addy.localPlayer->gVehicleManager()->fPosition.y, addy.localPlayer->gVehicleManager()->fPosition.z + 0.4f), ScreenLastVeh)) {
					float DistanceLastVeh = Tools->Get3dDistance(addy.localPlayer->gVehicleManager()->fPosition, addy.localPlayer->fPosition);
					renderer->Draw2dText(ScreenLastVeh.x, ScreenLastVeh.y, Colors::Red.v, 0.35f, true, "Last Car(%1.0f)", DistanceLastVeh);
				}
			}
		}
		if (Hacks->WayPointEsp && Tools->IsValid(addy.localPlayer->_ObjectNavigation)) {
			Vector3 ScreenWayPointTop(0.0f, 0.0f, 0.0f);
			Vector3 ScreenWayPointCenter(0.0f, 0.0f, 0.0f);
			Vector3 ScreenWayPointDown(0.0f, 0.0f, 0.0f);
			float Distance = Tools->Get3dDistance(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z), addy.localPlayer->fPosition);

			W2S(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z + 40.0f), ScreenWayPointTop);
			W2S(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z - 40.0f), ScreenWayPointDown);
			if (W2S(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z), ScreenWayPointCenter)) {
				if (Distance <= 400.0f)renderer->DrawLine(ScreenWayPointTop.x, ScreenWayPointTop.y, ScreenWayPointDown.x, ScreenWayPointDown.y, Colors::Red.v);
				renderer->Draw2dText(ScreenWayPointCenter.x, ScreenWayPointCenter.y, Colors::Red.v, Hacks->Esp_Font_Size, true, (const char*)"Marker %1.0f", Distance);
			}
		}

		if (Hacks->PickUpEsp)PickUpEsp();
		if (Hacks->VehicleEsp && Tools->IsValid(addy.ReplayInterface->vehicle_interface))VehicleEsp(pMenu->visible);
		if (Hacks->ObjectEsp && Tools->IsValid(addy.ReplayInterface->object_interface))ObjectEsp(Hacks->ObjectEsp);
		if (Tools->IsValid(addy.ReplayInterface->ped_interface))DrawPeds();
	}

	if (Hacks->Toggle_2D_Radar == 1) {
		float RadarSize = 125.0f + (Hacks->RadarSize) * 25.0f, RadarZoom = 0.2f + (Hacks->RadarZoom) * 0.1f;
		if (Hacks->BackGround)renderer->FillRect(Hacks->RadarLoc.x - (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), RadarSize, RadarSize, Color(0.0f, 0.0f, 0.0f, 0.45f));
		renderer->DrawCross(Hacks->RadarLoc.x, Hacks->RadarLoc.y, RadarSize / 2.0f, RadarSize / 2.0f, Colors::Red.v),
			renderer->DrawLine(Hacks->RadarLoc.x, Hacks->RadarLoc.y, Hacks->RadarLoc.x - (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), Colors::Red.v),
			renderer->DrawLine(Hacks->RadarLoc.x, Hacks->RadarLoc.y, Hacks->RadarLoc.x + (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), Colors::Red.v);

		static bool LockRadarPos = false;
		static float Radar_DifferenceX = 0.0f, Radar_DifferenceY = 0.0f;
		if (GetlButton && pMenu->visible && Radar_Menu) {
			if (Tools->isMouseinRegion(Hacks->RadarLoc.x - (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), Hacks->RadarLoc.x + (RadarSize / 2.0f), Hacks->RadarLoc.y + (RadarSize / 2.0f)) || LockRadarPos) {
				LockRadarPos = true;
				Hacks->RadarLoc.x = Tools->pPoint.x - Radar_DifferenceX,
					Hacks->RadarLoc.y = Tools->pPoint.y - Radar_DifferenceY;
			}
		}
		else {
			LockRadarPos = false;
			Radar_DifferenceX = (Tools->pPoint.x - Hacks->RadarLoc.x);
			Radar_DifferenceY = (Tools->pPoint.y - Hacks->RadarLoc.y);
		}
		if (Radar_Menu && Hacks->Center2d)Hacks->RadarLoc = Vector2(ScreenCenterX, ScreenCenterY);
	}

	static bool LockMenuPos = false;
	static float DifferenceX = 0.0f, DifferenceY = 0.0f;
	if (GetlButton) {
		if (Tools->isMouseinRegion(pMenu->x - 2.0f, pMenu->y - 60.0f, pMenu->x + pMenu->width + 2.0f, pMenu->y - 30.0f) || LockMenuPos) {
			LockMenuPos = true;
			pMenu->x = Tools->pPoint.x - DifferenceX;
			pMenu->y = Tools->pPoint.y - DifferenceY;
		}
	}
	else {
		LockMenuPos = false;
		DifferenceX = (Tools->pPoint.x - pMenu->x);
		DifferenceY = (Tools->pPoint.y - pMenu->y);
	}

	if (pMenu->visible) {
		ScreenWidth = renderer->GetWidth();
		ScreenHight = renderer->GetHeight();
		ScreenCenterX = ScreenWidth / 2.0f;
		ScreenCenterY = ScreenHight / 2.0f;

		if (pMenu->x <= 0.0f)pMenu->x = 2.0f;
		if (pMenu->x + pMenu->width >= ScreenWidth)pMenu->x = pMenu->x - (pMenu->width / 2.0f);
		if (pMenu->y < 100.0f)pMenu->y = 50.0f;
		if (pMenu->y >= ScreenHight - 60.0f)pMenu->y = pMenu->y - pMenu->hight;

		MakeMenu();
		pMenu->noitems = 0;
		MouseWheel = 0.0f;
		//DrawPlayerList();
	}
	if (Hacks->Crosshair == 1)
		renderer->DrawCross(ScreenCenterX, ScreenCenterY, Hacks->CrosshairSize, Hacks->CrosshairSize, Colors::Red.v, false);
	if (Hacks->Crosshair == 2)
		renderer->DrawCross(ScreenCenterX, ScreenCenterY, Hacks->CrosshairSize, Hacks->CrosshairSize, Colors::Red.v, true);

	if (pMenu->visible) {
		//float SizeCurso = 1.0f;
		//DX11SpriteBatch->Draw(LogoTexture.Get(), XMFLOAT2(Tools->pPoint.x + (LogoDesc.Width / 2.0f * SizeCurso), Tools->pPoint.y + (LogoDesc.Height / 2.0f * SizeCurso)), nullptr, Colors::Transparent, 0.f, XMFLOAT2(LogoDesc.Width / 2.0f, LogoDesc.Height / 2.0f), SizeCurso);
		renderer->DrawCross((float)Tools->pPoint.x, (float)Tools->pPoint.y, 5.0f, 5.0f, Colors::Red.v, true);
	}
	renderer->EndScene();
	return oReturnPresent(pSwapChain, SyncInterval, Flags);
}

int hkHook(void* pSwapChain, void* pBackBuffer, bool b) {
	int iReturn = oReturn(pSwapChain, pBackBuffer, b);
	if (firstTime) {
		renderer = std::make_unique<D3D11Renderer>((IDXGISwapChain*)pSwapChain);
		renderer->Initialize();
		m_font = std::make_unique<SpriteFont>(renderer->device, FA1, std::size(FA1));
		DX11SpriteBatch = std::make_unique<SpriteBatch>(renderer->deviceContext);
		firstTime = false;
	}

	renderer->BeginScene();

	if (Hacks->PlayerInfoCart)
		DrawFpsAndCpu();

	if (Hacks->DrawFov)renderer->DrawCircle(ScreenCenterX, ScreenCenterY, (float)Hacks->Aimbot_Fov, 0.5f, Colors::DeepSkyBlue.v);

	if (Tools->IsValid(addy.localPlayer) && Tools->IsValid(addy.ReplayInterface)) {

		if (Hacks->DrawLastVeh) {
			if (Tools->IsValid(addy.localPlayer->gVehicleManager())) {
				static Vector3 ScreenLastVeh(0.0f, 0.0f, 0.0f);
				if (W2S(Vector3(addy.localPlayer->gVehicleManager()->fPosition.x, addy.localPlayer->gVehicleManager()->fPosition.y, addy.localPlayer->gVehicleManager()->fPosition.z + 0.4f), ScreenLastVeh)) {
					float DistanceLastVeh = Tools->Get3dDistance(addy.localPlayer->gVehicleManager()->fPosition, addy.localPlayer->fPosition);
					renderer->Draw2dText(ScreenLastVeh.x, ScreenLastVeh.y, Colors::Red.v, 0.35f, true, "Last Car(%1.0f)", DistanceLastVeh);
				}
			}
		}
		if (Hacks->WayPointEsp && Tools->IsValid(addy.localPlayer->_ObjectNavigation)) {
			Vector3 ScreenWayPointTop(0.0f, 0.0f, 0.0f);
			Vector3 ScreenWayPointCenter(0.0f, 0.0f, 0.0f);
			Vector3 ScreenWayPointDown(0.0f, 0.0f, 0.0f);
			float Distance = Tools->Get3dDistance(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z), addy.localPlayer->fPosition);

			W2S(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z + 40.0f), ScreenWayPointTop);
			W2S(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z - 40.0f), ScreenWayPointDown);
			if (W2S(Vector3(WayPointPos.x, WayPointPos.y, addy.localPlayer->fPosition.z), ScreenWayPointCenter)) {
				if (Distance <= 400.0f)renderer->DrawLine(ScreenWayPointTop.x, ScreenWayPointTop.y, ScreenWayPointDown.x, ScreenWayPointDown.y, Colors::Red.v);
				renderer->Draw2dText(ScreenWayPointCenter.x, ScreenWayPointCenter.y, Colors::Red.v, Hacks->Esp_Font_Size, true, (const char*)"Marker %1.0f", Distance);
			}
		}

		if (Hacks->PickUpEsp)PickUpEsp();
		if (Hacks->VehicleEsp && Tools->IsValid(addy.ReplayInterface->vehicle_interface))VehicleEsp();
		if (Hacks->ObjectEsp && Tools->IsValid(addy.ReplayInterface->object_interface))ObjectEsp(Hacks->ObjectEsp);
		if (Tools->IsValid(addy.ReplayInterface->ped_interface))DrawPeds();
	}

	if (Hacks->Toggle_2D_Radar == 1) {
		float RadarSize = 125.0f + (Hacks->RadarSize) * 25.0f, RadarZoom = 0.2f + (Hacks->RadarZoom) * 0.1f;
		if (Hacks->BackGround)renderer->FillRect(Hacks->RadarLoc.x - (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), RadarSize, RadarSize, Color(0.0f, 0.0f, 0.0f, 0.45f));
		renderer->DrawCross(Hacks->RadarLoc.x, Hacks->RadarLoc.y, RadarSize / 2.0f, RadarSize / 2.0f, Colors::Red.v),
			renderer->DrawLine(Hacks->RadarLoc.x, Hacks->RadarLoc.y, Hacks->RadarLoc.x - (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), Colors::Red.v),
			renderer->DrawLine(Hacks->RadarLoc.x, Hacks->RadarLoc.y, Hacks->RadarLoc.x + (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), Colors::Red.v);

		static bool LockRadarPos = false;
		static float Radar_DifferenceX = 0.0f, Radar_DifferenceY = 0.0f;
		if (GetlButton && pMenu->visible && Radar_Menu) {
			if (Tools->isMouseinRegion(Hacks->RadarLoc.x - (RadarSize / 2.0f), Hacks->RadarLoc.y - (RadarSize / 2.0f), Hacks->RadarLoc.x + (RadarSize / 2.0f), Hacks->RadarLoc.y + (RadarSize / 2.0f)) || LockRadarPos) {
				LockRadarPos = true;
				Hacks->RadarLoc.x = Tools->pPoint.x - Radar_DifferenceX,
					Hacks->RadarLoc.y = Tools->pPoint.y - Radar_DifferenceY;
			}
		}
		else {
			LockRadarPos = false;
			Radar_DifferenceX = (Tools->pPoint.x - Hacks->RadarLoc.x);
			Radar_DifferenceY = (Tools->pPoint.y - Hacks->RadarLoc.y);
		}
		if (Radar_Menu && Hacks->Center2d)Hacks->RadarLoc = Vector2(ScreenCenterX, ScreenCenterY);
	}

	static bool LockMenuPos = false;
	static float DifferenceX = 0.0f, DifferenceY = 0.0f;
	if (GetlButton) {
		if (Tools->isMouseinRegion(pMenu->x - 2.0f, pMenu->y - 60.0f, pMenu->x + pMenu->width + 2.0f, pMenu->y - 30.0f) || LockMenuPos) {
			LockMenuPos = true;
			pMenu->x = Tools->pPoint.x - DifferenceX;
			pMenu->y = Tools->pPoint.y - DifferenceY;
		}
	}
	else {
		LockMenuPos = false;
		DifferenceX = (Tools->pPoint.x - pMenu->x);
		DifferenceY = (Tools->pPoint.y - pMenu->y);
	}

	if (pMenu->visible) {
		ScreenWidth = renderer->GetWidth();
		ScreenHight = renderer->GetHeight();
		ScreenCenterX = ScreenWidth / 2.0f;
		ScreenCenterY = ScreenHight / 2.0f;

		if (pMenu->x <= 0.0f)pMenu->x = 2.0f;
		if (pMenu->x + pMenu->width >= ScreenWidth)pMenu->x = pMenu->x - (pMenu->width / 2.0f);
		if (pMenu->y < 100.0f)pMenu->y = 50.0f;
		if (pMenu->y >= ScreenHight - 60.0f)pMenu->y = pMenu->y - pMenu->hight;

		MakeMenu();
		pMenu->noitems = 0;
		MouseWheel = 0.0f;
		//DrawPlayerList();
	}
	if (Hacks->Crosshair == 1)
		renderer->DrawCross(ScreenCenterX, ScreenCenterY, Hacks->CrosshairSize, Hacks->CrosshairSize, Colors::Red.v, false);
	if (Hacks->Crosshair == 2)
		renderer->DrawCross(ScreenCenterX, ScreenCenterY, Hacks->CrosshairSize, Hacks->CrosshairSize, Colors::Red.v, true);

	if (pMenu->visible) {
		//float SizeCurso = 1.0f;
		//DX11SpriteBatch->Draw(LogoTexture.Get(), XMFLOAT2(Tools->pPoint.x + (LogoDesc.Width / 2.0f * SizeCurso), Tools->pPoint.y + (LogoDesc.Height / 2.0f * SizeCurso)), nullptr, Colors::Transparent, 0.f, XMFLOAT2(LogoDesc.Width / 2.0f, LogoDesc.Height / 2.0f), SizeCurso);
		renderer->DrawCross((float)Tools->pPoint.x, (float)Tools->pPoint.y, 5.0f, 5.0f, Colors::Red.v, true);
	}
	renderer->EndScene();
	return iReturn;
}

bool IsReady() {
	if (GetModuleHandleA("d3d11.dll") != NULL && GetModuleHandleA("socialclub.dll") != NULL && GetModuleHandleA("user32.dll") != NULL)
		return true;
	else
		return false;
}


typedef bool(WINAPI* NtScreenToClient_fn)(HWND HwNd, LPPOINT pPoint);
NtScreenToClient_fn oNtScreenToClientHook = NULL;
bool WINAPI NtScreenToClientHook(HWND HwNd, LPPOINT Point) {
	GtaHWND = HwNd;
	//pPoint = (LPPOINT)Point;
	return oNtScreenToClientHook(HwNd, Point);
}

LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

#pragma comment(lib, "d3d11.lib")
#include "ini.h"

vector<string> split(string str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream. 
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}
#pragma optimize("", off)
DWORD WINAPI dwddwdw(LPVOID lpcst) {
	//AllocConsole();
	//freopen_s((FILE**)stdout, "conout$", "w", stdout);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	while (!IsReady()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	static char* buf = nullptr;
	static size_t sz = 1024;
	if (_dupenv_s(&buf, &sz, "LocalAppData") == 0 && buf != nullptr) {
		//printf("EnvVarName = %s\n", buf);
		//SaveLoadPath = buf;
		//SaveLoadPath += "\\Pure_Settings.cfg";
		SaveExePath = buf;
		SaveExePath += "\\Loader_Path.cfg";
		free(buf);
	}
	string line;
	ifstream myfilde(SaveExePath);
	if (myfilde.is_open()) {
		while (std::getline(myfilde, line)) {
			//LoaderSettingsPath = (char*)line.c_str();
		}
		myfilde.close();
	}


	mINI::INIFile SettignsFile(LoaderSettingsPath.c_str());
	// create a data structure
	mINI::INIStructure ini;


	if (DetourIsHelperProcess()) {
		return TRUE;
	}


	Hacks = std::make_unique<Options>();
	pMenu = std::make_unique<D3DMenu>();
	Tools = std::make_unique<cTools>();

	Tools->UnlinkModuleFromPEB((HMODULE)lpcst);
	//HideThread(lpcst);

	HMODULE u32Dll = reinterpret_cast<HMODULE>(GetModuleHandleA("user32.dll"));
	oNtScreenToClientHook = reinterpret_cast<NtScreenToClient_fn>(GetProcAddress(u32Dll, "ScreenToClient"));

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)oNtScreenToClientHook, NtScreenToClientHook);
	DetourTransactionCommit();

	while (GtaHWND == NULL)
		Sleep(100);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)oNtScreenToClientHook, NtScreenToClientHook);
	DetourTransactionCommit();

	InitializeWnd();

	while (addy.BASE == NULL)
		addy.BASE = reinterpret_cast<DWORD64>(GetModuleHandleA(NULL));

	DWORD64 addrDiscord64 = reinterpret_cast<DWORD64>(GetModuleHandleA(("DiscordHook64.dll")));
	if (addrDiscord64) {
		oReturnPresent = reinterpret_cast<OriginalFnPresent>(ps_find_idastyle("56 57 53 48 83 EC 30 44 89 C6", addrDiscord64, Tools->GetModuleInfoX(1).SizeOfImage));
	}

	DWORD64 addrOBS = reinterpret_cast<DWORD64>(GetModuleHandleA(("graphics-hook64.dll")));
	if (addrOBS) {
		oReturn = reinterpret_cast<OriginalFn>(ps_find_idastyle("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC 30 0F B6 05 ?? ?? ?? ?? 48 8B F2 48 89 6C 24 ?? 48 8B E9 84 C0 74 71 E8", addrOBS, Tools->GetModuleInfoX(2).SizeOfImage) + 0x9A0);
	}

	DWORD64 addrMedalTV = reinterpret_cast<DWORD64>(GetModuleHandleA(("medal-hook64.dll")));
	if (addrMedalTV) {
		oReturn = reinterpret_cast<OriginalFn>(ps_find_idastyle("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC 30 0F B6 05 ?? ?? ?? ?? 48 8B F2 48", addrMedalTV, Tools->GetModuleInfoX(3).SizeOfImage) + 0xA90);
	}
	uintptr_t scan_start = addy.BASE;
	size_t scan_size = Tools->GetModuleInfoX(0).SizeOfImage;

	DWORD dwOld = 0x0;
	if (oReturn) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)oReturn, hkHook);
		DetourTransactionCommit();
		OBShook = true;
	}
	else {
		auto Swapchain_Pattern = ps_find_idastyle("48 8B 0D ?? ?? ?? ?? 48 8D 55 A0 48 8B 01", scan_start, scan_size); // find bytes, returns 0 on failure
		if (Swapchain_Pattern) {
			auto SwapChain = (DWORD_PTR*)(Swapchain_Pattern + *(DWORD*)(Swapchain_Pattern + 0x3) + 0x7);
			pSwapChainVtable = *(DWORD_PTR**)(SwapChain);
			pSwapChainVtable = (DWORD_PTR*)(pSwapChainVtable[0]);
			SwapChainBackUp = pSwapChainVtable[8];

			VirtualProtect((LPVOID)pSwapChainVtable, sizeof(DWORD_PTR), (DWORD)PAGE_EXECUTE_READWRITE, (PDWORD)&dwOld);
			oReturnPresent = reinterpret_cast<OriginalFnPresent>(pSwapChainVtable[8]);
			//spoof_call(jmp_rbx_0, VirtualProtect, (LPVOID)pSwapChainVtable, (SIZE_T)0x1000, (DWORD)PAGE_EXECUTE_READWRITE, (PDWORD)&dwOld);
			pSwapChainVtable[8] = (uintptr_t)&hookD3D11Present;
			VirtualProtect((LPVOID)pSwapChainVtable, sizeof(DWORD_PTR), (DWORD)dwOld, (PDWORD)&dwOld);
			ptrswapPresent = true;
		}
		else {
			IDXGISwapChain* pSwapChain;
			WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "DX", NULL };
			RegisterClassExA(&wc);
			auto hWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, NULL, NULL, wc.hInstance, NULL);

			D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
			D3D_FEATURE_LEVEL obtainedLevel;
			ID3D11Device* d3dDevice = nullptr;
			ID3D11DeviceContext* d3dContext = nullptr;

			DXGI_SWAP_CHAIN_DESC scd;
			ZeroMemory(&scd, sizeof(scd));
			scd.BufferCount = 1;
			scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

			scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			scd.OutputWindow = hWnd;
			scd.SampleDesc.Count = 1; //Set to 1 to disable multisampling
			scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

			scd.BufferDesc.Width = 1;
			scd.BufferDesc.Height = 1;
			scd.BufferDesc.RefreshRate.Numerator = 0;
			scd.BufferDesc.RefreshRate.Denominator = 1;

			UINT createFlags = 0;
			IDXGISwapChain* d3dSwapChain = 0;
			if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags, requestedLevels, sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &scd, &pSwapChain, &d3dDevice, &obtainedLevel, &d3dContext)))
			{
				MessageBoxA(hWnd, "Failed to create directX device and swapchain!", "Error", MB_ICONERROR);
				return NULL;
			}

			pSwapChainVtable = (DWORD_PTR*)pSwapChain;
			pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];
			SwapChainBackUp = pSwapChainVtable[8];

			VirtualProtect((LPVOID)pSwapChainVtable, sizeof(DWORD_PTR), (DWORD)PAGE_EXECUTE_READWRITE, (PDWORD)&dwOld);
			oReturnPresent = reinterpret_cast<OriginalFnPresent>(pSwapChainVtable[8]);
			pSwapChainVtable[8] = (uintptr_t)hookD3D11Present;
			VirtualProtect((LPVOID)pSwapChainVtable, sizeof(DWORD_PTR), (DWORD)dwOld, (PDWORD)&dwOld);
			ptrswapPresent = true;
		}
	}
	InitGameFunctions();

	oOriginalFnGame = reinterpret_cast<OriginalFnGame>(ps_find_idastyle("48 89 5C 24 ?? 57 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 48 8B CB E8 ?? ?? ?? ?? 8B 83 ?? ?? ?? ??", scan_start, scan_size));
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)oOriginalFnGame, hookIs_Game_Thread);
	DetourTransactionCommit();

	FlyPatt = ps_find_idastyle("74 4C 48 8B 07 4C", scan_start, scan_size); // find bytes, returns 0 on failure

	while (!handle_unload) {
		if (Save == 1) {
			// populate the structure
			ini[P1]["Menu_Key"] = std::to_string(Hacks->OpenMenuKey);
			ini[P1]["Information"] = std::to_string(Hacks->PlayerInfoCart);
			ini[P1]["God_Mode"] = std::to_string(Hacks->God_Mode);
			ini[P1]["No_Gravity"] = std::to_string(Hacks->NoGravity);
			ini[P1]["Invsible"] = std::to_string(Hacks->Invisible);
			ini[P1]["No_Ragdoll"] = std::to_string(Hacks->NoRagdoll);
			ini[P1]["Crosshair"] = std::to_string(Hacks->Crosshair);
			ini[P1]["Wanted"] = std::to_string(Hacks->Wanted);
			ini[P1]["Pick_Up_ESP"] = std::to_string(Hacks->PickUpEsp);
			ini[P1]["Vehicle_ESP"] = std::to_string(Hacks->VehicleEsp);
			ini[P1]["Vehicle_ESP_Name"] = std::to_string(Hacks->VehicleEsp_Name);
			ini[P1]["Vehicle_ESP_Distance"] = std::to_string(Hacks->VehicleEsp_Distance);
			ini[P1]["Vehicle_ESP_3dBox"] = std::to_string(Hacks->VehicleEsp_3Dbox);

			ini[P1]["Object_ESP"] = std::to_string(Hacks->ObjectEsp);
			ini[P1]["Display_LocalPlayer"] = std::to_string(Hacks->LocalPlayerDisplay);

			ini[P2]["Max_Distance"] = std::to_string(Hacks->Max_Aimbot_Distance);
			ini[P2]["Toggle"] = std::to_string(Hacks->Aimbot_Toggle);
			ini[P2]["Type"] = std::to_string(Hacks->Aimbot_Type);
			ini[P2]["Smoothing"] = std::to_string(Hacks->Aimbot_Smooth);
			ini[P2]["Aimbot_HorizontalOnly"] = std::to_string(Hacks->Aimbot_HorizontalOnly);
			ini[P2]["Fov"] = std::to_string(Hacks->Aimbot_Fov);
			ini[P2]["Prediction_Scale"] = std::to_string(Hacks->Aimbot_Prediction);
			ini[P2]["Draw_Fov"] = std::to_string(Hacks->DrawFov);
			ini[P2]["Bone"] = std::to_string(Hacks->Aimbot_Bone);
			ini[P2]["Aim_Lock"] = std::to_string(Hacks->Aimbot_StickTo);
			ini[P2]["Silent"] = std::to_string(Hacks->Aimbot_Silent);
			ini[P2]["Target_Selection"] = std::to_string(Hacks->TargetSelectionAim);

			ini[P3]["No_Recoil"] = std::to_string(Hacks->No_Recoil);
			ini[P3]["No_Spread"] = std::to_string(Hacks->No_Spread);
			ini[P3]["No_Reload"] = std::to_string(Hacks->Fast_Reload);
			ini[P3]["Super_Velocity"] = std::to_string(Hacks->Super_Velocity);
			ini[P3]["Super_DMG"] = std::to_string(Hacks->Super_Dmg);
			ini[P3]["Sniper_Range"] = std::to_string(Hacks->Sniper_Range);
			ini[P3]["Rapide_Fire"] = std::to_string(Hacks->Rapide_Fire);
			ini[P3]["Rapide_Fire_Type"] = std::to_string(Hacks->Bullet_Type);
			ini[P3]["WeaponSpawn_1"] = std::to_string(Hacks->WeaponSpawn1);
			ini[P3]["WeaponSpawn_2"] = std::to_string(Hacks->WeaponSpawn2);
			ini[P3]["WeaponSpawn_3"] = std::to_string(Hacks->WeaponSpawn3);
			ini[P3]["WeaponSpawn_4"] = std::to_string(Hacks->WeaponSpawn4);

			ini[P4]["God_mode"] = std::to_string(Hacks->Veh_God_Mode);
			ini[P4]["Impulse"] = std::to_string(Hacks->Veh_Impulse);
			ini[P4]["Use_Locked_Cars"] = std::to_string(Hacks->UseLockedCars);
			ini[P4]["Draw_Last"] = std::to_string(Hacks->DrawLastVeh);

			ini[P5]["Max_Distance"] = std::to_string(Hacks->Npc_Max_Distance);
			ini[P5]["Name"] = std::to_string(Hacks->Npc_Name_Esp);
			ini[P5]["Box_Type"] = std::to_string(Hacks->Npc_Box_Esp_Type);
			ini[P5]["Health"] = std::to_string(Hacks->Npc_Health_Esp);
			ini[P5]["Weapon"] = std::to_string(Hacks->Npc_Weapon_Esp);
			ini[P5]["Distance"] = std::to_string(Hacks->Npc_Distance_Esp);
			ini[P5]["Barrel"] = std::to_string(Hacks->Npc_Barrel_Esp);
			ini[P5]["Bone"] = std::to_string(Hacks->Npc_Bone_Esp);
			ini[P5]["Head_Dot"] = std::to_string(Hacks->Npc_HeadDot_Esp);
			ini[P5]["Head_Dot_Size"] = std::to_string(Hacks->Npc_HeadDot_Size);
			ini[P5]["Target"] = std::to_string(Hacks->TargetSelection);

			ini[P6]["Toggle"] = std::to_string(Hacks->Toggle_2D_Radar);
			ini[P6]["Center"] = std::to_string(Hacks->Center2d);
			ini[P6]["Health"] = std::to_string(Hacks->Health2dRadar);
			ini[P6]["BackGround"] = std::to_string(Hacks->BackGround);
			ini[P6]["Radar_Size"] = std::to_string(Hacks->RadarSize);
			ini[P6]["Radar_Zoom"] = std::to_string(Hacks->RadarZoom);
			ini[P6]["Target_Selection"] = std::to_string(Hacks->TargetSelectionRadar);
			ini[P6]["Pos_X"] = std::to_string(Hacks->RadarLoc.x);
			ini[P6]["Pos_Y"] = std::to_string(Hacks->RadarLoc.y);

			ini[P7]["Use_Raycasting"] = std::to_string(Hacks->UseRayCast);
			ini[P7]["Fly_Hack"] = std::to_string(Hacks->FlyHack);
			ini[P7]["Fly_Hack_Key"] = std::to_string(Hacks->HotKeyFly);
			ini[P7]["Fly_Hack_Speed"] = std::to_string(Hacks->FlySpeed);
			ini[P7]["Text_Shadows"] = std::to_string(Hacks->Text_Shadow);
			ini[P7]["Rage_Names"] = std::to_string(Hacks->RageNames);
			ini[P7]["Friend_List"] = Hacks->FriendList;

			ini[P8]["Aim_Key"] = std::to_string(Hacks->HotKeyAim);
			ini[P8]["Aim_Key2"] = std::to_string(Hacks->HotKeyAim2);
			ini[P8]["Fly_Hack_Key"] = std::to_string(Hacks->HotKeyFly);
			ini[P8]["No_Clip_Key"] = std::to_string(Hacks->HotKeyNoClip);
			ini[P8]["Teleport_Key"] = std::to_string(Hacks->HotKeyTeleportToWayP);
			ini[P8]["ResetHealth_Key"] = std::to_string(Hacks->HotKeyResetHealth);
			ini[P8]["ResetArmour_Key"] = std::to_string(Hacks->HotKeyResetArmour);
			ini[P8]["FlipVeh_Key"] = std::to_string(Hacks->HotKeyFlipVeh);
			ini[P8]["Impulse_Increace"] = std::to_string(Hacks->HotKeyVehImpulseIncreace);
			ini[P8]["Impulse_Decreace"] = std::to_string(Hacks->HotKeyVehImpulseDecrease);
			ini[P8]["Impulse_Constant"] = std::to_string(Hacks->HotKeyVehImpulseConstant);
			// generate an INI file (overwrites any previous file)
			SettignsFile.generate(ini);
			Beep(1000, 100);
			Save = false;
		}
		if (Load == 1) {
			SettignsFile.read(ini);

			Hacks->OpenMenuKey = stoi(ini.get(P1).get("Menu_Key"));
			Hacks->PlayerInfoCart = stoi(ini.get(P1).get("Information"));
			Hacks->God_Mode = stoi(ini.get(P1).get("God_Mode"));
			Hacks->NoGravity = stoi(ini.get(P1).get("No_Gravity"));
			Hacks->Invisible = stoi(ini.get(P1).get("Invsible"));
			Hacks->NoRagdoll = stoi(ini.get(P1).get("No_Ragdoll"));
			Hacks->Crosshair = stoi(ini.get(P1).get("Crosshair"));
			Hacks->Wanted = stoi(ini.get(P1).get("Wanted"));
			Hacks->PickUpEsp = stoi(ini.get(P1).get("Pick_Up_ESP"));
			Hacks->VehicleEsp = stoi(ini.get(P1).get("Vehicle_ESP"));
			Hacks->VehicleEsp_Name = stoi(ini.get(P1).get("Vehicle_ESP_Name"));
			Hacks->VehicleEsp_Distance = stoi(ini.get(P1).get("Vehicle_ESP_Distance"));
			Hacks->VehicleEsp_3Dbox = stoi(ini.get(P1).get("Vehicle_ESP_3dBox"));
			Hacks->ObjectEsp = stoi(ini.get(P1).get("Object_ESP"));
			Hacks->LocalPlayerDisplay = stoi(ini.get(P1).get("Display_LocalPlayer"));

			Hacks->Max_Aimbot_Distance = stof(ini.get(P2).get("Max_Distance"));
			Hacks->Aimbot_Toggle = stoi(ini.get(P2).get("Toggle"));
			Hacks->Aimbot_Type = stoi(ini.get(P2).get("Type"));
			Hacks->Aimbot_Smooth = stof(ini.get(P2).get("Smoothing"));
			Hacks->Aimbot_HorizontalOnly = stoi(ini.get(P2).get("Aimbot_HorizontalOnly"));
			Hacks->Aimbot_Fov = stof(ini.get(P2).get("Fov"));
			Hacks->Aimbot_Prediction = stof(ini.get(P2).get("Prediction_Scale"));
			Hacks->DrawFov = stoi(ini.get(P2).get("Draw_Fov"));
			Hacks->Aimbot_Bone = stoi(ini.get(P2).get("Bone"));
			Hacks->Aimbot_StickTo = stoi(ini.get(P2).get("Aim_Lock"));
			Hacks->Aimbot_Silent = stoi(ini.get(P2).get("Silent"));
			Hacks->TargetSelectionAim = stoi(ini.get(P2).get("Target_Selection"));

			Hacks->No_Recoil = stoi(ini.get(P3).get("No_Recoil"));
			Hacks->No_Spread = stoi(ini.get(P3).get("No_Spread"));
			Hacks->Fast_Reload = stoi(ini.get(P3).get("No_Reload"));
			Hacks->Super_Velocity = stoi(ini.get(P3).get("Super_Velocity"));
			Hacks->Super_Dmg = stoi(ini.get(P3).get("Super_DMG"));
			Hacks->Sniper_Range = stoi(ini.get(P3).get("Sniper_Range"));
			Hacks->Rapide_Fire = stoi(ini.get(P3).get("Rapide_Fire"));
			Hacks->Bullet_Type = stoi(ini.get(P3).get("Rapide_Fire_Type"));
			Hacks->WeaponSpawn1 = stoi(ini.get(P3).get("WeaponSpawn_1"));
			Hacks->WeaponSpawn2 = stoi(ini.get(P3).get("WeaponSpawn_2"));
			Hacks->WeaponSpawn3 = stoi(ini.get(P3).get("WeaponSpawn_3"));
			Hacks->WeaponSpawn4 = stoi(ini.get(P3).get("WeaponSpawn_4"));

			Hacks->Veh_God_Mode = stoi(ini.get(P4).get("God_mode"));
			Hacks->Veh_Impulse = stof(ini.get(P4).get("Impulse"));
			Hacks->UseLockedCars = stoi(ini.get(P4).get("Use_Locked_Cars"));
			Hacks->DrawLastVeh = stoi(ini.get(P4).get("Draw_Last"));

			Hacks->Npc_Max_Distance = stof(ini.get(P5).get("Max_Distance"));
			Hacks->Npc_Name_Esp = stoi(ini.get(P5).get("Name"));
			Hacks->Npc_Box_Esp_Type = stoi(ini.get(P5).get("Box_Type"));
			Hacks->Npc_Health_Esp = stoi(ini.get(P5).get("Health"));
			Hacks->Npc_Weapon_Esp = stoi(ini.get(P5).get("Weapon"));
			Hacks->Npc_Distance_Esp = stoi(ini.get(P5).get("Distance"));
			Hacks->Npc_Barrel_Esp = stoi(ini.get(P5).get("Barrel"));
			Hacks->Npc_Bone_Esp = stoi(ini.get(P5).get("Bone"));
			Hacks->Npc_HeadDot_Esp = stoi(ini.get(P5).get("Head_Dot"));
			Hacks->Npc_HeadDot_Size = stof(ini.get(P5).get("Head_Dot_Size"));
			Hacks->TargetSelection = stoi(ini.get(P5).get("Target"));

			Hacks->Toggle_2D_Radar = stoi(ini.get(P6).get("Toggle"));
			Hacks->Center2d = stoi(ini.get(P6).get("Center"));
			Hacks->Health2dRadar = stoi(ini.get(P6).get("Health"));
			Hacks->BackGround = stoi(ini.get(P6).get("BackGround"));
			Hacks->RadarSize = stof(ini.get(P6).get("Radar_Size"));
			Hacks->RadarZoom = stof(ini.get(P6).get("Radar_Zoom"));
			Hacks->TargetSelectionRadar = stoi(ini.get(P6).get("Target_Selection"));
			Hacks->RadarLoc.x = stof(ini.get(P6).get("Pos_X"));
			Hacks->RadarLoc.y = stof(ini.get(P6).get("Pos_Y"));

			Hacks->UseRayCast = stoi(ini.get(P7).get("Use_Raycasting"));
			Hacks->FlyHack = stoi(ini.get(P7).get("Fly_Hack"));
			Hacks->HotKeyFly = stoi(ini.get(P7).get("Fly_Hack_Key"));
			Hacks->FlySpeed = stof(ini.get(P7).get("Fly_Hack_Speed"));
			Hacks->Text_Shadow = stoi(ini.get(P7).get("Text_Shadows"));
			Hacks->RageNames = stoi(ini.get(P7).get("Rage_Names"));
			Hacks->FriendList = ini.get(P7).get("Friend_List");
			Hacks->ArrayFriends = split(Hacks->FriendList, ',');

			Hacks->HotKeyAim = stoi(ini.get(P8).get("Aim_Key"));
			Hacks->HotKeyAim2 = stoi(ini.get(P8).get("Aim_Key2"));
			Hacks->HotKeyFly = stoi(ini.get(P8).get("Fly_Hack_Key"));
			Hacks->HotKeyNoClip = stoi(ini.get(P8).get("No_Clip_Key"));
			Hacks->HotKeyTeleportToWayP = stoi(ini.get(P8).get("Teleport_Key"));
			Hacks->HotKeyResetHealth = stoi(ini.get(P8).get("ResetHealth_Key"));
			Hacks->HotKeyResetArmour = stoi(ini.get(P8).get("ResetArmour_Key"));
			Hacks->HotKeyFlipVeh = stoi(ini.get(P8).get("FlipVeh_Key"));
			Hacks->HotKeyVehImpulseIncreace = stoi(ini.get(P8).get("Impulse_Increace"));
			Hacks->HotKeyVehImpulseDecrease = stoi(ini.get(P8).get("Impulse_Decreace"));
			Hacks->HotKeyVehImpulseConstant = stoi(ini.get(P8).get("Impulse_Constant"));

			Beep(1000, 100);
			Load = false;
		}

		if (Tools->IsValid(addy.LocalPlayerWrapper))
			addy.localPlayer = addy.LocalPlayerWrapper->getLocalPlayer();

		if (Hacks->Panic_DisableAll == true) {
			Hacks->Crosshair = 0; Hacks->God_Mode = 0; Hacks->NoRagdoll = 0; Hacks->Invisible = 0;
			Hacks->Wanted = 0; Hacks->NoGravity = 0; Hacks->No_Recoil = 0; Hacks->No_Spread = 0;
			Hacks->Fast_Reload = 0; Hacks->Super_Velocity = 0; Hacks->Rapide_Fire = 0; Hacks->Veh_God_Mode = 0;
			Hacks->DrawLastVeh = 0; Hacks->UseLockedCars = 0; Hacks->FixOnDmg = 0; Hacks->Npc_Name_Esp = 0;
			Hacks->Npc_Distance_Esp = 0; Hacks->Npc_Weapon_Esp = 0; Hacks->Npc_Barrel_Esp = 0; Hacks->Npc_Box_Esp_Type = 0;
			Hacks->Npc_Bone_Esp = 0; Hacks->Npc_Health_Esp = 0; Hacks->Npc_HeadDot_Esp = 0; Hacks->TargetSelection = 0;
			Hacks->Npc_Wanted_Esp = 0; Hacks->Toggle_2D_Radar = 0; Hacks->TargetSelectionRadar = 0; Hacks->Health2dRadar = 0;
			Hacks->Center2d = 0; Hacks->UseRayCast = 0; Hacks->WayPointEsp = 0; Hacks->LobbyStresser = 0;
			Hacks->Save_Mode = 0; Hacks->Esp_Font_Size = 0.40f; Hacks->Aimbot_Linear = 0; Hacks->Aimbot_StickTo = 0;
			Hacks->Aimbot_Toggle = 0; Hacks->TargetSelectionAim = 0; Hacks->UseRayCast = 0;
			Hacks->Panic_DisableAll = 0; Hacks->FlyHack = 0; Hacks->FlySpeed = 1;
		}


		ForGroundWindow = GetForegroundWindow();
		if (!Tools->IsValid(m_viewPort) && dw_ViewportPattern != NULL && *reinterpret_cast<DWORD*>(dw_ViewportPattern + 3))m_viewPort = *reinterpret_cast<CViewPort**>(dw_ViewportPattern + *reinterpret_cast<DWORD*>(dw_ViewportPattern + 3) + 7);
		std::this_thread::yield();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	if (ptrswapPresent) {
		VirtualProtect((LPVOID)pSwapChainVtable, sizeof(DWORD_PTR), (DWORD)PAGE_EXECUTE_READWRITE, (PDWORD)&dwOld);
		pSwapChainVtable[8] = (DWORD_PTR)SwapChainBackUp;
		VirtualProtect((LPVOID)pSwapChainVtable, sizeof(DWORD_PTR), (DWORD)dwOld, (PDWORD)&dwOld);
	}
	else {
		if (oReturn) {
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&(PVOID&)oReturn, hkHook);
			DetourTransactionCommit();
		}
		else {
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&(PVOID&)oReturnPresent, hookD3D11Present);
			auto error_detour = DetourTransactionCommit();
		}
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)oOriginalFnGame, hookIs_Game_Thread);
	auto error_detour = DetourTransactionCommit();

	RestoreWnd();

	//FreeLibraryAndExitThread((HMODULE)lpcst, -1);
	return 1;
}

bool VFunction(HMODULE hModule) {
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)dwddwdw, hModule, NULL, NULL);
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		VFunction(hModule);
	}
	return TRUE;
}

const char* opt_Save[] = { ("Off"), ("Saved") };
const char* opt_NameHEsp[] = { ("Off"), ("On"), ("Hash") };
const char* opt_Load[] = { ("Off"), ("Loaded") };
const char* opt_Crosshair[] = { ("Off"), ("Cross"), ("Rainbow") };
const char* opt_HealthEsp[] = { ("Off"), ("TOP"), ("SIDE"), ("TXT") };
const char* opt_BoxEsp[] = { ("Off"), ("2D"), ("3D"), ("FILL") };
const char* opt_RayCasting[] = { ("Off"), ("Map"), ("Objects"), ("Both"), ("Everything") };
const char* opt_DropMoney[] = { ("Off"), ("LocalPlayer"), ("Near") };
const char* opt_FreezeLobby[] = { ("Off"), ("Npcs"), ("Players"), ("Both") };
const char* opt_ObjectEsp[] = { ("Off"), ("On"), ("Hash"), ("Illigal") };
const char* opt_Aimbot_Toggle[] = { ("Off"), ("On"), ("Hotkey") };
const char* opt_Aimbot_Mode[] = { ("Off"), ("Angel"), ("Move") };
const char* opt_GodMode_Toggle[] = { ("Off"), ("Auto Heal"), ("Static") };
const char* opt_WantedLvl[] = { ("Off"), ("Never"), ("Always") };
const char* opt_Aimbot_Type[] = { ("NONE"), ("PLAYER"), ("NPC"), ("BOTH") };
const char* opt_Aimbot_Bone[] = { ("NONE"), ("HEAD"), ("NECK"), ("STOMACHE"), ("DISTANCE") };
const char* opt_Aimbot_Silent[] = { ("Off"), ("On"), ("Only") };
const char* opt_Bullet_Type[] = { ("InHand"), ("PISTOL"), ("SNIPER"), ("PUMP"), ("GRENADE"), ("RPG"), ("FIREWORK"), ("RAILGUN"), ("FLARE"), ("MOLOTOV"), ("SNOWBALL") };
const char* opt_TargetSelection[] = { ("NONE"), ("NPC"), ("Player"), ("BOTH") };
const char* opt_Text_Shadows[] = { ("Off"), ("Small"), ("Medium"), ("Large") };
const char* opt_Recoil[] = { ("Off"), ("On"), ("Counter"), ("Custom") };
const char* opt_Spread[] = { ("Off"), ("On"), ("Custom") };

bool ChangeWeather = false;

static bool Npc_Main_Butto = 0, Npc_Color_Visble = 0, Npc_Color_InVisble = 0;
void DrawButton(float x, float y, float w, float h, const char* txt, bool& Option) {
	renderer->DrawBox(x, y, w, h, DirectX::Colors::Black.v, DirectX::Colors::Black.v);
	XMVECTORF32 DarkMagenta = { { { 0.545098066f, 0.000000000f, 0.545098066f, 0.500000000f } } };
	renderer->DrawBorder2(x, y, w, h, 0.25f, DarkMagenta.v);
	renderer->Draw2dText(x + (w / 2.0f), y + (h / 2.0f), (Tools->isMouseinRegion(x, y, x + w, y + h) ? Colors::Purple.v : Colors::White.v), 0.5f, true, txt);
	if (Option)renderer->FillRect(x, y, 8, h, Colors::Purple.v);
	static bool SendKey = false;
	if (GetlButton) {
		if (Tools->isMouseinRegion(x, y, x + w, y + h) && SendKey)
			Player_Hacks = 0, Players_Menu = 0, Aimbot_Menu = 0, Weapon_Menu = 0, Vehicle_Menu = 0, Npc_Menu = 0, Radar_Menu = 0, Other_Menu = 0, Water_Menu = 0, Option = true, SendKey = false;
	}
	else {
		SendKey = true;
	}
}

static void DrawButtonTest(float x, float y, float w, float h, const char* txt, bool& Option) {
	renderer->DrawBox(x, y, w, h, { 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 0.800000000f }, { 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 0.800000000f });
	renderer->Draw2dText(x + (w / 2.0f), y + (h / 2.0f), (Tools->isMouseinRegion(x, y, x + w, y + h) ? Colors::Purple.v : Colors::White.v), 0.45f, true, txt);
	if (Option)renderer->FillRect(x, y, 5, h, Colors::Purple.v);
	static bool SendKey = false;
	if (GetlButton) {
		if (Tools->isMouseinRegion(x, y, x + w, y + h) && SendKey) {
			Npc_Main_Butto = false, Npc_Color_Visble = false, Npc_Color_InVisble = false, Option = true;
			SendKey = false;
		}
	}
	else {
		SendKey = true;
	}
}

static void DrawButtonHotKey(float x, float y, float w, float h, int& HotKey, char* txt) {
	renderer->DrawBox(x, y, w, h, { 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 0.800000000f }, { 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 0.800000000f });
	renderer->Draw2dText(x + (w / 2.0f), y + (h / 2.0f), (Tools->isMouseinRegion(x, y, x + w, y + h) ? Colors::Purple.v : Colors::White.v), 0.45f, true, txt);
	static bool SendKey = false;
	if (GetlButton == 1) {
		if (Tools->isMouseinRegion(x, y, x + w, y + h)) {
			HotKey = 0x0;
		}
	}
	else {
		if (HotKey == 0) {
			for (BYTE i = 0; i < 0xFF; i++)
				if (i != 255 && i != 0x1 && i != 0x28 && i != 0x0 && i != 0xB3)
					if (GetKeyState(i) & 0x8000)i == VK_BACK ? HotKey = 0x997 : HotKey = i, SendKey = false;
		}
		SendKey = true;
	}
}
void D3DMenu::AddItem(const char* txt, const char** opt, int maxval, int& var) {
	renderer->Draw2dText(pMenu->x + 10.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.45f, false, txt);
	if (Tools->isMouseinRegion(pMenu->x + 280.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 420.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f)) {
		if (var < maxval && MouseWheel < 0.0f)
			var += 1, MouseWheel = 0.0f;
		if (var > 0 && MouseWheel > 0.0f)
			var -= 1, MouseWheel = 0.0f;
	}

	static bool SendKey = false;
	if (GetlButton) {
		if (Tools->isMouseinRegion(pMenu->x + 280.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 350.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f) && var > 0 && SendKey == true)
			SendKey = false, var -= 1;
		if (Tools->isMouseinRegion(pMenu->x + 350.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 420.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f) && var < maxval && SendKey == true)
			SendKey = false, var += 1;
	}
	else {
		SendKey = true;
	}
	renderer->Draw2dText(pMenu->x + 300.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.65f, true, "-");
	renderer->Draw2dText(pMenu->x + 350.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), !var ? Colors::Red.v : Colors::Lime.v, 0.55f, true, (char*)opt[var]);
	renderer->Draw2dText(pMenu->x + 400.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.65f, true, "+");

	renderer->DrawLine(pMenu->x + 10.0f, pMenu->y + 27.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 405.0f, pMenu->y + 27.0f + ((float)pMenu->noitems * 20.0f), Colors::Cyan.v);
	this->noitems++;
}

void D3DMenu::AddintRange(const char* txt, int& var, int min, int max, int Step) {
	renderer->Draw2dText(pMenu->x + 10.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.45f, false, txt);
	if (Tools->isMouseinRegion(pMenu->x + 280.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 420.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f)) {
		if (var < max && MouseWheel < 0.0f)
			var += Step, MouseWheel = 0.0f;
		if (var > min && MouseWheel > 0.0f)
			var -= Step, MouseWheel = 0.0f;
	}
	static bool SendKey = false;
	if (GetlButton) {
		if (Tools->isMouseinRegion(pMenu->x + 280.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 350.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f) && var > min)
			var -= Step, SendKey = false;
		if (Tools->isMouseinRegion(pMenu->x + 350.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 420.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f) && var < max)
			var += Step, SendKey = false;
	}
	else {
		SendKey = true;
	}
	renderer->Draw2dText(pMenu->x + 300.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.65f, true, "-");
	renderer->Draw2dText(pMenu->x + 350.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), !var ? Colors::Red.v : Colors::Lime.v, 0.55f, true, "%d", var);
	renderer->Draw2dText(pMenu->x + 400.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.65f, true, "+");
	renderer->DrawLine(pMenu->x + 10.0f, pMenu->y + 27.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 405.0f, pMenu->y + 27.0f + ((float)pMenu->noitems * 20.0f), Colors::Cyan.v);
	this->noitems++;
}

void D3DMenu::AddfloatRange(const char* txt, float& var, float min, float max, float Step, const char* DisplayMode) {
	renderer->Draw2dText(pMenu->x + 10.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.45f, false, txt);
	if (Tools->isMouseinRegion(pMenu->x + 280.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 420.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f)) {
		if (var < max && MouseWheel < 0.0f)
			var += Step, MouseWheel = 0.0f;
		if (var > min && MouseWheel > 0.0f)
			var -= Step, MouseWheel = 0.0f;
	}

	static bool SendKey = false;
	if (GetlButton) {
		if (Tools->isMouseinRegion(pMenu->x + 280.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 350.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f) && var > min)
			var -= Step, SendKey = false;
		if (Tools->isMouseinRegion(pMenu->x + 350.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 420.0f, pMenu->y + 10.0f + ((float)pMenu->noitems * 20.0f) + 20.0f) && var < max)
			var += Step, SendKey = false;
	}
	else {
		SendKey = true;
	}
	renderer->Draw2dText(pMenu->x + 300.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.65f, true, "-");

	renderer->Draw2dText(pMenu->x + 350.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), !var ? Colors::Red.v : Colors::Lime.v, 0.55f, true, DisplayMode, var);

	renderer->Draw2dText(pMenu->x + 400.0f, pMenu->y + 18.0f + ((float)pMenu->noitems * 20.0f), Colors::White.v, 0.65f, true, "+");

	renderer->DrawLine(pMenu->x + 10.0f, pMenu->y + 27.0f + ((float)pMenu->noitems * 20.0f), pMenu->x + 405.0f, pMenu->y + 27.0f + ((float)pMenu->noitems * 20.0f), Colors::Cyan.v);
	this->noitems++;
}

const char* opt_OffOn[] = { ("Off"), ("On") };
void D3DMenu::AddOnOff(const char* txt, int& var)
{
	AddItem(txt, opt_OffOn, 1, var);
}

enum WeatherTypes {
	EXTRASUNNY,
	CLEAR,
	CLOUDS,
	SMOG,
	FOGGY,
	OVERCAST,
	RAIN,
	THUNDER,
	CLEARING,
	NEUTRAL,
	SNOW,
	BLIZZARD,
	SNOWLIGHT,
	XMAS,
	HALLOWEEN
};

void MakeMenu() {
	//renderer->FillRect(100, 100, 100, 100, Colors::Red.v);
	renderer->FillRect(pMenu->x - 2.0f, pMenu->y - 60.0f, pMenu->width + 2.0f, 30.0f, { 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 0.970000000f });

	renderer->Draw2dText(Players_Menu == 1 ? pMenu->x + 30.0f : pMenu->x + (pMenu->width / 2.0f), pMenu->y - 46.0f, Colors::White.v, 0.5f, Players_Menu == 1 ? false : true, "PhantomCheats for GTA5");
	//Draw Background

	renderer->FillRect(pMenu->x - 2.0f, pMenu->y - 30.0f, pMenu->width + 2.0f, pMenu->hight + 2.0f, { 5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 0.970000000f });

	int ButtonCount = 0;
	static int Page = 0;

	//DrawButton(pMenu->x, pMenu->y - 30.0f, 70.0f, 30.0f, "Player", &Player_Hacks), ButtonCount++;

	if (Page == 0)
		DrawButton(pMenu->x, pMenu->y - 30.0f, 70.0f, 30.0f, "Player", Player_Hacks), ButtonCount++;


	DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "Weapon", Weapon_Menu), ButtonCount++;
	DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "Vehicle", Vehicle_Menu), ButtonCount++;
	DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "Aimbot", Aimbot_Menu), ButtonCount++;
	DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "3D Radar", Npc_Menu);  ButtonCount++;
	DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "2D Radar", Radar_Menu); ButtonCount++;
	DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "Misc", Other_Menu); ButtonCount++;

	if (Page == 1)
		DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "LIST", Players_Menu), ButtonCount++;

	if (Page == 2)
		DrawButton(pMenu->x + float(ButtonCount * 70.0f), pMenu->y - 30.0f, 70.0f, 30.0f, "WATER", Water_Menu), ButtonCount++;

	renderer->FillRect(pMenu->x, pMenu->y - (1.5f / 2.0f), pMenu->width, 1.5f, Colors::Cyan.v);

	//if (Tools->isMouseinRegion(pMenu->x, pMenu->y - 30.0f, pMenu->x + pMenu->width, pMenu->y)) {
	//	if (MouseWheel == -1.0f && Page != 0)Page -= 1;
	//	if (MouseWheel == 1.0f && Page < 2)Page += 1;
	//}
	if (!Player_Hacks && !Aimbot_Menu && !Weapon_Menu && !Vehicle_Menu && !Npc_Menu && !Radar_Menu && !Other_Menu && !Players_Menu && !Water_Menu) {
		renderer->Draw2dText(pMenu->x + 5.0f, pMenu->y + 18.0f, Colors::White.v, 0.6f, false, SigFountCount < 35 ? "Loading Found %d of %d" : "Welcome to PhantomCheats :)", SigFountCount, 35);
	}

	if (SigFountCount < 35)
		Player_Hacks = 0, Players_Menu = 0, Aimbot_Menu = 0, Weapon_Menu = 0, Vehicle_Menu = 0, Npc_Menu = 0, Radar_Menu = 0, Other_Menu = 0;

	if (Players_Menu)
		pMenu->width = 800.0f;
	else
		pMenu->width = 492.0f, pMenu->hight = 360.0f;

	if (Player_Hacks) {
		pMenu->AddOnOff("Information", Hacks->PlayerInfoCart);
		pMenu->AddItem("Crosshair", opt_Crosshair, 2, Hacks->Crosshair);
		pMenu->AddItem("Unlimited Health", opt_GodMode_Toggle, 3, Hacks->God_Mode);
		pMenu->AddOnOff("No Collision", Hacks->NoPlayerCollision);
		pMenu->AddOnOff("No Ragdoll", Hacks->NoRagdoll);
		//////if (!Hacks->Save_Mode)pMenu->AddOnOff("Invisible", Hacks->Invisible);
		pMenu->AddItem("Wanted Level", opt_WantedLvl, 2, Hacks->Wanted);
		pMenu->AddOnOff("No Gravity", Hacks->NoGravity);
		pMenu->AddOnOff("Pickup ESP", Hacks->PickUpEsp);
		pMenu->AddOnOff("Vehicle ESP", Hacks->VehicleEsp);

		if (Hacks->VehicleEsp) {
			pMenu->AddOnOff(" Name", Hacks->VehicleEsp_Name);
			pMenu->AddOnOff(" Distance", Hacks->VehicleEsp_Distance);
			pMenu->AddOnOff(" 3D Box", Hacks->VehicleEsp_3Dbox);
		}
		//pMenu->AddItem("Object ESP", &Hacks->ObjectEsp, opt_ObjectEsp, 3);

		////if (!Hacks->Save_Mode){

		float addPy = !Hacks->VehicleEsp ? 200.0f : 250.0f;

		DrawButton(pMenu->x + 5.0f, pMenu->y + addPy, 100.0f, 35.0f, "Tp To Way", TpToWay); Player_Hacks = true;
		DrawButton(pMenu->x + 5.0f + (103.0f * 1.0f), pMenu->y + addPy, 100.0f, 35.0f, "Reset HP", Hacks->ResetHP); Player_Hacks = true;
		DrawButton(pMenu->x + 5.0f + (103.0f * 2.0f), pMenu->y + addPy, 100.0f, 35.0f, "Reset Armour", Hacks->ResetArmour); Player_Hacks = true;

		static char cTeleHotKey[128] = "\x00";
		sprintf_s(cTeleHotKey, (Hacks->HotKeyTeleportToWayP ? "0x%x" : "Waiting..."), Hacks->HotKeyTeleportToWayP);
		DrawButtonHotKey(pMenu->x + 5.0f, pMenu->y + addPy + 40.0f, 100.0f, 28.0f, Hacks->HotKeyTeleportToWayP, cTeleHotKey);

		static char cResetHealth[128] = "\x00";
		sprintf_s(cResetHealth, (Hacks->HotKeyResetHealth ? "0x%x" : "Waiting..."), Hacks->HotKeyResetHealth);
		DrawButtonHotKey(pMenu->x + 5.0f + (103.0f * 1.0f), pMenu->y + addPy + 40.0f, 100.0f, 28.0f, Hacks->HotKeyResetHealth, cResetHealth);

		static char cResetArmour[128] = "\x00";
		sprintf_s(cResetArmour, (Hacks->HotKeyResetArmour ? "0x%x" : "Waiting..."), Hacks->HotKeyResetArmour);
		DrawButtonHotKey(pMenu->x + 5.0f + (103.0f * 2.0f), pMenu->y + addPy + 40.0f, 100.0f, 28.0f, Hacks->HotKeyResetArmour, cResetArmour);
		//}
	}

	if (Vehicle_Menu) {
		pMenu->AddfloatRange("Veh Impulse", Hacks->Veh_Impulse, 1.0f, 10.0f, 0.5f, "%1.1f");
		if (!Hacks->Save_Mode)pMenu->AddOnOff("God Mode", Hacks->Veh_God_Mode);
		pMenu->AddOnOff("Show Last Vehicle", Hacks->DrawLastVeh);
		pMenu->AddOnOff("Unlock Locked Cars", Hacks->UseLockedCars);
		pMenu->AddOnOff("Fix On damage(Maybe crash)", Hacks->FixOnDmg);
		pMenu->AddOnOff("No Fall Off", Hacks->NoFallOfVeh);

		DrawButton(pMenu->x, pMenu->y + 130.0f, 100.0f, 28.0f, "Fix Car", FixCar), Vehicle_Menu = true;
		DrawButton(pMenu->x + 103.0f, pMenu->y + 130.0f, 100.0f, 28.0f, "Upgrade Car", UpgradeCar), Vehicle_Menu = true;
		//DrawButton(pMenu->x + 206.0f, pMenu->y + 130.0f, 100.0f, 28.0f, "Tp To Near", &TpToNearVeh), Vehicle_Menu = true;

		DrawButton(pMenu->x, pMenu->y + 130.0f + 32.0f, 100.0f, 28.0f, "Tp Last", TpToMe), Vehicle_Menu = true;
		DrawButton(pMenu->x + 103.0f, pMenu->y + 130.0f + 32.0f, 100.0f, 28.0f, "Flip", FlipVeh), Vehicle_Menu = true;
		//DrawButton(pMenu->x + 206.0f, pMenu->y + 130.0f + (32.0f * 2.0f), 100.0f, 28.0f, "Spawn", &SpawnVeh), Vehicle_Menu = true;

		if (!VK_Shift_PRESS)
			DrawButton(pMenu->x + 206.0f, pMenu->y + 130.0f + 32.0f, 100.0f, 28.0f, "Tp In Near", Hacks->TpInNearVeh), Vehicle_Menu = true;
		else
			DrawButton(pMenu->x + 206.0f, pMenu->y + 130.0f + 32.0f, 100.0f, 28.0f, "Tp As Passanger", TpInNearVeh_Passanger), Vehicle_Menu = true;


		DrawButton(pMenu->x, pMenu->y + 130.0f + (32.0f * 2.0f), 100.0f, 28.0f, "Start", StartEngine), Vehicle_Menu = true;

		static char cFlipVeh[128] = "\x00";
		sprintf_s(cFlipVeh, (Hacks->HotKeyFlipVeh ? "0x%x" : "Waiting..."), Hacks->HotKeyFlipVeh);
		DrawButtonHotKey(pMenu->x + 103.0f, pMenu->y + 110.0f + 44.0f + 44.0f, 100.0f, 28.0f, Hacks->HotKeyFlipVeh, cFlipVeh);


		renderer->Draw2dText(pMenu->x, pMenu->y + 160.0f + 44.0f + 44.0f - 5.0f, Colors::White.v, 0.5f, false, "Veh_Increace");
		static char cVehImpulseIncreace[128] = "\x00";
		sprintf_s(cVehImpulseIncreace, (Hacks->HotKeyVehImpulseIncreace ? "0x%x" : "Waiting..."), Hacks->HotKeyVehImpulseIncreace);
		DrawButtonHotKey(pMenu->x, pMenu->y + 160.0f + 44.0f + 44.0f, 100.0f, 28.0f, Hacks->HotKeyVehImpulseIncreace, cVehImpulseIncreace);

		renderer->Draw2dText(pMenu->x + 103.0f, pMenu->y + 160.0f + 44.0f + 44.0f - 5.0f, Colors::White.v, 0.5f, false, "Veh_Decreace");
		static char cVehImpulseDecreace[128] = "\x00";
		sprintf_s(cVehImpulseDecreace, (Hacks->HotKeyVehImpulseDecrease ? "0x%x" : "Waiting..."), Hacks->HotKeyVehImpulseDecrease);
		DrawButtonHotKey(pMenu->x + 103.0f, pMenu->y + 160.0f + 44.0f + 44.0f, 100.0f, 28.0f, Hacks->HotKeyVehImpulseDecrease, cVehImpulseDecreace);

		renderer->Draw2dText(pMenu->x + 206.0f, pMenu->y + 160.0f + 44.0f + 44.0f - 5.0f, Colors::White.v, 0.5f, false, "Veh_Constant");
		static char cVehImpulseConstant[128] = "\x00";
		sprintf_s(cVehImpulseConstant, (Hacks->HotKeyVehImpulseConstant ? "0x%x" : "Waiting..."), Hacks->HotKeyVehImpulseConstant);
		DrawButtonHotKey(pMenu->x + 206.0f, pMenu->y + 160.0f + 44.0f + 44.0f, 100.0f, 28.0f, Hacks->HotKeyVehImpulseConstant, cVehImpulseConstant);
	}

	if (Aimbot_Menu) {
		pMenu->AddfloatRange("Max Distance", Hacks->Max_Aimbot_Distance, 20.0f, 1000.0f, 20.0f, "%1.0f");
		pMenu->AddItem("Toggle", opt_Aimbot_Toggle, 2, Hacks->Aimbot_Toggle);
		pMenu->AddItem("Bone", opt_Aimbot_Bone, 4, Hacks->Aimbot_Bone);
		pMenu->AddOnOff("Stick On Target", Hacks->Aimbot_StickTo);
		if (!Hacks->Save_Mode)pMenu->AddItem("Silent Aim", opt_Aimbot_Silent, 2, Hacks->Aimbot_Silent);
		pMenu->AddOnOff("Draw Fov", Hacks->DrawFov);
		pMenu->AddfloatRange("Limit Fov", Hacks->Aimbot_Fov, 20.0f, 600.0f, 10.0f, "%1.0f");
		pMenu->AddfloatRange("Prediction Multiplier", Hacks->Aimbot_Prediction, 0.0000f, 1.5000f, 0.001f, "%1.4f");
		pMenu->AddfloatRange("Smooth", Hacks->Aimbot_Smooth, 0.0f, 40.0f, 1.0f, "%1.1f");
		//pMenu->AddOnOff("Linear", Hacks->Aimbot_Linear);
		pMenu->AddOnOff("Horizontal Only", Hacks->Aimbot_HorizontalOnly);
		pMenu->AddItem("Target Selection", opt_TargetSelection, 3, Hacks->TargetSelectionAim);
		static char cAimHotKey[128] = "\x00";
		sprintf_s(cAimHotKey, (Hacks->HotKeyAim ? "0x%x" : "Waiting..."), Hacks->HotKeyAim);
		DrawButtonHotKey(pMenu->x, pMenu->y + 230.0f, 100.0f, 28.0f, Hacks->HotKeyAim, cAimHotKey);

		static char cAimHotKey2[128] = "\x00";
		sprintf_s(cAimHotKey2, (Hacks->HotKeyAim2 ? "0x%x" : "Waiting..."), Hacks->HotKeyAim2);
		DrawButtonHotKey(pMenu->x + 105.0f, pMenu->y + 230.0f, 100.0f, 28.0f, Hacks->HotKeyAim2, cAimHotKey2);
	}

	if (Weapon_Menu) {
		pMenu->AddItem("No Recoil", opt_Recoil, 3, Hacks->No_Recoil);
		if (Hacks->No_Recoil == 3) {
			pMenu->AddfloatRange(" Recoil Value", Hacks->No_Recoil_Coustum, 0.0f, 2.0f, 0.02f, "%1.2f");
		}
		pMenu->AddItem("No Spread", opt_Spread, 2, Hacks->No_Spread);
		if (Hacks->No_Spread == 2) {
			pMenu->AddfloatRange(" Spread Value", Hacks->No_Spread_coustum, 0.0f, 2.0f, 0.02f, "%1.2f");
		}
		pMenu->AddOnOff("No Reload", Hacks->Fast_Reload);
		pMenu->AddOnOff("Super Velocity", Hacks->Super_Velocity);
		pMenu->AddOnOff("Super Dmg", Hacks->Super_Dmg);
		pMenu->AddOnOff("Sniper Range", Hacks->Sniper_Range);
		pMenu->AddOnOff("Super Impulse", Hacks->Super_Impulse);

		if (!Hacks->Save_Mode)pMenu->AddOnOff("Rapide Fire", Hacks->Rapide_Fire);
		if (!Hacks->Save_Mode)pMenu->AddItem("Bullet Type", opt_Bullet_Type, 10, Hacks->Bullet_Type);
		if (!Hacks->Save_Mode)pMenu->AddintRange("Fire Interval in MS", Hacks->Fire_Interval, 0, 400, 2);
		pMenu->AddintRange("Set ammo", Hacks->Set_Ammo, 0, 10000, 200);

		//if(!Hacks->Save_Mode){
		DrawButton(pMenu->x + 5.0f, pMenu->y + 270.0f, 100.0f, 23.0f, "Give Weapons", GiveWeaponB), Weapon_Menu = true;
		DrawButton(pMenu->x + 5.0f, pMenu->y + 300.0f, 100.0f, 23.0f, "Give Specific", GiveCoustumWeaponB), Weapon_Menu = true;
		DrawButton(pMenu->x + 205.0f, pMenu->y + 270.0f, 100.0f, 23.0f, "Set Ammo", SetPedAmmo), Weapon_Menu = true;
		DrawButton(pMenu->x + 105.0f, pMenu->y + 270.0f, 100.0f, 23.0f, "Remove All", Remove_All), Weapon_Menu = true;
		DrawButton(pMenu->x + 105.0f, pMenu->y + 300.0f, 100.0f, 23.0f, "Remove Cur", Remove_InHand), Weapon_Menu = true;
		DrawButton(pMenu->x + 205.0f, pMenu->y + 300.0f, 100.0f, 23.0f, "Take Sucide", Take_Sucide), Weapon_Menu = true;
		//}
	}

	if (Npc_Menu) {
		pMenu->AddfloatRange("Max Distance", Hacks->Npc_Max_Distance, 0.0f, 10000.0f, 100.0f, "%1.0f");
		pMenu->AddItem("Names", opt_NameHEsp, 2, Hacks->Npc_Name_Esp);
		//pMenu->AddOnOff("Wanted Level", &Hacks->Npc_Wanted_Esp);
		pMenu->AddOnOff("Distance", Hacks->Npc_Distance_Esp);
		pMenu->AddOnOff("Weapon", Hacks->Npc_Weapon_Esp);
		pMenu->AddOnOff("Barrel", Hacks->Npc_Barrel_Esp);
		pMenu->AddItem("Box", opt_BoxEsp, 3, Hacks->Npc_Box_Esp_Type);
		pMenu->AddOnOff("Skeleton", Hacks->Npc_Bone_Esp);
		pMenu->AddItem("Health", opt_HealthEsp, 3, Hacks->Npc_Health_Esp);
		pMenu->AddOnOff("Head Dot", Hacks->Npc_HeadDot_Esp);
		pMenu->AddfloatRange("Dot Size", Hacks->Npc_HeadDot_Size, 2, 6, 1, "%1.1f");
		pMenu->AddfloatRange("Line Size", Hacks->Npc_Line_Size, 1, 6, 1, "%1.1f");
		pMenu->AddItem("Targeting", opt_TargetSelection, 3, Hacks->TargetSelection);
	}
	if (Radar_Menu) {
		pMenu->AddOnOff("Toggle", Hacks->Toggle_2D_Radar);
		pMenu->AddOnOff("Health", Hacks->Health2dRadar);
		pMenu->AddOnOff("Center", Hacks->Center2d);
		pMenu->AddOnOff("Background", Hacks->BackGround);
		pMenu->AddfloatRange("Size", Hacks->RadarSize, 1.0f, 20.0f, 1.0f, "%1.0f");
		pMenu->AddfloatRange("Zoom", Hacks->RadarZoom, 1.0f, 20.0f, 1.0f, "%1.0f");
		pMenu->AddItem("Target Selection", opt_TargetSelection, 3, Hacks->TargetSelectionRadar);
	}

	if (Other_Menu) {
		pMenu->AddItem("Raycasting", opt_RayCasting, 4, Hacks->UseRayCast);
		pMenu->AddOnOff("Fly Hack", Hacks->FlyHack);
		pMenu->AddfloatRange("Fly Speed", Hacks->FlySpeed, 1.0f, 40.0f, 1.0f, "%1.0f");
		pMenu->AddOnOff("No Clipping", Hacks->NoClip);
		pMenu->AddOnOff("Draw WayPoint", Hacks->WayPointEsp);
		////if (!Hacks->Save_Mode)pMenu->AddItem("Freeze Lobby", &Hacks->LobbyStresser, opt_FreezeLobby, 3);
		//pMenu->AddOnOff("Save Mode", Hacks->Save_Mode);
		pMenu->AddfloatRange("Esp Font Size", Hacks->Esp_Font_Size, 0.2f, 1.0f, 0.02f, "%1.2f");
		////pMenu->AddOnOff("CG mode", &Hacks->ClassicGangWarMode);
		pMenu->AddItem("Text Shadows", opt_Text_Shadows, 3, Hacks->Text_Shadow);
		pMenu->AddOnOff("Rage Names", Hacks->RageNames);

		static char cFlyHotKey[128] = "\x00";
		static char cNoClipHotKey[128] = "\x00";
		sprintf_s(cFlyHotKey, (Hacks->HotKeyFly ? "0x%x" : "Waiting..."), Hacks->HotKeyFly);
		DrawButtonHotKey(pMenu->x, pMenu->y + 180.0f, 100.0f, 23.0f, Hacks->HotKeyFly, cFlyHotKey);

		sprintf_s(cNoClipHotKey, (Hacks->HotKeyNoClip ? "0x%x" : "Waiting..."), Hacks->HotKeyNoClip);
		DrawButtonHotKey(pMenu->x + 110.0f, pMenu->y + 180.0f, 100.0f, 23.0f, Hacks->HotKeyNoClip, cNoClipHotKey);

		DrawButton(pMenu->x + (110.0f * 2.0f), pMenu->y + 230.0f, 100.0f, 23.0f, "All Off", Hacks->Panic_DisableAll); Other_Menu = 1;
		DrawButton(pMenu->x + (110.0f * 2.0f), pMenu->y + 230.0f + 30.0f, 100.0f, 23.0f, "Exit", handle_unload), Other_Menu = 1;

		static char cOpenMenu[128] = "\x00";
		sprintf_s(cOpenMenu, (Hacks->OpenMenuKey ? "0x%x" : "Waiting..."), Hacks->OpenMenuKey);
		DrawButtonHotKey(pMenu->x + (110.0f * 2.0f) + 102.0f, pMenu->y + 260.0f + 30.0f, 100.0f, 23.0f, Hacks->OpenMenuKey, cOpenMenu);

		DrawButton(pMenu->x, pMenu->y + 180.0f + 30.0f, 100.0f, 23.0f, Save ? "Saved" : "Save Settings 1", Save), Other_Menu = 1;
		DrawButton(pMenu->x + 110.0f, pMenu->y + 180.0f + 30.0f, 100.0f, 23.0f, Load ? "Loaded" : "Load Settings 1", Load), Other_Menu = 1;

		DrawButton(pMenu->x, pMenu->y + 180.0f + 60.0f, 100.0f, 23.0f, Save2 ? "Saved" : "Save Settings 2", Save2), Other_Menu = 1;
		DrawButton(pMenu->x + 110.0f, pMenu->y + 180.0f + 60.0f, 100.0f, 23.0f, Load2 ? "Loaded" : "Load Settings 2", Load2), Other_Menu = 1;

		DrawButton(pMenu->x, pMenu->y + 180.0f + 90.0f, 100.0f, 23.0f, Save3 ? "Saved" : "Save Settings 3", Save3), Other_Menu = 1;
		DrawButton(pMenu->x + 110.0f, pMenu->y + 180.0f + 90.0f, 100.0f, 23.0f, Load3 ? "Loaded" : "Load Settings 3", Load3), Other_Menu = 1;


	}

	if (Water_Menu) {
		pMenu->AddOnOff("Glass Water", Hacks->GlassWater);
		pMenu->AddOnOff("No Water", Hacks->NoWater);
	}
}