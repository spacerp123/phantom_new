#pragma once

class D3DMenu
{
public:
	// position and sizes
	float x = 175.0f, y = 230.0f;				// current position of the menu
	float width = 600.0f;
	float hight = 370.0f;

	// menu vars
	int noitems = 0;			// number of menu items
	bool visible = false;

	void AddItem(const char* txt, const char** opt, int maxval, int& var);
	void AddOnOff(const char* txt, int& var);
	void AddintRange(const char* txt, int& var, int min, int max, int Step);
	void AddfloatRange(const char* txt, float& var, float min, float max, float Step, const char* DisplayMode);
};

void MakeMenu();
static bool TeleportToWayPointNow = false;

#define MenuBack      Color(  5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 0.92f) 
#define MenuIsEnabled      Color(230 / 255.0f, 15 / 255.0f, 15 / 255.0f, 15 / 255.0f) 
#define OrangeRedX      Color(230 / 255.0f, 255 / 255.0f, 69 / 255.0f, 000) 