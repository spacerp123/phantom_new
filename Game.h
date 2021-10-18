#pragma once
#include "Classes.h"
#include "Tools.h"
#include "D3D11Renderer.h"

extern std::unique_ptr<Options> Hacks;

struct MemoryPool
{
	uintptr_t ListAddr;
	char* BoolAdr;
	int MaxCount;
	int ItemSize;
};

uintptr_t get_address_of_item_in_pool(MemoryPool* pool, int handle)
{
	if (pool == nullptr)
		return 0;

	DWORD64 index = handle >> 8;
	int flag = pool->BoolAdr[index];

	if (flag & 0x80 || flag != (handle & 0xFF))
		return 0;

	return pool->ListAddr + index * pool->ItemSize;
}
static MemoryPool** m_entityPool;

CObject* handle_to_ptr(int handle)
{
	uintptr_t	ptr = get_address_of_item_in_pool(*m_entityPool, handle);
	if (ptr == 0)
		return nullptr;
	return *reinterpret_cast<CObject**>(ptr + 8);
}

#pragma pack(push, 1)
typedef struct PVector3
{
	float x;
	DWORD _paddingx;
	float y;
	DWORD _paddingy;
	float z;
	DWORD _paddingz;

	PVector3(float x, float y, float z) :
		x(x), y(y), z(z),
		_paddingx(0), _paddingy(0), _paddingz(0)
	{
	}
	PVector3() noexcept : PVector3(0.f, 0.f, 0.f) {}
}PVector3;
#pragma pack(pop)

Vector3 transformRotToDir(Vector3 In)
{
	float	a = XMConvertToRadians(In.x),
		b = XMConvertToRadians(In.z),
		c = cos(a);
	return Vector3((float)-(c * sin(b)), (float)(c * cos(b)), (float)sin(a));
}

Vector3 GetCoordsInfrontOfCam(Vector3 CamPos, Vector3 CamRot, float distance) {
	return CamPos + CamRot * distance;
}

#pragma region Bones
#define SKEL_ROOT 0x0
#define SKEL_Pelvis 0x2e28
#define SKEL_L_Thigh 0xe39f
#define SKEL_L_Calf 0xf9bb
#define SKEL_L_Foot 0x3779
#define SKEL_L_Toe0 0x83c
#define IK_L_Foot 0xfedd
#define PH_L_Foot 0xe175
#define MH_L_Knee 0xb3fe
#define SKEL_R_Thigh 0xca72
#define SKEL_R_Calf 0x9000
#define SKEL_R_Foot 0xcc4d
#define SKEL_R_Toe0 0x512d
#define IK_R_Foot 0x8aae
#define PH_R_Foot 0x60e6
#define MH_R_Knee 0x3fcf
#define RB_L_ThighRoll 0x5c57
#define RB_R_ThighRoll 0x192a
#define SKEL_Spine_Root 0xe0fd
#define SKEL_Spine0 0x5c01
#define SKEL_Spine1 0x60f0
#define SKEL_Spine2 0x60f1
#define SKEL_Spine3 0x60f2
#define SKEL_L_Clavicle 0xfcd9
#define SKEL_L_UpperArm 0xb1c5
#define SKEL_L_Forearm 0xeeeb
#define SKEL_L_Hand 0x49d9
#define SKEL_L_Finger00 0x67f2
#define SKEL_L_Finger01 0xff9
#define SKEL_L_Finger02 0xffa
#define SKEL_L_Finger10 0x67f3
#define SKEL_L_Finger11 0x1049
#define SKEL_L_Finger12 0x104a
#define SKEL_L_Finger20 0x67f4
#define SKEL_L_Finger21 0x1059
#define SKEL_L_Finger22 0x105a
#define SKEL_L_Finger30 0x67f5
#define SKEL_L_Finger31 0x1029
#define SKEL_L_Finger32 0x102a
#define SKEL_L_Finger40 0x67f6
#define SKEL_L_Finger41 0x1039
#define SKEL_L_Finger42 0x103a
#define PH_L_Hand 0xeb95
#define IK_L_Hand 0x8cbd
#define RB_L_ForeArmRoll 0xee4f
#define RB_L_ArmRoll 0x1470
#define MH_L_Elbow 0x58b7
#define SKEL_R_Clavicle 0x29d2 //schulter oder auch Schl�sselbein
#define SKEL_R_UpperArm 0x9d4d
#define SKEL_R_Forearm 0x6e5c
#define SKEL_R_Hand 0xdead
#define SKEL_R_Finger00 0xe5f2
#define SKEL_R_Finger01 0xfa10
#define SKEL_R_Finger02 0xfa11
#define SKEL_R_Finger10 0xe5f3
#define SKEL_R_Finger11 0xfa60
#define SKEL_R_Finger12 0xfa61
#define SKEL_R_Finger20 0xe5f4
#define SKEL_R_Finger21 0xfa70
#define SKEL_R_Finger22 0xfa71
#define SKEL_R_Finger30 0xe5f5
#define SKEL_R_Finger31 0xfa40
#define SKEL_R_Finger32 0xfa41
#define SKEL_R_Finger40 0xe5f6
#define SKEL_R_Finger41 0xfa50
#define SKEL_R_Finger42 0xfa51
#define PH_R_Hand 0x6f06
#define IK_R_Hand 0x188e
#define RB_R_ForeArmRoll 0xab22
#define RB_R_ArmRoll 0x90ff
#define MH_R_Elbow 0xbb0
#define SKEL_Neck_1 0x9995
#define SKEL_Head 0x796e
#define IK_Head 0x322c
#define FACIAL_facialRoot 0xfe2c
#define FB_L_Brow_Out_000 0xe3db
#define FB_L_Lid_Upper_000 0xb2b6
#define FB_L_Eye_000 0x62ac
#define FB_L_CheekBone_000 0x542e
#define FB_L_Lip_Corner_000 0x74ac
#define FB_R_Lid_Upper_000 0xaa10
#define FB_R_Eye_000 0x6b52
#define FB_R_CheekBone_000 0x4b88
#define FB_R_Brow_Out_000 0x54c
#define FB_R_Lip_Corner_000 0x2ba6
#define FB_Brow_Centre_000 0x9149
#define FB_UpperLipRoot_000 0x4ed2
#define FB_UpperLip_000 0xf18f
#define FB_L_Lip_Top_000 0x4f37
#define FB_R_Lip_Top_000 0x4537
#define FB_Jaw_000 0xb4a0
#define FB_LowerLipRoot_000 0x4324
#define FB_LowerLip_000 0x508f
#define FB_L_Lip_Bot_000 0xb93b
#define FB_R_Lip_Bot_000 0xc33b
#define FB_Tongue_000 0xb987
#define RB_Neck_1 0x8b93
#define IK_Root 0xdd1c
#define STOMACH           0x60F0
#define BONETAG_HEAD		0x796E 
#define BONETAG_CHEST		0x60F2
#pragma endregion

typedef int(__fastcall* CLEAR_PED_TASKS_IMMEDIATELY_t)(DWORD ped);
CLEAR_PED_TASKS_IMMEDIATELY_t oCLEAR_PED_TASKS_IMMEDIATELY = NULL;

typedef BOOL(__fastcall* GetBoneFromMask2)(CObject* pThis, __m128* vBonePos, unsigned int wMask);
GetBoneFromMask2 GetBone2;
Vector3 FromM128(__m128 in) { return Vector3(in.m128_f32[0], in.m128_f32[1], in.m128_f32[2]); }


Vector3 GetBonePosition2Vec(CObject* pThis, unsigned int wMask) {
	__m128 a1;
	GetBone2(pThis, &a1, wMask);
	return FromM128(a1);
}

PVector3 ToPVector3(Vector3 pVec) {
	return PVector3(pVec.x, pVec.y, pVec.z);
}
Vector3 ToVector3(PVector3 pVec) {
	return Vector3(pVec.x, pVec.y, pVec.z);
}

extern bool SetPedAmmo;
extern bool GiveWeaponB;
extern bool GiveCoustumWeaponB;
extern bool GiveAmmoB;

float ScreenWidth;
float ScreenHight;
float ScreenCenterX;
float ScreenCenterY;

BOOL W2S(Vector3 WorldPos, Vector3& ScrPos) {
	if (!Tools->IsValid(m_viewPort))
		return 0;

	if (WorldPos.x == 0.0f)
		return 0;

	Matrix matrixNew = m_viewPort->m_viewProj; //024c
	matrixNew.Transpose(matrixNew);

	Vector4 vForward = Vector4{ matrixNew._41 , matrixNew._42,  matrixNew._43, matrixNew._44 };
	Vector4 vRight = Vector4{ matrixNew._21, matrixNew._22, matrixNew._23, matrixNew._24 };
	Vector4 vUpward = Vector4{ matrixNew._31, matrixNew._32, matrixNew._33, matrixNew._34 };

	ScrPos.x = (vRight.x * WorldPos.x) + (vRight.y * WorldPos.y) + (vRight.z * WorldPos.z) + vRight.w;
	ScrPos.y = (vUpward.x * WorldPos.x) + (vUpward.y * WorldPos.y) + (vUpward.z * WorldPos.z) + vUpward.w;
	ScrPos.z = (vForward.x * WorldPos.x) + (vForward.y * WorldPos.y) + (vForward.z * WorldPos.z) + vForward.w;

	if (ScrPos.z < 0.01f) {
		return 0;
	}


	float invw = 1.0f / ScrPos.z;
	ScrPos.x *= invw;
	ScrPos.y *= invw;

	float xTmp = ScreenWidth / 2.0f;
	float yTmp = ScreenHight / 2.0f;

	xTmp += (0.5f * ScrPos.x * ScreenWidth + 0.5f);
	yTmp -= (0.5f * ScrPos.y * ScreenHight + 0.5f);

	ScrPos.x += xTmp;
	ScrPos.y = yTmp;
	return 1;
}
DWORD64 WorldToScreenAddress = 0x0;

