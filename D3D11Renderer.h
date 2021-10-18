#pragma once

#include "DxINC\ScreenGrab.h"
#include "DxINC\SimpleMath.h"
#include "DxINC\SpriteBatch.h"
#include "DxINC\PrimitiveBatch.h"
#include "Helper.h"
#include "D3D11StateSaver.h"
#include "D3D11Shader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class D3D11Renderer
{
private:
	struct COLOR_VERTEX
	{
		Vector3	Position;
		Color Color[4];
	};
	bool restoreState = false;
public:
	ID3D11InputLayout* mInputLayout = NULL;
	//~D3D11Renderer();

	IDXGISwapChain* swapChain = NULL;
	ID3D11Device* device = NULL;
	ID3D11DeviceContext* deviceContext = NULL;
	ID3D11Buffer* mVertexBuffer = NULL;
	ID3D11VertexShader* mVS;
	ID3D11PixelShader* mPS;
	ID3D11BlendState* transparency;
	D3D11StateSaver* stateSaver = NULL;

	D3D11Renderer(IDXGISwapChain* SwapChain) {
		this->swapChain = SwapChain;
		this->device = NULL;
		this->deviceContext = NULL;
		this->mVS = NULL;
		this->mPS = NULL;
		this->transparency = NULL;
		this->mInputLayout = NULL;
		this->mVertexBuffer = NULL;
		this->stateSaver = new D3D11StateSaver();
	}
	bool Initialize();
	//void DrawCursor(float x, float y, bool Engine_Rect = 0);
	void DrawCross(float X, float Y, float Width, float Height, Color dColor, bool Rainbow = 0);
	void DrawDot(float X, float Y, float Width, float Height, Color dColor, bool Center);
	void FillRect(float x, float y, float w, float h, Color color);
	void DrawBorder(float x, float y, float w, float h, float thickness, Color BorderColor);
	void DrawBorder2(float x, float y, float w, float h, float thickness, Color BorderColor);
	void DrawBox(float x, float y, float w, float h, Color BoxColor, Color BorderColor);
	void DrawAimCross(float Size, Color BorderColor);
	void DrawLine(float x1, float y1, float x2, float y2, Color color);
	bool DrawHealthBar(float x, float y, float w, float health, float max, float h = 2.0f);
	void Circle(float X, float Y, float radius, float numSides, Color Circle);
	void DrawCircle(float x0, float y0, float radius, float thickness, Color color);
	bool DrawArmorBar(float x, float y, float w, float Armor, float max);
	bool DrawHealthBar(float x, float y, float w, float h, float health, float max, bool Side = 0);
	bool DrawArmorBar(float x, float y, float w, float h, float Armor, float max, bool Side = 0);
	void Draw2dText(float x, float y, Color Colortxt, float scale, bool Center, const char* szText, ...);
	//bool W2S(Vector3 WorldPos, Vector3* ScrPos);
	bool Draw3DBox(Vector3 Position, Vector3 Rotation, Color BoxColor, float HeadHight = 0.852f, float FoodHight = 1.0f, bool CrosshairInOnly = false);
	bool Draw3DBoxVehicle(Vector3 Position, Vector3 Rotation, Vector3 min, Vector3 max, Color BoxColor, bool CrosshairInOnly = false);
	float GetWidth();
	float GetHeight();
	void BeginScene();
	void EndScene();
};
static bool VisMenu = false;
extern std::unique_ptr<D3D11Renderer> renderer;


