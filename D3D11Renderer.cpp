#include "D3D11Renderer.h"
#include "Tools.h"
#include "Classes.h"

std::unique_ptr<D3D11Renderer> renderer = NULL;

using VertexType = DirectX::VertexPositionColor;
extern std::unique_ptr<SpriteFont>						 m_font;
extern std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
extern std::unique_ptr<SpriteBatch>						 DX11SpriteBatch;
extern std::unique_ptr<DirectX::CommonStates> m_states;
extern Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
extern std::unique_ptr<DirectX::BasicEffect> m_effect;

bool D3D11Renderer::Initialize()
{
	HRESULT hr = E_FAIL;

	if (!this->swapChain)
		return false;

	this->swapChain->GetDevice(__uuidof(this->device), (void**)&this->device);
	if (!this->device)
		return false;

	this->device->GetImmediateContext(&this->deviceContext);
	if (!this->deviceContext)
		return false;

	typedef HRESULT(__stdcall* D3DCompile_t)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob* ppErrorMsgs);
	D3DCompile_t myD3DCompile = (D3DCompile_t)GetProcAddress(GetD3DCompiler(), "D3DCompile");
	if (!myD3DCompile)
		return false;

	ID3D10Blob* VS, * PS;
	hr = myD3DCompile(D3D11FillShader, sizeof(D3D11FillShader), NULL, NULL, NULL, "VS", "vs_4_0", 0, 0, &VS, NULL);
	if (FAILED(hr))
		return false;

	hr = this->device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &this->mVS);
	if (FAILED(hr))
	{
		SAFE_RELEASE(VS);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->device->CreateInputLayout(layout, ARRAYSIZE(layout), VS->GetBufferPointer(), VS->GetBufferSize(), &this->mInputLayout);
	SAFE_RELEASE(VS);
	if (FAILED(hr))
		return false;

	myD3DCompile(D3D11FillShader, sizeof(D3D11FillShader), NULL, NULL, NULL, "PS", "ps_4_0", 0, 0, &PS, NULL);
	if (FAILED(hr))
		return false;

	hr = this->device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &this->mPS);
	if (FAILED(hr))
	{
		SAFE_RELEASE(PS);
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = 4 * sizeof(COLOR_VERTEX);
	bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	hr = this->device->CreateBuffer(&bufferDesc, NULL, &this->mVertexBuffer);
	if (FAILED(hr))
		return false;

	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(blendStateDescription));

	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	hr = this->device->CreateBlendState(&blendStateDescription, &this->transparency);

	if (FAILED(hr))
		return false;
	return true;
}

extern float ScreenWidth;
extern float ScreenHight;

const float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