void DrawBone(Vector3 bone1, Vector3 bone2, Color BoneColor) {
	Vector3 BonePos1W2S(0.0f, 0.0f, 0.0f);
	Vector3 BonePos2W2S(0.0f, 0.0f, 0.0f);
	if (W2S(bone1, BonePos1W2S) && W2S(bone2, BonePos2W2S))
		renderer->DrawLine(BonePos1W2S.x, BonePos1W2S.y, BonePos2W2S.x, BonePos2W2S.y, BoneColor);
}

enum BoneTypesForEsp {
	pSKEL_Head,
	pSKEL_Neck_1,
	pSKEL_L_UpperArm,
	pMH_L_Elbow,
	pSKEL_L_Hand,
	pSKEL_R_UpperArm,
	pMH_R_Elbow,
	pSKEL_R_Hand,
	pSTOMACH,
	pSKEL_R_Thigh,
	pSKEL_R_Calf,
	pSKEL_R_Foot,
	pSKEL_L_Thigh,
	pSKEL_L_Calf,
	pSKEL_L_Foot
};

unsigned int EspBoneType[15] = {
	SKEL_Head, SKEL_Neck_1, SKEL_L_UpperArm, MH_L_Elbow, SKEL_L_Hand, SKEL_R_UpperArm, MH_R_Elbow, SKEL_R_Hand, STOMACH, SKEL_R_Thigh, SKEL_R_Calf, SKEL_R_Foot, SKEL_L_Thigh, SKEL_L_Calf, SKEL_L_Foot
};
Vector3 NPC_Bones[257][15];

void BoneEsp(int c, Color BoneColor) {
	//Head to Neck
	DrawBone(NPC_Bones[c][pSKEL_Head], NPC_Bones[c][pSKEL_Neck_1], BoneColor);
	//Left Arm with CALF
	DrawBone(NPC_Bones[c][pSKEL_Neck_1], NPC_Bones[c][pSKEL_L_UpperArm], BoneColor);
	DrawBone(NPC_Bones[c][pSKEL_L_UpperArm], NPC_Bones[c][pMH_L_Elbow], BoneColor);
	DrawBone(NPC_Bones[c][pMH_L_Elbow], NPC_Bones[c][pSKEL_L_Hand], BoneColor);
	//Right Arm with CALF
	DrawBone(NPC_Bones[c][pSKEL_Neck_1], NPC_Bones[c][pSKEL_R_UpperArm], BoneColor);
	DrawBone(NPC_Bones[c][pSKEL_R_UpperArm], NPC_Bones[c][pMH_R_Elbow], BoneColor);
	DrawBone(NPC_Bones[c][pMH_R_Elbow], NPC_Bones[c][pSKEL_R_Hand], BoneColor);
	//Neck to STOMACH
	DrawBone(NPC_Bones[c][pSKEL_Neck_1], NPC_Bones[c][pSTOMACH], BoneColor);
	//Right Leg
	DrawBone(NPC_Bones[c][pSTOMACH], NPC_Bones[c][pSKEL_R_Thigh], BoneColor);
	DrawBone(NPC_Bones[c][pSKEL_R_Thigh], NPC_Bones[c][pSKEL_R_Calf], BoneColor);
	DrawBone(NPC_Bones[c][pSKEL_R_Calf], NPC_Bones[c][pSKEL_R_Foot], BoneColor);
	//Left Leg
	DrawBone(NPC_Bones[c][pSTOMACH], NPC_Bones[c][pSKEL_L_Thigh], BoneColor);
	DrawBone(NPC_Bones[c][pSKEL_L_Thigh], NPC_Bones[c][pSKEL_L_Calf], BoneColor);
	DrawBone(NPC_Bones[c][pSKEL_L_Calf], NPC_Bones[c][pSKEL_L_Foot], BoneColor);
}

