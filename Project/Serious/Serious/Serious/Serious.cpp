#include "stdafx.h"
#include "Serious.h"
#include <vector>
#include <iostream>
#include <CommCtrl.h>
using namespace std;
#define MAX_LOADSTRING 100

HINSTANCE hInst;                               
WCHAR szTitle[MAX_LOADSTRING];                 
WCHAR szWindowClass[MAX_LOADSTRING];           

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERIOUS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {	return FALSE;    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERIOUS));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {   TranslateMessage(&msg);
            DispatchMessage(&msg);	}
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERIOUS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERIOUS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		32, 32, 1600, 996, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{	return FALSE;	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}
int STAGE;
// >> SET start :
#define TILENUM  6
#define TOWERNUM 12
enum Set_state : int
{
	Set_open = 0, Set_lock, Set_filled
};
class SET
{
public:
	POINT center;
	int radius;
	BOOL Selection;
	int state;
	HWND hBt;
	HMENU hBtMenu;
	HIMAGELIST hIL;
};
SET TileSet[TILENUM];
SET TowerSet[TOWERNUM];
void SetInit(HWND hWnd);
void RenderSet(HDC hdc);
// >> SET end :
// >> NODE start :
#define MAPSIZE 14
#define NODESIZE 64
#define INF INT_MAX-1
enum Node_stat : int
{
	Node_start = 0, Node_end, Node_ground, Node_tile, Node_fog, Node_tower, Node_open, Node_close, Node_calculated, Node_path
};
class Node
{
public:
	POINT center;
	int state;
	HIMAGELIST hIL;
};
Node Map[MAPSIZE][MAPSIZE];
void NodeInit();
void RenderNode(HDC hdc);
// >> NODE end :
void StartInit(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
	case WM_CREATE:
		STAGE = 1;
		SetInit(hWnd);
		NodeInit();
		StartInit(hWnd);
		// 콘솔
		//AllocConsole();
		//freopen("CONOUT$", "wt", stdout);
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RenderSet(hdc);
			RenderNode(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		// 콘솔
		//FreeConsole();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void SetInit(HWND hWnd)
{
	int i;
	for (i = 0; i < TILENUM; i++)
	{
		TileSet[i].center.x = 1024 + 96 * i;
		TileSet[i].center.y = 128;
		TileSet[i].radius = 32;
		TileSet[i].state = Set_lock;
		TileSet[i].hBtMenu = (HMENU)i;
		TileSet[i].hBt = CreateWindow(TEXT("BUTTON"), NULL, WS_CHILD,
			(TileSet[i].center.x - TileSet[i].radius), 
			(TileSet[i].center.y - TileSet[i].radius), 
			TileSet[i].radius * 2, TileSet[i].radius * 2, 
			hWnd, TileSet[i].hBtMenu, hInst, NULL);
		TileSet[i].hIL = ImageList_LoadImage(NULL, _T("Resource/TileSet.bmp"), 64, 1, CLR_NONE, IMAGE_BITMAP, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	}
	for (i = 0; i < TOWERNUM; i++)
	{
		TowerSet[i].center.x = 1024 + 96 * i - (int)(i / 6) * 576;
		TowerSet[i].center.y = 256 + 96 * (int)(i / 6);
		TowerSet[i].radius = 32;
		TowerSet[i].state = Set_lock;
		TowerSet[i].hBtMenu = (HMENU)(i + TILENUM);
		TowerSet[i].hBt = CreateWindow(TEXT("BUTTON"), NULL, WS_CHILD,
			(TowerSet[i].center.x - TowerSet[i].radius),
			(TowerSet[i].center.y - TowerSet[i].radius),
			TowerSet[i].radius * 2, TowerSet[i].radius * 2,
			hWnd, TowerSet[i].hBtMenu, hInst, NULL);
		TowerSet[i].hIL = ImageList_LoadImage(NULL, _T("Resource/TowerSet.bmp"), 64, 1, CLR_NONE, IMAGE_BITMAP, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	}
	for (i = 0; i <= (STAGE - 1) / 2; i++)
		TileSet[i].state = Set_open;
	for (i = 0; i < STAGE; i++)
		TowerSet[i].state = Set_open;
}
void RenderSet(HDC hdc)
{
	for (int i = 0; i < TILENUM; i++)
		ImageList_Draw(TileSet[i].hIL, TileSet[i].state, hdc, (TileSet[i].center.x - TileSet[i].radius), (TileSet[i].center.y - TileSet[i].radius), ILD_TRANSPARENT);
	for (int i = 0; i < TOWERNUM; i++)
		ImageList_Draw(TowerSet[i].hIL, TowerSet[i].state, hdc, (TowerSet[i].center.x - TowerSet[i].radius), (TowerSet[i].center.y - TowerSet[i].radius), ILD_TRANSPARENT);
}
void NodeInit()
{
	int i, j;
	for (i = 0; i < MAPSIZE; i++)
		for (j = 0; j < MAPSIZE; j++)
		{
			Map[i][j].center.x = 32 + NODESIZE / 2 + NODESIZE*j;
			Map[i][j].center.y = 32 + NODESIZE / 2 + NODESIZE*i;
			Map[i][j].state = Node_fog;
			Map[i][j].hIL = ImageList_LoadImage(NULL, _T("Resource/MapNode.bmp"), 64, 1, CLR_NONE, IMAGE_BITMAP, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		}
	for (i = 12 - STAGE; i < MAPSIZE; i++)
		for (j = 12 - STAGE; j < MAPSIZE; j++)
			Map[i][j].state = Node_ground;
	Map[12 - STAGE][12 - STAGE].state = Node_start;
	Map[MAPSIZE - 2][MAPSIZE - 2].state = Node_tile;
	Map[MAPSIZE - 1][MAPSIZE - 1].state = Node_end;
}
void RenderNode(HDC hdc)
{
	for (int i = 0; i < MAPSIZE; i++)
		for (int j = 0; j < MAPSIZE; j++)
			ImageList_Draw(Map[i][j].hIL, Map[i][j].state, hdc, (Map[i][j].center.x - NODESIZE / 2), (Map[i][j].center.y - NODESIZE / 2), ILD_TRANSPARENT);
}

void StartInit(HWND hWnd)
{
	HWND StartButton;
	HMENU StartButtonMenu;
	StartButtonMenu = (HMENU)100;
	StartButton = CreateWindow(TEXT("BUTTON"), TEXT("START"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		1408, 800, 128, 128, hWnd, StartButtonMenu, hInst, NULL);
}
