#include "stdafx.h"
#include "Object.h"
#include <math.h>
#include <string>
#include <vector>
using namespace std;
#define MAX_LOADSTRING 100

HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OBJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {	return FALSE;    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OBJECT));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {   if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {   TranslateMessage(&msg);
            DispatchMessage(&msg);     }
    }
    return (int) msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OBJECT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OBJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      32, 32, 1600, 996, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {	return FALSE;   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}
// >> : OBJECT START
#define TILENUM 6
#define TOWERNUM 12
class OBJECT
{
public:
	POINT center;
	int radius;
	int level;
	int type;
	BOOL Selection;
};
double LengthPts(int x1, int y1, int x2, int y2) { return (sqrt((float)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)))); }
BOOL InCircle(const OBJECT & ob, int mx, int my) {
	if (LengthPts(ob.center.x, ob.center.y, mx, my) < ob.radius) return TRUE;
	else return FALSE;
}

void Draw(HDC hdc, const OBJECT & ob);
void Move(OBJECT ob);
void Swap(OBJECT ob);
void Merge(OBJECT ob);
void Delete(OBJECT ob);
vector<OBJECT> Tile(TILENUM);
vector<OBJECT> Tower(TOWERNUM);
// >> : OBJECT END
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int mx, my;
    switch (message)
    {
	case WM_CREATE:
		for (int i = 0; i < TILENUM; i++)
		{
			Tile[i].center.x = 1024 + 96 * i;
			Tile[i].center.y = 128;
			Tile[i].radius = 32;
			Tile[i].level = 1;
			Tile[i].type = 0;
			Tile[i].Selection = FALSE;
		}
		for (int i = 0; i < TOWERNUM; i++)
		{
			Tower[i].center.x = 1024 - (int)(i / 6) * 580 + 96 * i;
			Tower[i].center.y = 256 + 96 * (int)(i / 6);
			Tower[i].radius = 32;
			Tower[i].level = 1;
			Tower[i].type = 1;
			Tower[i].Selection = FALSE;
		}
		InvalidateRgn(hWnd, NULL, TRUE);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:
	{
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		for (int i = 0; i < TILENUM; i++)
			if (InCircle(Tile[i], mx, my)) Tile[i].Selection = TRUE;
		for (int i = 0; i < TOWERNUM; i++)
			if (InCircle(Tower[i], mx, my)) Tower[i].Selection = TRUE;
		InvalidateRgn(hWnd, NULL, TRUE);
	}
		break;
	case WM_MOUSEMOVE:
	{
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		for (int i = 0; i<TILENUM; i++)
			if (Tile[i].Selection)
			{
				Tile[i].center.x = mx;
				Tile[i].center.y = my;
				InvalidateRgn(hWnd, NULL, TRUE);
			}
		for (int i = 0; i<TOWERNUM; i++)
			if (Tower[i].Selection)
			{
				Tower[i].center.x = mx;
				Tower[i].center.y = my;
				InvalidateRgn(hWnd, NULL, TRUE);
			}
	}
		break;
	case WM_LBUTTONUP:
		for (int i = 0; i < TILENUM; i++)
			Tile[i].Selection = FALSE;
		for (int i = 0; i < TOWERNUM; i++)
			Tower[i].Selection = FALSE;
		InvalidateRgn(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			for (int i = 0; i < TILENUM; i++)
			{
				Draw(hdc, Tile[i]);
				if(Tile[i].Selection) Ellipse(hdc, Tile[i].center.x - Tile[i].radius, Tile[i].center.y - Tile[i].radius, Tile[i].center.x + Tile[i].radius, Tile[i].center.y + Tile[i].radius);
			}
			for (int i = 0; i < TOWERNUM; i++)
				Draw(hdc, Tower[i]);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void Draw(HDC hdc, const OBJECT & ob)
{
	RECT rect;
	rect.left = ob.center.x - ob.radius;
	rect.top = ob.center.y - ob.radius;
	rect.right = ob.center.x + ob.radius;
	rect.bottom = ob.center.y + ob.radius;
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	DrawText(hdc, to_wstring(ob.level).c_str(), _tcslen(to_wstring(ob.level).c_str()), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	DrawText(hdc, to_wstring(ob.type).c_str(), _tcslen(to_wstring(ob.type).c_str()), &rect, DT_SINGLELINE | DT_BOTTOM | DT_RIGHT);
}