void Barrel(Vector3 HeadPos, Vector3 Rotation, Color Color, const float trailLen = 10.0f) {
	float rot = acosf(Rotation.x) * 180.0f / DirectX::XM_PI;
	if (asinf(Rotation.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;

	auto ForwardDirection = XMConvertToRadians(rot);
	Vector3 forwardVec(cosf(ForwardDirection), sinf(ForwardDirection), 0.0f);
	Vector3 screenOrigin, screenTrail, trailEnd;

	Vector3 trail(forwardVec.x * trailLen, forwardVec.y * trailLen, forwardVec.z * trailLen);
	trailEnd = trail + HeadPos;

	if (W2S(HeadPos, screenOrigin) && W2S(trailEnd, screenTrail)) {
		renderer->DrawLine(screenOrigin.x, screenOrigin.y, screenTrail.x, screenTrail.y, Color);
	}
}
DirectX::SimpleMath::Color TransP = DirectX::SimpleMath::Color(0.0f, 0.0f, 0.0f, 0.0f);

void DrawBoundingBox(Vector3 Top2d,/* Vector3 Bottom2d,*/ float HightBox, Color Color, float Size) {
	float H = HightBox;
	float W = H / 4.0f;

	//renderer->FillRect(Top2d.x - W - Size, Top2d.y - Size, W * 2.0f + (Size * 2.0f), H + (Size * 2.0f), Color); //First rect
	//renderer->FillRect(Top2d.x - W, Top2d.y, W * 2.0f, H, TransP); //First rect
	//renderer->FillRect(Top2d.x - W, Top2d.y - H, Top2d.x + W * 2.0f, Top2d.y + H, Colors::Transparent.v); //First rect

	renderer->FillRect(Top2d.x - W, Top2d.y, W * 2.0f, Size, Color); // TOP
	renderer->FillRect(Top2d.x - W, Top2d.y + H, W * 2.0f + Size, Size, Color); //BOTTOM
	renderer->FillRect(Top2d.x - W, Top2d.y, Size, H, Color); //Left side
	renderer->FillRect(Top2d.x + W, Top2d.y, Size, H, Color); //Right side
}

Vector3 CamRotLocal(0.0f, 0.0f, 0.0f);
PVector3 CamRotLocal2(0.0f, 0.0f, 0.0f);
PVector3 pLocalCamPos(0.0f, 0.0f, 0.0f);

Vector3 CurrentAngels1(0.0f, 0.0f, 0.0f);
Vector3 CurrentAngels2(0.0f, 0.0f, 0.0f);
Vector3 CurrentAngels3(0.0f, 0.0f, 0.0f);
Vector3 CurrentAngels4(0.0f, 0.0f, 0.0f);
Vector3 CurrentAngels5(0.0f, 0.0f, 0.0f);

BOOL GetlButton = 0;

typedef BOOL(__fastcall* fpNetworkRequestControlOfEntity)(DWORD entity);
fpNetworkRequestControlOfEntity pNetworkRequestControlOfEntity = nullptr;

DWORD LocalPlayerHandle = 0x0;

void SetAngi(CObject* Entity, Vector3 Target, float Distance, float CrossDistance, bool Qrite) {
	if (Tools->IsValid(addy.pGameCamManager)
		&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles)
		&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles)
		&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer1)
		&& Tools->IsValid(&addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer2)
		&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles)) {
		Vector3 Speed(0.0f, 0.0f, 0.0f);
		Vector3 MyPos = ToVector3(pLocalCamPos);
		Vector3 LocalSpeed(0.0f, 0.0f, 0.0f);

		if (Tools->IsValid(Entity)) {
			if (addy.localPlayer->IsInVehicle() && Tools->IsValid(addy.localPlayer->gVehicleManager()))
				LocalSpeed = addy.localPlayer->gVehicleManager()->V3VehSpeed();
			else
				LocalSpeed = addy.localPlayer->v3Velocity;

			if (Entity->IsInVehicle() && Tools->IsValid(Entity->gVehicleManager()))
				Speed = Entity->gVehicleManager()->V3VehSpeed();
			else
				Speed = Entity->v3Velocity;

		}

		float realpredi = Hacks->Aimbot_Prediction;

		if (addy.localPlayer->IsInVehicle())
			realpredi *= 2.0f;

		LocalSpeed *= realpredi;
		Speed *= realpredi;

		Target += Speed;
		MyPos += LocalSpeed;

		Vector3 ScreenPos2D(0.0f, 0.0f, 0.0f);
		W2S(Vector3(Target.x, Target.y, Target.z), ScreenPos2D);
		float OutZ = ((ScreenPos2D.y / ScreenHight) * 2.0f) - 1.0f;



		Vector3 delta(Target.x - MyPos.x, Target.y - MyPos.y, Target.z - MyPos.z);
		delta.Normalize(delta);
		Vector3 Out(delta.x / delta.Length(), delta.y / delta.Length(), delta.z / delta.Length());

		if (addy.localPlayer->IsInVehicle())Out.z = (-OutZ + CurrentAngels1.z);
		Vector3 diff1 = Out - CurrentAngels1;
		if (addy.localPlayer->IsInVehicle())Out.z = (-OutZ + CurrentAngels2.z);
		Vector3 diff2 = Out - CurrentAngels2;
		if (addy.localPlayer->IsInVehicle())Out.z = (-OutZ + CurrentAngels3.z);
		Vector3 diff3 = Out - CurrentAngels3;
		if (addy.localPlayer->IsInVehicle())Out.z = (-OutZ + CurrentAngels4.z);
		Vector3 diff4 = Out - CurrentAngels4;
		if (addy.localPlayer->IsInVehicle())Out.z = (-OutZ + CurrentAngels5.z);
		Vector3 diff5 = Out - CurrentAngels5;


		float RealAimSmooth = (Hacks->Aimbot_Smooth <= 2.0f ? 2.0f : (Hacks->Aimbot_Smooth * 16.0f));
		float RealAimSmooth2 = (Hacks->Aimbot_Smooth <= 2.0f ? 2.5f : Hacks->Aimbot_Smooth);

		auto Smooth1 = (diff1 / diff1.Length()) / RealAimSmooth;
		auto Smooth2 = (diff2 / diff2.Length()) / RealAimSmooth;
		auto Smooth3 = (diff3 / diff3.Length()) / RealAimSmooth;
		auto Smooth4 = (diff4 / diff4.Length()) / RealAimSmooth;
		auto Smooth5 = (diff5 / diff5.Length()) / RealAimSmooth;

		if (Hacks->Aimbot_Linear) {
			Smooth1 = (diff1 / diff1.Length()) / RealAimSmooth;
			Smooth2 = (diff2 / diff2.Length()) / RealAimSmooth;
			Smooth3 = (diff3 / diff3.Length()) / RealAimSmooth;
			Smooth4 = (diff4 / diff4.Length()) / RealAimSmooth;
			Smooth5 = (diff5 / diff5.Length()) / RealAimSmooth;
		}
		else {
			Smooth1 = diff1 / RealAimSmooth2;
			Smooth2 = diff2 / RealAimSmooth2;
			Smooth3 = diff3 / RealAimSmooth2;
			Smooth4 = diff4 / RealAimSmooth2;
			Smooth5 = diff5 / RealAimSmooth2;
		}

		if (Hacks->Aimbot_Smooth < 2.0f) {
			if (addy.localPlayer->IsInVehicle()) {
				Smooth1 = diff1 / 2.0f;
				Smooth2 = diff2 / 2.0f;
				Smooth3 = diff3 / 2.0f;
				Smooth4 = diff4 / 2.0f;
				Smooth5 = diff5 / 2.0f;
			}
			else {
				Smooth1 = diff1;
				Smooth2 = diff2;
				Smooth3 = diff3;
				Smooth4 = diff4;
				Smooth5 = diff5;
			}
		}

		if (Smooth1.Length() > diff1.Length())Smooth1 = diff1;
		if (Smooth2.Length() > diff2.Length())Smooth2 = diff2;
		if (Smooth3.Length() > diff3.Length())Smooth3 = diff3;
		if (Smooth4.Length() > diff4.Length())Smooth4 = diff4;
		if (Smooth5.Length() > diff5.Length())Smooth5 = diff5;

		Vector3 Out1 = (CurrentAngels1 + Smooth1);
		Vector3 Out2 = (CurrentAngels2 + Smooth2);
		Vector3 Out3 = (CurrentAngels3 + Smooth3);
		Vector3 Out4 = (CurrentAngels4 + Smooth4);
		Vector3 Out5 = (CurrentAngels5 + Smooth5);

		Out1.Clamp(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
		Out2.Clamp(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
		Out3.Clamp(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
		Out4.Clamp(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
		Out5.Clamp(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));

		if (Qrite) {
			if (!addy.localPlayer->IsInsideFirstPerson()) {
				addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles->CamRotFPS = Out1;
				addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer1->CamRot = Out2;
				addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer2->CamRot = Out3;
				addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer1->CamRotFPS = Out4;
				addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer2->CamRotFPS = Out5;
			}
			else {
				addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles->CamRotFPS = Out1;
			}
		}
	}
}

static DWORD OFF_FixVehAimVec = 0x2AC;

void AimVec(Vector3 point, CObject* TmpObj = NULL, bool qWriteAimVec = false)
{
	__try
	{
		if (Tools->IsValid(addy.pGameCamManager)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles)
			&& Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles)) {
			DWORD64 addr = reinterpret_cast<DWORD64>(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles);
			//Fix Aim Vectors
			__try
			{
				DWORD64 m_CCameraViewAngles = addr;
				DWORD64 camParams = *(DWORD64*)(addr + 0x10);
				if (camParams != NULL) {
					if (addy.localPlayer->IsInVehicle())
					{
						if (*(float*)(camParams + OFF_FixVehAimVec) == -2.0f)
						{
							*(float*)(camParams + OFF_FixVehAimVec) = 0.0f;
							*(float*)(camParams + 0x2C0) = 111.0f;
							*(float*)(camParams + 0x2C4) = 111.0f;
						}
					}
					else {
						if (*(float*)(camParams + 0x130) == 8.0f)
						{
							*(float*)(camParams + 0x130) = 111.0f; //def 8.0f
							*(float*)(camParams + 0x134) = 111.0f; //def 10.0f
							*(float*)(camParams + 0x4CC) = 0.0f;   //def 4.0f

							if (*(float*)(camParams + 0x49C) == 1.0f)
								*(float*)(camParams + 0x49C) = 0.0f;   //def 1.0f

							*(float*)(camParams + 0x2AC) = 0.0f;   //def -3.0f
							*(float*)(camParams + 0x2B0) = 0.0f;   //def -8.0f
						}
					}
				}
			}
			__except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {}

			Vector3 CrosshairPos = *(Vector3*)(addr + 0x60);

			Vector3 Speed(0.0f, 0.0f, 0.0f);
			Vector3 LocalSpeed(0.0f, 0.0f, 0.0f);

			if (Tools->IsValid(TmpObj)) {
				if (addy.localPlayer->IsInVehicle() && Tools->IsValid(addy.localPlayer->gVehicleManager()))
					LocalSpeed = addy.localPlayer->gVehicleManager()->V3VehSpeed();
				else
					LocalSpeed = addy.localPlayer->v3Velocity;

				if (TmpObj->IsInVehicle() && Tools->IsValid(TmpObj->gVehicleManager()))
					Speed = TmpObj->gVehicleManager()->V3VehSpeed();
				else
					Speed = TmpObj->v3Velocity;

			}

			float realpredi = Hacks->Aimbot_Prediction;

			LocalSpeed *= realpredi;
			Speed *= realpredi;

			point += Speed;
			CrosshairPos += LocalSpeed;

			float distance = Vector3().Distance(CrosshairPos, point);

			static DWORD64 smoothTime;
			if (GetTickCount64() - smoothTime > 2)
			{
				smoothTime = GetTickCount64();

				float threshold = 0.5f / (Hacks->Aimbot_Smooth * 20.0f);
				Vector3 Out = Vector3((point.x - CrosshairPos.x) / distance, (point.y - CrosshairPos.y) / distance, (point.z - CrosshairPos.z) / distance);

				Out.Clamp(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));

				if (!qWriteAimVec)return;

				if (Hacks->Aimbot_Smooth <= 1)
				{
					if (Hacks->Aimbot_HorizontalOnly) {
						*(Vector2*)(addr + 0x40) = Vector2(Out.x, Out.y);  //FPS
						*(Vector2*)(addr + 0x3D0) = Vector2(Out.x, Out.y);  //TPS
					}
					else {
						*(Vector3*)(addr + 0x40) = Out;  //FPS
						*(Vector3*)(addr + 0x3D0) = Out; //TPS
					}
				}
				else {
					//FPS
					{
						Vector3 angles = *(Vector3*)(addr + 0x40);
						if (((Out.x - angles.x) > threshold) || ((angles.x - Out.x) > threshold))
						{
							if (angles.x > Out.x)
								*(float*)(addr + 0x40 + 0x0) -= threshold;
							else if (angles.x < Out.x)
								*(float*)(addr + 0x40 + 0x0) += threshold;
						}
						if (((Out.y - angles.y) > threshold) || ((angles.y - Out.y) > threshold))
						{
							if (angles.y > Out.y)
								*(float*)(addr + 0x40 + 0x4) -= threshold;
							else if (angles.y < Out.y)
								*(float*)(addr + 0x40 + 0x4) += threshold;
						}
						if (!Hacks->Aimbot_HorizontalOnly)
							if (((Out.z - angles.z) > threshold) || ((angles.z - Out.z) > threshold))
							{
								if (angles.z > Out.z)
									*(float*)(addr + 0x40 + 0x8) -= threshold;
								else if (angles.z < Out.z)
									*(float*)(addr + 0x40 + 0x8) += threshold;
							}
					}

					//TPS
					{
						Vector3 angles = *(Vector3*)(addr + 0x3D0);
						if (((Out.x - angles.x) > threshold) || ((angles.x - Out.x) > threshold))
						{
							if (angles.x > Out.x)
								*(float*)(addr + 0x3D0 + 0x0) -= threshold;
							else if (angles.x < Out.x)
								*(float*)(addr + 0x3D0 + 0x0) += threshold;
						}
						if (((Out.y - angles.y) > threshold) || ((angles.y - Out.y) > threshold))
						{
							if (angles.y > Out.y)
								*(float*)(addr + 0x3D0 + 0x4) -= threshold;
							else if (angles.y < Out.y)
								*(float*)(addr + 0x3D0 + 0x4) += threshold;
						}
						if (!Hacks->Aimbot_HorizontalOnly)
							if (((Out.z - angles.z) > threshold) || ((angles.z - Out.z) > threshold))
							{
								if (angles.z > Out.z)
									*(float*)(addr + 0x3D0 + 0x8) -= threshold;
								else if (angles.z < Out.z)
									*(float*)(addr + 0x3D0 + 0x8) += threshold;
							}
					}
				}
			}
		}
	}
	__except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {}
}

