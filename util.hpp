#pragma once
//external dependencies
#include <windows.h>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <TlHelp32.h>
#include <dwmapi.h>
#include <xmmintrin.h>
#include <array>
#include <vector>
#include <cstdlib>
#include <random>
#include <direct.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <d3d11.h>
#include <d3d9.h>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3d11.lib")

//internal dependencies

#include "cheat/driver/driver.h"
#include "cheat/sdk.hpp"
#include "cheat/actors/cache.h"
#include "cheat/structs.hpp"
#include "cheat/mouse.hpp"

//#include "cheat/actors/esp.hpp"


#include "render/render.h"


inline int screen_width = GetSystemMetrics(SM_CXSCREEN);
inline int screen_height = GetSystemMetrics(SM_CYSCREEN);
inline INT32 find_process(LPCTSTR process_name) {
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) {
		do {
			if (!lstrcmpi(pt.szExeFile, process_name)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);


	return { NULL };
}
inline void DrawRect(int x, int y, int w, int h, int t, ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0, t);
}

inline double GetCrossDistance(double x1, double y1, double x2, double y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}
inline int get_fps()
{
	using namespace std::chrono;
	static int count = 0;
	static auto last = high_resolution_clock::now();
	auto now = high_resolution_clock::now();
	static int fps = 0;

	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000) {
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}
inline void DrawFilledRect(int x, int y, int w, int h, ImU32 color, bool round = false)
{

	if (round)
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, ImDrawCornerFlags_All, 30);
	else
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

inline void DrawCornerBox(int x, int y, int w, int h, int borderPx, ImU32 color)
{
	int borderWidth = (borderPx < (w / 2) && borderPx < (h / 2)) ? borderPx : ((w / 2) < (h / 2) ? (w / 2) : (h / 2));


	DrawFilledRect(x + borderWidth, y, w / 3, borderWidth, color); //top 
	DrawFilledRect(x + w - w / 3, y, w / 3, borderWidth, color); //top 
	DrawFilledRect(x, y, borderWidth, h / 3, color); //left 
	DrawFilledRect(x, y + h - h / 3, borderWidth, h / 3, color); //left 
	DrawFilledRect(x + borderWidth, y + h - borderWidth, w / 3, borderWidth, color); //bottom 
	DrawFilledRect(x + w - w / 3, y + h - borderWidth, w / 3, borderWidth, color); //bottom 
	DrawFilledRect(x + w - borderWidth, y, borderWidth, h / 3, color);//right 
	DrawFilledRect(x + w - borderWidth, y + h - h / 3, borderWidth, h / 3, color);//right 
}


class CKey {
private:
	BYTE bPrevState[0x100];
public:
	CKey() {
		memset(bPrevState, 0, sizeof(bPrevState));
	}

	BOOL IsKeyPushing(BYTE vKey) {
		return GetAsyncKeyState(vKey) & 0x8000;
	}

	BOOL IsKeyPushed(BYTE vKey) {
		BOOL bReturn = FALSE;

		if (IsKeyPushing(vKey))
			bPrevState[vKey] = TRUE;
		else
		{
			if (bPrevState[vKey] == TRUE)
				bReturn = TRUE;
			bPrevState[vKey] = FALSE;
		}

		return bReturn;
	}
};
inline CKey Key;
inline void debug_ptr(uintptr_t p, const char* s) {
	if (config.debug)
		printf("%s %I64u \n", s, p);
}
inline void debug_3d(vector3 three, const char* s) {
	if (config.debug)
		printf("%s %f, %f, %f \n", s, three.x, three.y, three.z);
}
inline void debug_2d(vector2 two, const char* s) {
	if (config.debug)
		printf("%s %f, %f \n", s, two.x, two.y);
}
inline void debug_1d(double one, const char* s) {
	if (config.debug)
		printf("%s %f \n", s, one);
}
inline void debug_string(std::string s) {
	if (config.debug)
		printf("%s \n", s.c_str());
}
inline void print_3d(vector3 three, const char* s) {
	
	printf("%s %f, %f, %f \n", s, three.x, three.y, three.z);
}
inline void print_ptr(uintptr_t p, const char* s) {
	printf("%s %I64u \n", s, p);
}
inline void print_string(std::string s) {
	printf("%s \n", s.c_str());
}