void D3D11Renderer::FillRect(float x, float y, float w, float h, Color color) {
	if (!Tools->IsValid(this->deviceContext))
		return;

	float a = color.A();
	float r = color.R();
	float g = color.G();
	float b = color.B();
	static D3D11_VIEWPORT vp;
	vp.Width = ScreenWidth;
	vp.Height = ScreenHight;

	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;

	float xx0 = 2.0f * (x0 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y0 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;

	COLOR_VERTEX* v = NULL;
	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(this->mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	v[0].Position.x = (float)x0;
	v[0].Position.y = (float)y0;
	v[0].Position.z = 0;
	*v[0].Color = { r, g, b, a };

	v[1].Position.x = (float)x1;
	v[1].Position.y = (float)y1;
	v[1].Position.z = 0;
	*v[1].Color = { (float)r, (float)g, (float)b, (float)a };

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;
	v[0].Position.z = 0;
	*v[0].Color = { (float)r, (float)g, (float)b, (float)a };

	v[1].Position.x = xx1;
	v[1].Position.y = yy0;
	v[1].Position.z = 0;
	*v[1].Color = { (float)r, (float)g, (float)b, (float)a };

	v[2].Position.x = xx0;
	v[2].Position.y = yy1;
	v[2].Position.z = 0;
	*v[2].Color = { (float)r, (float)g, (float)b, (float)a };

	v[3].Position.x = xx1;
	v[3].Position.y = yy1;
	v[3].Position.z = 0;
	*v[3].Color = { (float)r, (float)g, (float)b, (float)a };

	this->deviceContext->Unmap(this->mVertexBuffer, NULL);
	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;

	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);

	this->deviceContext->Draw(4, 0);
}

void D3D11Renderer::DrawLine(float x1, float y1, float x2, float y2, Color color)
{
	if (this->deviceContext == NULL)
		return;

	float a = color.A();
	float r = color.R();
	float g = color.G();
	float b = color.B();

	static D3D11_VIEWPORT vp;

	vp.Width = ScreenWidth;
	vp.Height = ScreenHight;

	float xx0 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x2 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y2 - 0.5f) / vp.Height;

	COLOR_VERTEX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(this->mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;

	v[0].Position.z = 0;
	*v[0].Color = { (float)r, (float)g, (float)b, (float)a };

	v[1].Position.x = xx1;
	v[1].Position.y = yy1;
	v[1].Position.z = 0;
	*v[1].Color = { (float)r, (float)g, (float)b, (float)a };

	this->deviceContext->Unmap(this->mVertexBuffer, NULL);

	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;

	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(2, 0);
}

void D3D11Renderer::DrawCircle(float x0, float y0, float radius, float thickness, Color color)
{
	float x = radius, y = 0;
	float radiusError = 1 - x;

	while (x >= y)
	{
		this->FillRect(x + x0, y + y0, thickness, thickness, color);
		this->FillRect(y + x0, x + y0, thickness, thickness, color);
		this->FillRect(-x + x0, y + y0, thickness, thickness, color);
		this->FillRect(-y + x0, x + y0, thickness, thickness, color);
		this->FillRect(-x + x0, -y + y0, thickness, thickness, color);
		this->FillRect(-y + x0, -x + y0, thickness, thickness, color);
		this->FillRect(x + x0, -y + y0, thickness, thickness, color);
		this->FillRect(y + x0, -x + y0, thickness, thickness, color);
		y++;
		if (radiusError < 0)
		{
			radiusError += 2 * y + 1;
		}
		else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}



void D3D11Renderer::Circle(float X, float Y, float radius, float numSides, Color Circle)
{
	Vector2 Line[128];
	float Step = DirectX::XM_PI * 2.0f / numSides;
	int Count = 0;
	for (float a = 0; a < DirectX::XM_PI * 2.0f; a += Step)
	{
		float X1 = radius * cosf(a) + X;
		float Y1 = radius * sinf(a) + Y;
		float X2 = radius * cosf(a + Step) + X;
		float Y2 = radius * sinf(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
		this->DrawLine(X1, Y1, X2, Y2, Circle);
	};
}

void D3D11Renderer::DrawBorder(float x, float y, float w, float h, float px, Color BorderColor)
{
	FillRect(x - px, y - px, (w + (px + px)), (h + (px + px)), BorderColor);
}

void D3D11Renderer::DrawBorder2(float x, float y, float w, float h, float thickness, Color BorderColor)
{
	//Top horiz line
	FillRect(x, y, w, thickness, BorderColor);
	//Left vertical line
	FillRect(x, y, thickness, h, BorderColor);
	//right vertical line
	FillRect((x + w), y, thickness, h, BorderColor);
	//bottom horiz line
	FillRect(x, y + h, w + thickness, thickness, BorderColor);
}

void D3D11Renderer::DrawBox(float x, float y, float w, float h, Color BoxColor, Color BorderColor)
{
	//DrawBorder(x, y, w, h, 1, BorderColor);
	FillRect(x, y, w + 2, h + 2, BorderColor);
	FillRect(x + 1, y + 1, w, h, BoxColor);
}

void D3D11Renderer::DrawAimCross(float Size, Color CrossColor)
{
	float ScreenCenterX = GetWidth() / 2.0f;
	float ScreenCenterY = GetHeight() / 2.0f;
	DrawBorder2(ScreenCenterX - (Size / 2.0f), ScreenCenterY - (Size / 2.0f), Size, Size, 1.0f, CrossColor);
	DrawLine(ScreenCenterX - (Size / 2.0f), ScreenCenterY - (Size / 2.0f), ScreenCenterX + (Size / 2.0f), ScreenCenterY + (Size / 2.0f), CrossColor);
	DrawLine(ScreenCenterX + (Size / 2.0f), ScreenCenterY - (Size / 2.0f), ScreenCenterX - (Size / 2.0f), ScreenCenterY + (Size / 2.0f), CrossColor);
}

bool D3D11Renderer::DrawHealthBar(float x, float y, float w, float health, float max, float h)
{
	this->DrawHealthBar(x, y, w, 2.0f, health, max, false);
	return true;
}

bool D3D11Renderer::DrawHealthBar(float x, float y, float w, float h, float health, float max, bool Side) {
	if (!Side) {
		if (health > max)
			return false;

		if (w < 5.0f)
			return false;

		if (health < 0.0f)
			health = 0.0f;

		float step = (w / max);

		if (step > max)
			return false;


		float draw = (step * health);

		FillRect(x, y, w, h, Color(0.2f, 0, 0, 0)); //Background
		FillRect(x, y, draw, h, Colors::Lime.v); //Healthbar
		DrawBorder2(x - 1, y - 1, w + 1, h + 1, 1, Colors::Black.v); //Border
	}
	else {
		if (health > max)
			return false;

		if (h < 5.0f)
			return false;

		if (health < 0.0f)
			health = 0.0f;

		float step = (h / max);

		if (step > max)
			return false;

		float draw = (step * health);

		FillRect(x, y, w, h, Color(0.2f, 0, 0, 0)); //Background
		FillRect(x, y, w, draw, Colors::Lime.v); //Healthbar
		DrawBorder2(x - 1.0f, y - 1.0f, w + 1.0f, h + 1.0f, 1.0f, Colors::Black.v); //Border
	}
	return true;
}

bool D3D11Renderer::DrawArmorBar(float x, float y, float w, float Armor, float max)
{
	this->DrawArmorBar(x, y, w, 2.0f, Armor, max);
	return true;
}

bool D3D11Renderer::DrawArmorBar(float x, float y, float w, float h, float Armor, float max, bool Side) {
	if (!Side) {
		if (Armor > max)
			return false;

		if (w < 5.0f)
			return false;

		if (Armor < 0.0f)
			Armor = 0.0f;

		float step = (w / max);

		if (step > max)
			return false;

		float draw = (step * Armor);

		this->FillRect(x, y, w, h, Color(80, 0, 0, 0));; //Background
		this->FillRect(x, y, draw, h, Colors::Blue.v);;//Armorhbar
		DrawBorder2(x - 1, y - 1, w + 1, h + 1, 1, Colors::Black.v);//Border
	}
	else {
		if (Armor > max)
			return false;

		if (h < 5.0f)
			return false;

		if (Armor < 0.0f)
			Armor = 0.0f;

		float step = (h / max);

		if (step > max)
			return false;

		float draw = (step * Armor);

		this->FillRect(x, y, w, h, Color(0.2f, 0, 0, 0));; //Background
		this->FillRect(x, y, w, draw, Colors::Blue.v);;//Armorhbar
		DrawBorder2(x - 1, y - 1, w + 1, h + 1, 1, Colors::Black.v);//Border
	}
	return true;
}

void D3D11Renderer::DrawDot(float X, float Y, float Width, float Height, Color dColor, bool Center)
{
	if (Center) {
		this->FillRect(X - 1.0f - (Width / 2.0f), Y - 1.0f - (Height / 2.0f), Width + 2.0f, Height + 2.0f, Colors::Black.v);
		this->FillRect(X - (Width / 2.0f), Y - (Height / 2.0f), Width, Height, dColor);
	}
	else {
		this->FillRect(X - 1.0f, Y - 1.0f, Width + 2.0f, Height + 2.0f, Colors::Black.v);
		this->FillRect(X, Y, Width, Height, dColor);
	}
}

void D3D11Renderer::DrawCross(float X, float Y, float Width, float Height, Color dColor, bool Rainbow)
{
	if (Rainbow) {
		this->FillRect(X - 20.0f, Y, 40.0f, 1.0f, Colors::Purple.v);//Purple
		this->FillRect(X, Y - 20.0f, 1.0f, 40.0f, Colors::Purple.v);

		this->FillRect(X - 17.0f, Y, 34.0f, 1.0f, Colors::Blue.v);//Blue
		this->FillRect(X, Y - 17.0f, 1.0f, 34.0f, Colors::Blue.v);

		this->FillRect(X - 14.0f, Y, 28.0f, 1.0f, Colors::Cyan.v);//Cyan
		this->FillRect(X, Y - 14.0f, 1.0f, 28.0f, Colors::Cyan.v);

		this->FillRect(X - 11.0f, Y, 22.0f, 1.0f, Colors::Green.v);//Green
		this->FillRect(X, Y - 11.0f, 1.0f, 22.0f, Colors::Green.v);

		this->FillRect(X - 9.0f, Y, 18.0f, 1.0f, Colors::Yellow.v);//Yellow
		this->FillRect(X, Y - 9.0f, 1.0f, 18.0f, Colors::Yellow.v);

		this->FillRect(X - 6.0f, Y, 12.0f, 1.0f, Colors::Orange.v);//Orange
		this->FillRect(X, Y - 6.0f, 1.0f, 12.0f, Colors::Orange.v);

		this->FillRect(X - 3.0f, Y, 6.0f, 1.0f, Colors::Red.v);//Red
		this->FillRect(X, Y - 3.0f, 1.0f, 6.0f, Colors::Red.v);
	}
	else {
		FillRect(X - Width, Y, Width * 2.0f, 1.0f, dColor);
		FillRect(X, Y - Height, 1.0f, Height * 2.0f, dColor);
	}
}

float D3D11Renderer::GetWidth()
{
	D3D11_VIEWPORT vp;
	UINT nvp = 1;
	this->deviceContext->RSGetViewports(&nvp, &vp);
	return vp.Width;
}

float D3D11Renderer::GetHeight()
{
	D3D11_VIEWPORT vp;
	UINT nvp = 1;
	this->deviceContext->RSGetViewports(&nvp, &vp);
	return vp.Height;
}

void D3D11Renderer::BeginScene()
{
	this->stateSaver->saveCurrentState(this->deviceContext);

	renderer->deviceContext->OMSetBlendState(this->transparency, nullptr, 0xFFFFFFFF);
	//renderer->deviceContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	//renderer->deviceContext->RSSetState(m_states->CullNone());

	//m_effect->Apply(renderer->deviceContext);
	//renderer->deviceContext->IASetInputLayout(m_inputLayout.Get());
	//m_batch->Begin();

	DX11SpriteBatch->Begin(SpriteSortMode_Deferred);
}

void D3D11Renderer::EndScene()
{
	DX11SpriteBatch->End();
	//m_batch->End();
	this->stateSaver->restoreSavedState();
}

//std::wstring ConvertToWStr(const std::string& s)
//{
//	int len = 0;
//	int slength = (int)s.length() + 1;
//	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
//	wchar_t* buf = new wchar_t[len];
//	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
//	std::wstring r(buf);
//	delete[] buf;
//	return r;
//}

extern std::unique_ptr<Options> Hacks;
void D3D11Renderer::Draw2dText(float x, float y, Color Colortxt, float scale, bool Center, const char* szText, ...) {
	char textbuf[1024] = { '\0' };
	va_list VAL;
	va_start(VAL, szText);
	vsprintf_s(textbuf, szText, VAL);
	va_end(VAL);
	XMFLOAT2 origin(0.0f, 0.0f);
	XMFLOAT2 pos = { x, y };

	std::wstring text = Tools->asciiDecode(textbuf);
	auto wtext = text.c_str();
	if (Center)
	{
		XMVECTOR size = m_font->MeasureString(wtext);
		float sizeX = XMVectorGetX(size);
		float sizeY = XMVectorGetY(size);
		origin = XMFLOAT2(sizeX / 2.0f, sizeY / 2.0f);
	}
	else {
		XMVECTOR size = m_font->MeasureString(wtext);
		float sizeY = XMVectorGetY(size);
		origin = XMFLOAT2(0.0f, sizeY / 2.0f);
	}
	if (Hacks->Text_Shadow == 1) {
		m_font->DrawString(DX11SpriteBatch.get(), wtext, pos + Vector2(1.5f, 1.5f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
		m_font->DrawString(DX11SpriteBatch.get(), wtext, pos + Vector2(-1.5f, 1.5f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
	}
	if (Hacks->Text_Shadow == 2) {
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(1.f, 1.f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(-1.f, 1.f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(-1.f, -1.f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(1.f, -1.f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
	}
	if (Hacks->Text_Shadow == 3) {
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(1.5f, 1.5f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(-1.5f, 1.5f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(-1.5f, -1.5f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
		m_font->DrawString(DX11SpriteBatch.get(), wtext,
			pos + Vector2(1.5f, -1.5f), Colors::Black, 0.f, origin, scale, SpriteEffects_None);
	}

	m_font->DrawString(DX11SpriteBatch.get(), wtext, pos, Colortxt, 0.f, origin, scale, SpriteEffects_None);
}
extern BOOL W2S(Vector3 WorldPos, Vector3& ScrPos);
bool D3D11Renderer::Draw3DBox(Vector3 Position, Vector3 Rotation, Color BoxColor, float HeadHight, float FoodHight, bool CrosshairIn)
{
	float rot = acosf(Rotation.x) * 180.0f / DirectX::XM_PI;
	if (asinf(Rotation.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;

	Vector3 HeadPos = Vector3(Position.x, Position.y, Position.z + HeadHight);
	Vector3 FeetPos = Vector3(Position.x, Position.y, Position.z - FoodHight);

	Vector3 t[8];

	Vector4 Cosines;
	Vector4 Sines;
	Vector4 Corners = Vector4(XMConvertToRadians(rot - 45.0f), XMConvertToRadians(rot - 135.0f), XMConvertToRadians(rot + 45.0f), XMConvertToRadians(rot + 135.0f));
	Cosines.x = cosf(Corners.x);
	Cosines.y = cosf(Corners.y);
	Cosines.z = cosf(Corners.z);
	Cosines.w = cosf(Corners.w);
	Sines.x = sinf(Corners.x);
	Sines.y = sinf(Corners.y);
	Sines.z = sinf(Corners.z);
	Sines.w = sinf(Corners.w);

	float radius = 0.5f;

	Vector3 HeadLeftForward = HeadPos + Vector3(Cosines.x * radius, Sines.x * radius, 0.0f);
	Vector3 HeadLeftBackward = HeadPos + Vector3(Cosines.y * radius, Sines.y * radius, 0.0f);
	Vector3 HeadRightForward = HeadPos + Vector3(Cosines.z * radius, Sines.z * radius, 0.0f);
	Vector3 HeadRightBackward = HeadPos + Vector3(Cosines.w * radius, Sines.w * radius, 0.0f);

	Vector3 FeetLeftForward = FeetPos + Vector3(Cosines.x * radius, Sines.x * radius, 0.0f);
	Vector3 FeetLeftBackward = FeetPos + Vector3(Cosines.y * radius, Sines.y * radius, 0.0f);
	Vector3 FeetRightForward = FeetPos + Vector3(Cosines.z * radius, Sines.z * radius, 0.0f);
	Vector3 FeetRightBackward = FeetPos + Vector3(Cosines.w * radius, Sines.w * radius, 0.0f);


	if (W2S(HeadLeftForward, t[0]) && W2S(HeadLeftBackward, t[1]) && W2S(HeadRightBackward, t[2]) && W2S(HeadRightForward, t[3]))
	{
		if (W2S(FeetLeftForward, t[4]) && W2S(FeetLeftBackward, t[5]) && W2S(FeetRightBackward, t[6]) && W2S(FeetRightForward, t[7]))
		{
			for (int i = 0; i < 4; i++)
			{
				this->DrawLine(t[i].x, t[i].y, t[i + 4].x, t[i + 4].y, BoxColor);

				if (i != 3) this->DrawLine(t[i].x, t[i].y, t[i + 1].x, t[i + 1].y, BoxColor);
				else this->DrawLine(t[3].x, t[3].y, t[0].x, t[0].y, BoxColor);
			}

			for (int i = 4; i < 8; i++)
			{
				if (i != 7) this->DrawLine(t[i].x, t[i].y, t[i + 1].x, t[i + 1].y, BoxColor);
				else this->DrawLine(t[7].x, t[7].y, t[4].x, t[4].y, BoxColor);
			}
		}
	}
	return true;
}

bool D3D11Renderer::Draw3DBoxVehicle(Vector3 Position, Vector3 Rotation, Vector3 min, Vector3 max, Color BoxColor, bool CrosshairInOnly) {
	Vector3 corners[8]{
	  { max.x, max.y, max.z }, { max.x, min.y, max.z },
	  { min.x, max.y, max.z }, { min.x, min.y, max.z },
	  { min.x, min.y, min.z }, { max.x, min.y, min.z },
	  { min.x, max.y, min.z }, { max.x, max.y, min.z },
	};

	auto rot = acosf(Rotation.x) * 180.0f / DirectX::XM_PI;
	if (asinf(Rotation.y) * 180.0f / DirectX::XM_PI < 0.0f) rot *= -1.0f;

	Vector2 sc{ std::sin(XMConvertToRadians(rot)), std::cos(XMConvertToRadians(rot)) };

	for (auto& corner : corners) {
		corner = Position + Vector3{ corner.y * sc.y - corner.x * sc.x, corner.x * sc.y + corner.y * sc.x, corner.z };

		if (!W2S(corner, corner)) {
			return false;
		}
	}

	Vector3 pRoot = Position;
	Vector3 scpRoot;

	if (!W2S(pRoot, scpRoot)) {
		return false;
	}

	Vector2 Dot1 = scpRoot - Vector2(35.0f, 35.0f);
	Vector2 Dot2 = scpRoot + Vector2(35.0f, 35.0f);

	constexpr std::pair<int, int> indices[]{
		{ 0,1 },{ 2,3 },{ 1,3 },{ 0, 2 },{ 0,7 },{ 1,5 },{ 2, 6 },{ 3,4 },{ 4, 6 },{ 5,7 },{ 4,5 },{ 6,7 }
	};

	if (CrosshairInOnly) {
		return Tools->isMouseinRegion(Dot1.x, Dot1.y, Dot2.x, Dot2.y);
	}
	else {
		for (auto [id1, id2] : indices) {
			this->DrawLine(corners[id1].x, corners[id1].y, corners[id2].x, corners[id2].y, BoxColor);
		}
		return true;
	}
}