Vector2 WorldToRadar(Vector3 Location, FLOAT RadarX, FLOAT RadarY, float RadarSize, float RadarZoom) {
	Vector2 Return(0.0f, 0.0f);
	if (Tools->IsValid(addy.pGameCamManager) && Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles) && Tools->IsValid(addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles)) {
		CPlayerAngles* pPlayerAnglesFPS = (addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->pMyFPSAngles);
		CPlayerAngles* pPlayerAngles = (addy.pGameCamManager->pMyCameraManagerAngles->pMyCameraAngles->VehiclePointer1);
		if (Tools->IsValid(pPlayerAnglesFPS) && Tools->IsValid(pPlayerAngles)) {
			float rot = 0.0f;
			if (addy.localPlayer->IsInsideFirstPerson()) {
				rot = acosf(pPlayerAnglesFPS->CamRotFPS.x) * 180.0f / DirectX::XM_PI;
				if (asinf(pPlayerAnglesFPS->CamRotFPS.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;
			}
			else {
				rot = acosf(pPlayerAngles->CamRot.x) * 180.0f / DirectX::XM_PI;
				if (asinf(pPlayerAngles->CamRot.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;
			}

			float ForwardDirection = XMConvertToRadians(rot);
			Vector2 forwardVec = Vector2(cosf(ForwardDirection), sinf(ForwardDirection));

			float CosYaw = forwardVec.x;
			float SinYaw = forwardVec.y;

			float DeltaX = Location.x - addy.localPlayer->fPosition.x;
			float DeltaY = Location.y - addy.localPlayer->fPosition.y;

			float LocationX = (DeltaY * CosYaw - DeltaX * SinYaw) / RadarZoom;
			float LocationY = (DeltaX * CosYaw + DeltaY * SinYaw) / RadarZoom;

			if (LocationX > RadarSize / 2.0f - 2.5f)
				LocationX = RadarSize / 2.0f - 2.5f;
			else if (LocationX < -(RadarSize / 2.0f - 2.5f))
				LocationX = -(RadarSize / 2.0f - 2.5f);

			if (LocationY > RadarSize / 2.0f - 2.5f)
				LocationY = RadarSize / 2.0f - 2.5f;
			else if (LocationY < -(RadarSize / 2.0f - 2.5f))
				LocationY = -(RadarSize / 2.0f - 2.5f);

			Return = Vector2(-LocationX + RadarX, -LocationY + RadarY);
		}
	}
	return Return;
}
typedef DWORD64(__fastcall* SetVehicleForwardSpeed_t)(CVehicleManager* vehicle, float Speed);
static SetVehicleForwardSpeed_t SetVehicleForwardSpeed;

typedef DWORD64(__fastcall* SetEntityVisible_t)(CObject* entity, uint32_t a2, bool toggle, bool unk1);
static SetEntityVisible_t SetEntityVisible;

DWORD64 LocalPlayerWrapper_address = 0;
typedef DWORD64(__fastcall* tSTART_SHAPE_TEST_RAY)(PVector3 From, PVector3 To, unsigned int flags, unsigned int entity, DWORD64* a8, DWORD64 a9);
tSTART_SHAPE_TEST_RAY _START_SHAPE_TEST_RAY = nullptr;

typedef DWORD64(__fastcall* t_GET_RAYCAST_RESULT)(DWORD64 Handle, DWORD* hit, PVector3* VecEnd, PVector3* VecSurface, DWORD* entity);
t_GET_RAYCAST_RESULT _GET_RAYCAST_RESULT = nullptr;

typedef DWORD64(__fastcall* fpGiveDelayedWeaponToPed)(DWORD ped, DWORD hash, int ammo, bool equipNow);
static fpGiveDelayedWeaponToPed GiveDelayedWeaponToPed = nullptr;

DWORD weaponsList[] = {
	0x93E220BD, 0xA0973D5E, 0x24B17070, 0x2C3731D9, 0xAB564B93, 0x787F0BB, 0xBA45E8B8, 0x23C9F95C, 0xFDBC8A50, 0x497FACC3, //Throwables
	0x34A67B97, 0xFBAB5776, 0x060EC506, //Miscellaneous
	0xB1CA77B1, 0xA284510B, 0x42BF8A85, 0x7F7497E5, 0x6D544C99, 0x63AB0442, 0x0781FE4A, 0xB62D1F67, //Heavy Weapons
	0x05FC3C11, 0x0C472FE2, 0xA914799, 0xC734385A, 0x6A6C02E0, //Sniper Rifles
	0x9D07F764, 0x7FD62962, 0xDBBD7280, 0x61012683, //Light Machine Guns
	0xBFEFFF6D, 0x394F415C, 0x83BF0278, 0xFAD1F1C9, 0xAF113F99, 0xC0A3098D, 0x969C3D67, 0x7F229F94, 0x84D6FAFD, 0x624FE830, //Assault Rifles
	0x13532244, 0x2BE6766B, 0x78A97CD0, 0xEFE7E2DF, 0x0A3D4D34, 0xDB1AA450, 0xBD248B55, 0x476BF155, //Submachine Guns
	0x1B06D571, 0xBFE256D4, 0x5EF9FEC4, 0x22D8FE39, 0x3656C8C1, 0x99AEEB3B, 0xBFD21232, 0x88374054, 0xD205520E, 0x83839C4, 0x47757124, 0xDC4DB296, 0xC1B3C3D1, 0xCB96392F, 0x97EA20B8, 0xAF3696A1, //Pistols
	0x92A27487, 0x958A4A8F, 0xF9E6AA4B, 0x84BD7BFD, 0x8BB05FD7, 0x440E4788, 0x4E875F73, 0xF9DCBF2D, 0xD8DF3C3C, 0x99B507EA, 0xDD5DF8D9, 0xDFE37640, 0x678B81B1, 0x19044EE0, 0xCD274149, 0x94117305, 0x3813FC08, //Mele
	0x1D073A89, 0x555AF99A, 0x7846A318, 0xE284C527, 0x9D61E50F, 0xA89CB99E, 0x3AABBBAA, 0xEF951FBB, 0x12E82D3D
};

typedef DWORD(__fastcall* centity_to_handle_fn)(CObject* Obj);
static centity_to_handle_fn centity_to_handle = nullptr;

typedef DWORD(__fastcall* centity_to_handle_fn_veh)(CVehicleManager* Obj);
static centity_to_handle_fn_veh centity_to_handle_veh = nullptr;


void GiveAllWeapons() {
	LocalPlayerHandle = centity_to_handle(addy.localPlayer);
	for (int i = 0; i < std::size(weaponsList); i++) {
		int ammo = 999;
		GiveDelayedWeaponToPed((DWORD)LocalPlayerHandle, (DWORD)weaponsList[i], (int)ammo, (bool)true);
	}
}

typedef BOOL(__fastcall* fpRemoveWeaponFromPed)(DWORD ped, DWORD hash);
fpRemoveWeaponFromPed RemoveWeaponFromPed = nullptr;
bool Remove_InHand = false;
bool Remove_All = false;

int Invisible_Near = false;
bool Visible_Near = false;

void RemoveAllWeapons() {
	LocalPlayerHandle = centity_to_handle(addy.localPlayer);
	for (int i = 0; i < std::size(weaponsList); i++) {
		//spoof_call(jmp_rbx_0, RemoveWeaponFromPed, (DWORD)LocalPlayerHandle, (DWORD)weaponsList[i]);
		RemoveWeaponFromPed((DWORD)LocalPlayerHandle, (DWORD)weaponsList[i]);
	}
}

void GiveSelectedWeapons() {
	LocalPlayerHandle = centity_to_handle(addy.localPlayer);

	GiveDelayedWeaponToPed((DWORD)LocalPlayerHandle, (DWORD)Hacks->WeaponSpawn1, (int)999, (bool)false);
	GiveDelayedWeaponToPed((DWORD)LocalPlayerHandle, (DWORD)Hacks->WeaponSpawn2, (int)999, (bool)false);
	GiveDelayedWeaponToPed((DWORD)LocalPlayerHandle, (DWORD)Hacks->WeaponSpawn3, (int)999, (bool)false);
	GiveDelayedWeaponToPed((DWORD)LocalPlayerHandle, (DWORD)Hacks->WeaponSpawn4, (int)999, (bool)false);

}
typedef DWORD64(__fastcall* SetPedAmmo_t) (DWORD ped, DWORD WeaponHash, int ammo);
SetPedAmmo_t oSetPedAmmo = nullptr;

void vSetPedAmmo(int Amount) {
	LocalPlayerHandle = centity_to_handle(addy.localPlayer);
	for (int i = 0; i < std::size(weaponsList); i++)
		oSetPedAmmo(LocalPlayerHandle, weaponsList[i], Amount);
}

DWORD64 FromM128_DWORD(__m64 in) { return DWORD64(in.m64_u64); }
bool FromM128_BOOL(__m64 in) { return bool(in.m64_i8[0]); }

DWORD64 FrameCount = 0x0;
//extern void ObjectEsp(bool DisplayHash);
std::uint64_t g_HookFrameCount = 0x0;
bool Refresh = false;

enum eGameState {
	GameStatePlaying,
	GameStateIntro,
	GameStateLicenseShit = 3,
	GameStateMainMenu = 5,
	GameStateLoadingSP_MP = 6
}; eGameState GameState;

DWORD64 Veh_Speed = 0x0;
DWORD64 pGameCamManager_pattern = 0x0;
DWORD64 ReplayInterface_pattern = 0x0;

typedef DWORD64(__fastcall* RequestModel_t)(DWORD a1);
RequestModel_t RequestModel = nullptr;

typedef DWORD64(__fastcall* fpShootSingleBulletBetweenCoords)(PVector3 pos1, PVector3 pos2, int damage, BOOL unk0, DWORD hash, DWORD owner, BOOL isAudible, BOOL isVisible, float speed, DWORD IgnoreEntity, BOOL unk1, BOOL unk2, BOOL unk3, BOOL unk4);
fpShootSingleBulletBetweenCoords shoot_bullets = NULL;

class Func {
public:
	fpShootSingleBulletBetweenCoords shoot_bullets = NULL;
};
static Func Funktion;

typedef PVector3* (__fastcall* pGetCamRot)(PVector3* out, uint32_t unk1);
pGetCamRot GetCamRot = nullptr;

typedef PVector3* (__fastcall* pGetGameplayCamCoord)(PVector3* out);
pGetGameplayCamCoord GetGameplayCamCoord = nullptr;

typedef DWORD64(__fastcall* CLEAR_PED_TASKS_t)(DWORD HandleOfPed);
CLEAR_PED_TASKS_t CLEAR_PED_TASKS = nullptr;

typedef DWORD64(__fastcall* fpClearPedTasksImmediately)(DWORD HandleOfPed);
fpClearPedTasksImmediately ClearPedTasksImmediately = nullptr;

typedef BOOL(__fastcall* fpNetworkSetInSpectatorMode)(bool toggle, DWORD HandleOfPed);
fpNetworkSetInSpectatorMode NetworkSetInSpectatorMode = nullptr;

typedef DWORD64(__fastcall* pGetWayPoint)(Vector3* WaypointOut);
pGetWayPoint GetWayPoint = nullptr;

void VehSpeed(float fSpeed) {
	if (Tools->IsValid(addy.localPlayer->gVehicleManager())) {
		//auto VehHandle = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
		SetVehicleForwardSpeed(addy.localPlayer->gVehicleManager(), fSpeed);
	}
}
static bool Key_VK_ADD = false;
static bool Key_VK_SUBTRACT = false;
static bool Key_VK_MULTIPLY = false;

void VehSpeedHack() {
	float speed = *(float*)(Veh_Speed);
	if (Key_VK_ADD && Key_VK_SUBTRACT) {
		VehSpeed(speed);
	}
	else {
		if (Key_VK_ADD) {
			speed += (Hacks->Veh_Impulse * 0.25f);
			if (speed < 300.0f)VehSpeed(speed);
		}
		if (Key_VK_SUBTRACT) {
			if (speed > 0.0f)speed -= (Hacks->Veh_Impulse * 0.25f);
			VehSpeed(speed);
		}
	}
	if (Key_VK_MULTIPLY) {
		VehSpeed(0.0f);
	}
}

typedef DWORD64(__fastcall* fpSetVehicleOnGroundProperly)(DWORD vehicle, DWORD unk);
fpSetVehicleOnGroundProperly pSetVehicleOnGroundProperly = nullptr;

Vector2 WayPointPos(0.0f, 0.0f);
void TeleportToWaypoint() {
	if (addy.localPlayer->IsInVehicle() && Tools->IsValid(addy.localPlayer->gVehicleManager())) {
		DWORD v = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
		//if (!pNetworkRequestControlOfEntity(v))
		//	return;
		VehSpeed(0.0f);
		pSetVehicleOnGroundProperly(v, 0);
		addy.localPlayer->gVehicleManager()->SetPosClass(Vector3(WayPointPos.x, WayPointPos.y, -200.5f));
		pSetVehicleOnGroundProperly(v, 0);
		VehSpeed(0.0f);
	}
	else {
		//if (!pNetworkRequestControlOfEntity(LocalPlayerHandle))
		//	return;
		LocalPlayerHandle = centity_to_handle(addy.localPlayer);
		CLEAR_PED_TASKS(LocalPlayerHandle);
		oCLEAR_PED_TASKS_IMMEDIATELY(LocalPlayerHandle);
		addy.localPlayer->SetPosClass(Vector3(WayPointPos.x, WayPointPos.y, -200.5f));
		CLEAR_PED_TASKS(LocalPlayerHandle);
		oCLEAR_PED_TASKS_IMMEDIATELY(LocalPlayerHandle);
	}
}
#include <thread>
void TeleportToPos(Vector3 Pos) {
	LocalPlayerHandle = centity_to_handle(addy.localPlayer);
	if (Pos.z != -50 && Pos.z > -175) {
		if (Tools->IsValid(addy.localPlayer) && Tools->IsValid(addy.localPlayer->_ObjectNavigation)) {
			if (addy.localPlayer->IsInVehicle() && Tools->IsValid(addy.localPlayer->gVehicleManager())) {
				DWORD v = centity_to_handle(reinterpret_cast<CObject*>(addy.localPlayer->gVehicleManager()));
				//if (!pNetworkRequestControlOfEntity(v))
				//	return;
				VehSpeed(0.0f);
				pSetVehicleOnGroundProperly(v, 0);
				addy.localPlayer->gVehicleManager()->SetPosClass(Vector3(Pos.x, Pos.y, Pos.z));
				pSetVehicleOnGroundProperly(v, 0);
				VehSpeed(0.0f);

			}
			else {
				//pNetworkRequestControlOfEntity(LocalPlayerHandle);
				CLEAR_PED_TASKS(LocalPlayerHandle);
				oCLEAR_PED_TASKS_IMMEDIATELY(LocalPlayerHandle);
				addy.localPlayer->SetPosClass(Vector3(Pos.x, Pos.y, Pos.z));
				CLEAR_PED_TASKS(LocalPlayerHandle);
				oCLEAR_PED_TASKS_IMMEDIATELY(LocalPlayerHandle);
			}
		}
	}
}

typedef DWORD64(__fastcall* fpSetVehicleFixed)(DWORD vehicle);
fpSetVehicleFixed pSetVehicleFixed = nullptr;

typedef DWORD64(__fastcall* fpSetVehicleFixed2)(CVehicleManager* vehicle, float Health);
fpSetVehicleFixed2 pSetVehicleFixed2 = nullptr;

typedef DWORD64(__fastcall* fpSetVehicleModKit)(DWORD vehicle, int modKit);
fpSetVehicleModKit pSetVehicleModKit = nullptr;

typedef DWORD64(__fastcall* fpSetVehiclePlateText)(DWORD vehicle, const char* Text);
fpSetVehiclePlateText pSetVehiclePlateText = nullptr;

typedef DWORD64(__fastcall* fpToggleVehicleMod)(DWORD vehicle, int modType, bool toggle);
fpToggleVehicleMod pToggleVehicleMod = nullptr;

typedef DWORD64(__fastcall* fpSetVehicleMod)(DWORD vehicle, int modType, int modIndex, bool customTires);
fpSetVehicleMod pSetVehicleMod = nullptr;

typedef DWORD64(__fastcall* fpSetVehicleWheelType)(DWORD vehicle, int wheelType);
fpSetVehicleWheelType pSetVehicleWheelType = nullptr;

typedef int(__fastcall* fpGetNumVehicleMod)(DWORD vehicle, int modType);
fpGetNumVehicleMod pGetNumVehicleMod = nullptr;

typedef bool(__fastcall* fpDecorSetInt)(DWORD entity, const char* name, int value);
fpDecorSetInt pDecorSetInt = nullptr;

typedef DWORD64(__fastcall* fpSetPedIntoVehicle)(DWORD ped, DWORD vehicle, int seat);
fpSetPedIntoVehicle pSetPedIntoVehicle = nullptr;

typedef BOOL(__fastcall* fpGetEntityRotation)(PVector3* out, DWORD entity, int rotationOrder);
fpGetEntityRotation pGetEntityRotation = nullptr;

typedef void(__fastcall* SetPedConfigFlag_t)(CObject* ped, int Flag, BOOLEAN toggle);
SetPedConfigFlag_t SetPedConfigFlag = nullptr;

typedef DWORD64(__fastcall* fpSetVehicleEngineOn)(DWORD vehicle, bool unk0, bool instant, bool unk1);
fpSetVehicleEngineOn pSetVehicleEngineOn = nullptr;

typedef DWORD64(__fastcall* tSET_PED_INTO_VEH)(CObject* Player, CVehicleManager* veh, unsigned int Seat, unsigned int unk1);
tSET_PED_INTO_VEH SET_PED_INTO_VEH = NULL;

#define mpply_offensive_language 0x3CDB43E2
#define mpply_griefing 0x9C6A0C42
#define mpply_helpful 0x893E1390
#define mpply_offensive_ugc 0xF3DE4879
#define mpply_offensive_tagplate 0xE8FB6DD5
#define mpply_exploits 0x9F79BA0B
#define mpply_game_exploits 0xCBFD04A4
#define mpply_vc_hate 0xE7072CD
#define mpply_vc_annoyingme 0x62EB8C5A

Vector3 BoneCalExternal(__m128i TmpBone, CBoneManager* BoneManager) {
	__m128* v12 = reinterpret_cast<__m128*>(BoneManager);
	__m128 v13 = _mm_mul_ps(_mm_castsi128_ps(_mm_shuffle_epi32(TmpBone, 0)), *v12);
	__m128 v14 = _mm_mul_ps(_mm_castsi128_ps(_mm_shuffle_epi32(TmpBone, 0x55)), v12[1]);
	__m128 v15 = _mm_mul_ps(_mm_castsi128_ps(_mm_shuffle_epi32(TmpBone, 0xAA)), v12[2]);
	__m128 v16 = _mm_add_ps(_mm_add_ps(v13, v12[3]), v14);

	__m128 v17 = _mm_add_ps(v16, v15);
	return Vector3(v17.m128_f32[0], v17.m128_f32[1], v17.m128_f32[2]);
}

int SigFountCount = 0;
HWND GtaHWND = NULL;
DWORD64 LocalPlayerWrapper_pattern = 0x0;
DWORD64 dw_ViewportPattern = 0x0;

typedef DWORD(__fastcall* fpGetPedInVehicleSeat)(DWORD vehicle, int index);
fpGetPedInVehicleSeat pGetPedInVehicleSeat = nullptr;

typedef DWORD(__fastcall* tGET_HASH_KEY)(char* value);
tGET_HASH_KEY GET_HASH_KEY = nullptr;

DWORD $(char* str) {
	return GET_HASH_KEY(&str[0u]);
}

DWORD64 WaterPattern = 0x0;
CWaterDataItem* WaterDatItem;

typedef BOOL(__fastcall* HasModelLoaded_t) (DWORD modelHash);
static HasModelLoaded_t HasModelLoaded = 0x0;

//typedef void(__fastcall* CreateVehicle_t)(DWORD hash, PVector3 Pos, float heading, bool isNetwork, bool thisScriptCheck, bool a8);// typedef void(__fastcall* fSpawnVeh)(DWORD hash, PVector3 Pos, float heading, bool isNetwork, bool thisScriptCheck, bool a8);
typedef DWORD64(__fastcall* CreateVehicle_t) (DWORD modelHash, PVector3 pos, float heading, BOOL networkHandle, BOOL vehiclehandle);
static CreateVehicle_t CreateVehicle = 0x0;

static DWORD FirstdwShoot = 0x0;
static DWORD SecdwShoot = 0x0;
static DWORD ThirddwShoot = 0x0;

#include "scan.h"

typedef BOOL(__fastcall* fpSetEntityVelocity)(DWORD entity, PVector3* velocity);
fpSetEntityVelocity pSetEntityVelocity = NULL;

typedef BOOL(__fastcall* fpSetEntityCollision)(DWORD entity, bool toggle, bool physics, bool unk0);
fpSetEntityCollision pSetEntityCollision = NULL;

#include <iostream>
#include <fstream>
using namespace std;

#include "PlayerArrayNames.h"
#include "IDA.h"

typedef DWORD64(__fastcall* fpGetName)(DWORD64 thisptr, WORD a2);
typedef PVOID64(__fastcall* GetNameVerify_t)();
fpGetName pGetName = NULL;
GetNameVerify_t GetNameVerify;
DWORD64 pGetNameVerify = NULL;
DWORD64 addrRagePool = 0x0;

std::string GetNameFromIndex(WORD i) {
	if (i < std::size(player_array_names)) {
		WORD id = ~__ROR2__(i + ((0x0 - 0x198) & 0x3FF), 8);
		return *reinterpret_cast<std::string*>(pGetName(pGetNameVerify, player_array_names[id]));
	}
	return "None";
}

typedef DWORD64(__fastcall* fpSetEntityVel)(CObject* a1, PVector3 a2);
fpSetEntityVel pSetEntityVel = NULL;

DWORD64 PatternReplayInterfaceRageMP = 0x0;
BOOL InitGameFunctions() {
	uintptr_t  scan_start = addy.BASE;
	size_t     scan_size = Tools->GetModuleInfoX(0).SizeOfImage;
	uintptr_t  scan_start_rage = (uintptr_t)Tools->GetModuleInfoX(4).lpBaseOfDll;
	size_t     scan_size_rage = Tools->GetModuleInfoX(4).SizeOfImage;

	if (scan_start_rage) {
		pGetName = reinterpret_cast<fpGetName>(ps_find_idastyle("0F B7 C2 48 C1 E0 05", scan_start_rage, scan_size_rage));
		GetNameVerify = (GetNameVerify_t)ps_find_idastyle("48 83 EC 38 48 C7 44 24 ?? ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 85 C0 75 1B 8D 48 08", scan_start_rage, scan_size_rage);
		pGetNameVerify = (DWORD64)GetNameVerify();
		PatternReplayInterfaceRageMP = ps_find_idastyle("48 8B 05 ?? ?? ?? ?? 48 8B 1C C8 48 85 DB 0F 84 ?? ?? ?? ?? 8B", scan_start_rage, scan_size_rage); ++SigFountCount;
		addrRagePool = *reinterpret_cast<DWORD64*>(PatternReplayInterfaceRageMP + *reinterpret_cast<DWORD*>(PatternReplayInterfaceRageMP + 3) + 7);
	}

	pSetVehiclePlateText = reinterpret_cast<fpSetVehiclePlateText>(ps_find_idastyle("40 53 48 83 EC 20 48 8B DA E8 ?? ?? ?? ?? 48 85 C0 74 10 48 8B 48 48", scan_start, scan_size));
	pSetEntityCollision = reinterpret_cast<fpSetEntityCollision>(ps_find_idastyle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 20 41 8A F1 41 8A E8 40 8A FA", scan_start, scan_size));
	//pSetEntityVelocity = reinterpret_cast<fpSetEntityVelocity>(ps_find_idastyle("F3 0F 10 4A ? F3 0F 10 52 ? F3 0F 11 40", scan_start, scan_size) - 0xB);
	//WorldToScreenAddress = ps_find_idastyle("48 89 5C 24 ?? 55 56 57 48 83 EC 70 65 4C 8B 0C 25", scan_start, scan_size);  ++SigFountCount;
	pSetVehicleEngineOn = reinterpret_cast<fpSetVehicleEngineOn>(ps_find_idastyle("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 20 41 8A E9 41 8A F8", scan_start, scan_size)); SigFountCount++;
	oSetPedAmmo = reinterpret_cast<SetPedAmmo_t>(ps_find_idastyle("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC 20 41 8B F0 8B FA E8 ?? ?? ?? ?? 48 8B D8", scan_start, scan_size)); SigFountCount++;
	DWORD64 WaypointPatt = ps_find_idastyle("74 1F F3 0F 10 05 ?? ?? ?? ?? F3 0F 11 03", scan_start, scan_size); ++SigFountCount;
	addy.WayPointRead = WaypointPatt + *(DWORD*)(WaypointPatt + 6) + 0xA; SigFountCount++;
	SET_PED_INTO_VEH = reinterpret_cast<tSET_PED_INTO_VEH>(ps_find_idastyle("45 8B E9 41 8B F8", scan_start, scan_size) - 0x38); ++SigFountCount;
	DWORD64 m_entityPoolPattern = ps_find_idastyle("4C 8B 0D ?? ?? ?? ?? 44 8B C1 49 8B 41 08", scan_start, scan_size); ++SigFountCount;
	m_entityPool = reinterpret_cast<MemoryPool**>(m_entityPoolPattern + *reinterpret_cast<DWORD*>(m_entityPoolPattern + 3) + 7); SigFountCount++;
	pSetVehicleOnGroundProperly = reinterpret_cast<fpSetVehicleOnGroundProperly>(ps_find_idastyle("40 84 FF 75 0F 4C", scan_start, scan_size) - 0x51); ++SigFountCount;
	pSetVehicleModKit = reinterpret_cast<fpSetVehicleModKit>(ps_find_idastyle("66 89 45 20 8B 45 20 0D ?? ?? ?? ?? 0F BA F0 1C 89 45 20 66 3D FF FF 74 63", scan_start, scan_size) - 0x77); ++SigFountCount;
	pSetPedIntoVehicle = reinterpret_cast<fpSetPedIntoVehicle>(ps_find_idastyle("48 89 5C 24 ?? 48 89 6C 24 ?? 56 57 41 56 48 83 EC 30 41 8B E8", scan_start, scan_size)); ++SigFountCount;
	pDecorSetInt = reinterpret_cast<fpDecorSetInt>(ps_find_idastyle("48 89 5C 24 ?? 44 89 44 24 ?? 57 48 83 EC 20 48 8B FA E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 74 22 48 8B D7 33 C9 E8 ?? ?? ?? ?? 4C 8D 44 24 ?? 48 8D 54 24 ?? 48 8B CB 89 44 24 48 E8 ?? ?? ?? ?? B0 01", scan_start, scan_size)); ++SigFountCount;
	pSetVehicleMod = reinterpret_cast<fpSetVehicleMod>(ps_find_idastyle("48 89 5C 24 ?? 48 89 6C 24 ?? 56 57 41 56 48 83 EC 30 41 8A E9", scan_start, scan_size)); ++SigFountCount;
	pGetNumVehicleMod = reinterpret_cast<fpGetNumVehicleMod>(ps_find_idastyle("40 53 48 83 EC 20 8B DA 83 FA 0A 7E 10 8D 42 F5 83 F8 0D 77 05 83 C3 19 EB 03 83 EB 0E E8 ?? ?? ?? ?? 48 85 C0 74 2C", scan_start, scan_size)); ++SigFountCount;
	pToggleVehicleMod = reinterpret_cast<fpToggleVehicleMod>(ps_find_idastyle("48 89 5C 24 ?? 57 48 83 EC 20 41 8A F8 8B DA 83 FA 0A", scan_start, scan_size)); ++SigFountCount;
	pSetVehicleFixed = reinterpret_cast<fpSetVehicleFixed>(ps_find_idastyle("40 53 48 83 EC 20 E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 74 28 4C 8B 10", scan_start, scan_size)); ++SigFountCount;
	pNetworkRequestControlOfEntity = reinterpret_cast<fpNetworkRequestControlOfEntity>(ps_find_idastyle("48 89 5C 24 ?? 57 48 83 EC 20 8B D9 E8 ?? ?? ?? ?? 84 C0", scan_start, scan_size)); ++SigFountCount;
	RemoveWeaponFromPed = reinterpret_cast<fpRemoveWeaponFromPed>(ps_find_idastyle("48 89 5C 24 ?? 57 48 83 EC 20 8B FA E8 ?? ?? ?? ?? 33 DB", scan_start, scan_size)); ++SigFountCount;
	SetEntityVisible = reinterpret_cast<SetEntityVisible_t>(ps_find_idastyle("8B 4C 24 48 BA ?? ?? ?? ?? D3", scan_start, scan_size) - 0x3E); ++SigFountCount;
	dw_ViewportPattern = ps_find_idastyle("48 8B 15 ?? ?? ?? ?? 48 8D 2D ?? ?? ?? ?? 48 8B CD", scan_start, scan_size); //++SigFountCount;
	m_viewPort = *reinterpret_cast<CViewPort**>(dw_ViewportPattern + *reinterpret_cast<DWORD*>(dw_ViewportPattern + 3) + 7);
	//CreatePickup = reinterpret_cast<CreatePickup_t>( ps_find_idastyle("48 8D 68 C1 48 81 EC ?? ?? ?? ?? 45 8B F9", scan_start, scan_size) - 0x18); ++SigFountCount;
	GetWayPoint = reinterpret_cast<pGetWayPoint>(ps_find_idastyle("33 C0 49 8B C8 39 19 74 1F", scan_start, scan_size) - 0x55); ++SigFountCount;
	oCLEAR_PED_TASKS_IMMEDIATELY = reinterpret_cast<CLEAR_PED_TASKS_IMMEDIATELY_t>(ps_find_idastyle("40 53 48 83 EC 30 E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 0F 84", scan_start, scan_size)); ++SigFountCount;
	CLEAR_PED_TASKS = reinterpret_cast<CLEAR_PED_TASKS_t>(ps_find_idastyle("40 53 48 83 EC 30 E8 ?? ?? ?? ?? 48 8B D8 48 85 C0 74 55", scan_start, scan_size)); ++SigFountCount;
	GetGameplayCamCoord = reinterpret_cast<pGetGameplayCamCoord>(ps_find_idastyle("40 53 48 83 EC 20 48 8B D9 E8 ?? ?? ?? ?? 8B 90", scan_start, scan_size)); ++SigFountCount;
	GetCamRot = reinterpret_cast<pGetCamRot>(ps_find_idastyle("48 89 5C 24 ?? 57 48 83 EC 30 8B DA 48 8B F9 E8 ?? ?? ?? ?? 48 8D 4C 24 ?? 48 8D 90", scan_start, scan_size)); ++SigFountCount;
	centity_to_handle = reinterpret_cast<centity_to_handle_fn>(ps_find_idastyle("74 23 4C 8B 05", scan_start, scan_size) - 0x56), ++SigFountCount;
	centity_to_handle_veh = reinterpret_cast<centity_to_handle_fn_veh>(centity_to_handle);
	DWORD64 FrameCount_pattern = DWORD64(ps_find_idastyle("8B 15 ?? ?? ?? ?? 41 FF CF FF", scan_start, scan_size)); ++SigFountCount;
	FrameCount = (FrameCount_pattern + *(DWORD*)(FrameCount_pattern + 0x2) + 0x6);
	DWORD64 Veh_Speed_Pattern = DWORD64(ps_find_idastyle("38 1D ?? ?? ?? ?? 74 0D 0F", scan_start, scan_size) - 0x17); ++SigFountCount;
	Veh_Speed = (Veh_Speed_Pattern + *(DWORD*)(Veh_Speed_Pattern + 0x4) + 0x8);
	DWORD64 dwshoot_bullets1 = (ps_find_idastyle("4C 8B DC 48 81 EC ?? ?? ?? ?? 8B 84", scan_start, scan_size)); ++SigFountCount; //FiveM Found
	//DWORD64 dwshoot_bullets2 = 0x0;
	//DWORD64 dwshoot_bullets3 = 0x0;
	//if (!dwshoot_bullets1)dwshoot_bullets2 = ps_find_idastyle("54 41 5B 48 81 EC ? ? ? ? 8B", scan_start, scan_size), ++SigFountCount;//FiveM not found    //48 83 EC 68 83 64 24 ? ? 83 64 24 ? ? 8A 
	//if (!dwshoot_bullets2)dwshoot_bullets3 = ps_find_idastyle("48 8D 64 24 ?? FF 64 24 F8 48 83 EC 68", scan_start, scan_size) + 0x9, ++SigFountCount; //FiveM not found
	//if (dwshoot_bullets3)shoot_bullets = reinterpret_cast<fpShootSingleBulletBetweenCoords>(dwshoot_bullets3);
	//if (dwshoot_bullets2)shoot_bullets = reinterpret_cast<fpShootSingleBulletBetweenCoords>(dwshoot_bullets2);
	if (dwshoot_bullets1)shoot_bullets = reinterpret_cast<fpShootSingleBulletBetweenCoords>(dwshoot_bullets1);
	GiveDelayedWeaponToPed = reinterpret_cast<fpGiveDelayedWeaponToPed>(ps_find_idastyle("74 33 44 8B C6 8B D7 E8 ?? ?? ?? ?? 40", scan_start, scan_size) - 0x57); ++SigFountCount;
	_START_SHAPE_TEST_RAY = reinterpret_cast<tSTART_SHAPE_TEST_RAY>(ps_find_idastyle("41 8B CE C7 85 ?? ?? ?? ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? B2", scan_start, scan_size) - 0xFE); ++SigFountCount;
	_GET_RAYCAST_RESULT = reinterpret_cast<t_GET_RAYCAST_RESULT>(ps_find_idastyle("0F 95 C1 41 89 0F 85 C9 74", scan_start, scan_size) - 0x71);
	SetVehicleForwardSpeed = reinterpret_cast<SetVehicleForwardSpeed_t>(ps_find_idastyle("48 8B 03 48 8D 54 24 ?? 0F 28 5B", scan_start, scan_size) - 0x16); ++SigFountCount;
	GetBone2 = reinterpret_cast<GetBoneFromMask2>(ps_find_idastyle("48 8B 01 41 8B E8 48 8B F2", scan_start, scan_size) - 0x14); ++SigFountCount; //GetBone1 = reinterpret_cast<GetBoneFromMask1>((DWORD64)GetBone2 - 0xD8);
	DWORD64 pGameCamManager_pattern = ps_find_idastyle("48 8B 05 ?? ?? ?? ?? 4A 8B 1C F0", scan_start, scan_size); ++SigFountCount;
	addy.pGameCamManager = (CGameCameraAngles*)(pGameCamManager_pattern + *(DWORD*)(pGameCamManager_pattern + 0x3) + 0x7);
	ReplayInterface_pattern = (ps_find_idastyle("48 8D 0D ?? ?? ?? ?? 48 8B D7 E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 8A D8 E8 ?? ?? ?? ?? 84 DB 75 13 48 8D 0D ?? ?? ?? ?? 48 8B D7 E8 ?? ?? ?? ?? 84 C0 74 BC 8B 8F", scan_start, scan_size)); ++SigFountCount; //48 8B 05 ?? ?? ?? ?? 41 8B 1E
	string SaveGameVersionPath = "none";
	static char* buf = nullptr;
	static size_t sz = 1024;
	if (_dupenv_s(&buf, &sz, "LocalAppData") == 0 && buf != nullptr) {
		//printf("EnvVarName = %s\n", buf);
		SaveGameVersionPath = buf;
		SaveGameVersionPath += "\\Game_Version.cfg";
		free(buf);
	}
	int Game_Ver = 0;
	string line;
	ifstream myfilde(SaveGameVersionPath);
	if (myfilde.is_open()) {
		while (std::getline(myfilde, line)) {
			Game_Ver = stoi(line);
		}
		myfilde.close();
	}

	if (Game_Ver > 0) {
		//RageName 1.1 0xA4
		OFFvehSpeed = 0xB70;
		OFFvehMgr = 0xD30;
		OFFplayerInfoMgr = 0x10C8;
		OFFweaponMgr = 0x10D8;
		OFFinVeh = 0xE52;
		OFFarmour = 0x14E0;
		OFFrecoil = 0x2F4;
		OFFspread = 0x84;
		OFFspread2 = 0x124;
		OFFnoGravity = 0x1110;
		OFF_FixVehAimVec = 0x2AC;
		OFFpedType = 0x10B8;
	}
	else {
		OFFvehMgr = 0xD28;
		OFFplayerInfoMgr = 0x10B8;
		OFFweaponMgr = 0x10C8;
		//OFFinVeh = 0x146D;
		//OFFinVeh = 0x1465;
		OFFinVeh = 0xE42;
		OFFvehSpeed = 0xB30;
		OFFarmour = 0x14B8;
		OFFrecoil = 0x2E8;
		OFFspread = 0x84;
		OFFspread2 = 0x0;
		OFFnoGravity = 0x1100;
		OFF_FixVehAimVec = 0x28C;
		OFFpedType = 0x10A8;
	}
	addy.ReplayInterface = *reinterpret_cast<CReplayInterface**>(ReplayInterface_pattern + *(DWORD*)(ReplayInterface_pattern + 0x3) + 0x7);
	LocalPlayerWrapper_pattern = ps_find_idastyle("48 8B 05 ?? ?? ?? ?? 45 0F C6 C0", scan_start, scan_size); ++SigFountCount; //48 8B 05 ?? ?? ?? ?? 45 0F C6 C0 SigFountCount++;
	addy.LocalPlayerWrapper = *reinterpret_cast<CWorld**>(LocalPlayerWrapper_pattern + *(DWORD*)(LocalPlayerWrapper_pattern + 0x3) + 0x7);
	++SigFountCount;
	return true;
}

float get_distance(Vector3 pointA, Vector3 pointB) {
	float x_ba = (float)(pointB.x - pointA.x);
	float y_ba = (float)(pointB.y - pointA.y);
	float z_ba = (float)(pointB.z - pointA.z);
	float y_2 = y_ba * y_ba;
	float x_2 = x_ba * x_ba;
	float sum_2 = y_2 + x_2;
	return (float)sqrtf(sum_2 + z_ba);
}

extern bool EnableNoClipRoutineHotKey;
void NoClip(float Speed) {
	if (EnableNoClipRoutineHotKey) {
		if (addy.localPlayer->IsInVehicle())
			VehSpeed(0.0f), addy.localPlayer->gVehicleManager()->SetPosClass(addy.localPlayer->gVehicleManager()->fPosition + CamRotLocal * (Speed / 15.0f));
		else
			addy.localPlayer->fPosition = addy.localPlayer->fPosition + CamRotLocal * (Speed / 15.0f);
	}
}

static CVehicleManager* AimMouseTarget = NULL;
static CVehicleManager* AimMouseTargetLoop = NULL;

void VehicleEsp(bool visMenu = false) {
	if (Tools->IsValid(addy.ReplayInterface) && Tools->IsValid(addy.ReplayInterface->vehicle_interface))
		for (int i = 0; i <= 255; i++) {
			auto tmpObject = addy.ReplayInterface->vehicle_interface->get_vehicle(i);
			if (Tools->IsValid(tmpObject)) {
				auto Distance = float(Tools->Get3dDistance(addy.localPlayer->fPosition, Vector3(tmpObject->fPosition.x, tmpObject->fPosition.y, tmpObject->fPosition.z + 0.85f)));
				Vector3 ObjectPos2DTOP(0.0f, 0.0f, 0.0f);
				if (Tools->IsValid(tmpObject->ModelInfo) && Tools->IsValid(tmpObject->_ObjectNavigation)) {
					if (W2S(Vector3(tmpObject->fPosition.x, tmpObject->fPosition.y, tmpObject->fPosition.z + tmpObject->ModelInfo->MaxDim.z), ObjectPos2DTOP)) {
						char VehPrefix[256];

						if (Hacks->VehicleEsp_Name && !Hacks->VehicleEsp_Distance)
							sprintf_s(VehPrefix, "%s", tmpObject->ModelInfo->Name);

						if (Hacks->VehicleEsp_Distance && !Hacks->VehicleEsp_Name)
							sprintf_s(VehPrefix, "%1.0f", Distance);

						if (Hacks->VehicleEsp_Name && Hacks->VehicleEsp_Distance)
							sprintf_s(VehPrefix, "%s(%1.0f)", tmpObject->ModelInfo->Name, Distance);

						if (Hacks->VehicleEsp_3Dbox)
							renderer->Draw3DBoxVehicle(tmpObject->fPosition, tmpObject->_ObjectNavigation->Rotation, tmpObject->ModelInfo->MinDim, tmpObject->ModelInfo->MaxDim, AimMouseTarget == tmpObject ? DirectX::Colors::Green.v : DirectX::Colors::Red.v);

						if ((Hacks->VehicleEsp_Name || Hacks->VehicleEsp_Distance) && !visMenu)
							renderer->Draw2dText(ObjectPos2DTOP.x, ObjectPos2DTOP.y - 10.0f, Color::XMFLOAT4{ 1.000000000f, 0.000000000f, 0.000000000f, 0.5f }, Hacks->Esp_Font_Size, true, VehPrefix);
					}
				}
			}
		}
}

void PickUpEsp() {
	if (Tools->IsValid(addy.ReplayInterface) && Tools->IsValid(addy.ReplayInterface->pickup_interface))
		for (int i = 0; i < 73; i++) {
			auto tmpObject = addy.ReplayInterface->pickup_interface->get_pickup(i);
			if (Tools->IsValid(tmpObject)) {
				auto Distance = Tools->Get3dDistance(addy.localPlayer->fPosition, tmpObject->v3VisualPos);
				Vector3 ObjectPos2D(0.0f, 0.0f, 0.0f);
				if (W2S(tmpObject->v3VisualPos, ObjectPos2D)) {
					renderer->Draw2dText(ObjectPos2D.x, ObjectPos2D.y, Colors::Red.v, Hacks->Esp_Font_Size, true, "Pickup(%1.0fm)", Distance);
				}
			}
		}
}

static struct {
	int Hash;
	const char* Name;
} Object_Names[] = {
	{ -54433116, "hei_prop_heist_weed_block_01" },
	{ -680115871, "hei_prop_heist_weed_block_01b" },
	{ -553616286, "hei_prop_heist_weed_pallet" },
	{ 1228076166, "hei_prop_heist_weed_pallet_02" },
	{ 2021859795, "p_weed_bottle_s" },
	{ 452618762, "prop_weed_01" },
	{ -305885281, "prop_weed_02" },
	{ 370119105, "prop_weed_03" },
	{ -1688127, "prop_weed_block_01" },
	{ 671777952, "prop_weed_bottle" },
	{ 243282660, "prop_weed_pallet" },
	{ -232602273, "prop_weed_tub_01" },
	{ 1913437669, "prop_weed_tub_01b" },
	{ 289396019, "prop_money_bag_01" },
	{ 1585260068, "prop_michael_backpack" },
	{ 1203231469, "p_michael_backpack_s" },
	{ 1049338225, "hei_prop_hei_drug_case" },
	{ 525896218, "hei_prop_hei_drug_pack_01a" },
	{ -395076527, "hei_prop_hei_drug_pack_01b" },
	{ -1907742965, "hei_prop_hei_drug_pack_02" },
	{ -2127785247, "ng_proc_drug01a002" },
	{ 528555233, "prop_drug_package" },
	{ -1964997422, "prop_drug_package_02" },
	{ -1620734287, "prop_mp_drug_pack_blue" },
	{ 138777325, "prop_mp_drug_pack_red" },
	{ 765087784, "prop_mp_drug_package" },
	{ 1808635348, "p_meth_bag_01_s" },
	{ 285917444, "prop_meth_bag_01" },
	{ -2059889071, "prop_meth_setup_01" }
};

void ObjectEsp(int Hash_ESP) {
	//if (Tools->IsValid(addy.ReplayInterface) && Tools->IsValid(addy.ReplayInterface->object_interface))
	//	for (int i = 0; i < addy.ReplayInterface->object_interface->iMaxObjects - 1; i++) {
	//		auto tmpObject = addy.ReplayInterface->object_interface->get_object(i);
	//		if (Tools->IsValid(tmpObject) && Tools->IsValid(tmpObject->ModelInfo())) {
	//			auto Distance = Tools->Get3dDistance(addy.localPlayer->fPosition, tmpObject->fPosition);
	//			auto ColorObject = Colors::Red.v;
	//			bool IsIlligalItem = false;
	//			static char* ObjName;
	//			for (int cObj = 0; cObj < std::size(Object_Names); cObj++){//Check for illigal items
	//				if (tmpObject->ModelInfo()->GetHash() == Object_Names[cObj].Hash)
	//					ColorObject = Colors::Green, IsIlligalItem = true, ObjName = Object_Names[cObj].Name;
	//			}
	//			if(Hacks->ObjectEsp == 3 ? IsIlligalItem : true){
	//				Vector3 ObjectPos2D(0, 0, 0);
	//				if (W2S(tmpObject->fPosition, ObjectPos2D) && Distance <= Hacks->Npc_Max_Distance) {
	//					if (Hash_ESP == 2) {
	//						renderer->Draw2dText(ObjectPos2D.x, ObjectPos2D.y, ColorObject, Hacks->Esp_Font_Size, true, "%x (%1.1fm)", tmpObject->ModelInfo()->GetHash(), Distance);
	//					} else{
	//						if (IsIlligalItem) {
	//							renderer->Draw2dText(ObjectPos2D.x, ObjectPos2D.y, ColorObject, Hacks->Esp_Font_Size, true, "%s(%1.1fm)", ObjName, Distance);
	//						} else {
	//							renderer->Draw2dText(ObjectPos2D.x, ObjectPos2D.y, ColorObject, Hacks->Esp_Font_Size, true, "Object(%1.1fm)", Distance);
	//						}
	//						
	//					}
	//				}
	//			}
	//		}
	//	}
}




