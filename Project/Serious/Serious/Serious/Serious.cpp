#include "stdafx.h"
#include "Serious.h"
#include <vector>
#include <iostream>
#include <CommCtrl.h>
#include <stack>
#include <algorithm>
#include <queue>
#include <functional>
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
	Node_start = 0, Node_end, Node_open, Node_close, Node_calculated, Node_ground, Node_tile, Node_fog, Node_path, Node_tower
};
class Node
{
public:
	POINT center;
	int state;
	HIMAGELIST hIL;
	Node * closest_Node;
	int G;
	int H;
	int F;
	Node() { InitNode(); }
	void InitNode()
	{
		G = INF;
		H = INF;
		F = INF;
		closest_Node = NULL;
		state = Node_open;
	}
};
bool operator<(const Node & node1, const Node & node2)
{
	if (node1.F == node2.F)
		return node1.G < node2.G;
	return node1.F > node2.F;
}
inline bool operator==(const POINT & p1, const POINT & p2)
{
	if (p1.x == p2.x && p1.y == p2.y)
		return true;
	return false;
}
vector<Node> oc;
Node Map[MAPSIZE][MAPSIZE];
void NodeInit();
void RenderNode(HDC hdc);
static POINT startPoint;
static POINT endPoint;
void DrawMon(HDC hdc);
static vector<POINT> MonPath;
static Node MonSter;
bool AStarPathFinding(const POINT & startPoint, const POINT & endPoint);
void AStarCalculateNode(const POINT & startPoint, const POINT & endPoint, const POINT & curPoint);
void AStarPathMaking(const POINT & endPoint);
// >> NODE end :
void StartInit(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int time = 0;
    switch (message)
    {
	case WM_CREATE:
		STAGE = 12;
		SetInit(hWnd);
		NodeInit();
		StartInit(hWnd);
		MonSter.center.x = Map[0][0].center.x;
		MonSter.center.y = Map[0][0].center.y;
		// 콘솔
		//AllocConsole();
		//freopen("CONOUT$", "wt", stdout);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
				enum Start
				{
					start = 100
				};
			case start:
			{
				while (1)
				{
					if (!oc.empty())
						AStarPathFinding(startPoint, endPoint);
					else
					{
						if (Map[MAPSIZE - 1][MAPSIZE - 1].closest_Node != NULL)
						{
							AStarPathMaking(endPoint);
							break;
						}
					}
				}
				reverse(MonPath.begin(), MonPath.end());
				MonPath.insert(MonPath.begin(), Map[0][0].center);
				MonPath.push_back(Map[MAPSIZE - 1][MAPSIZE - 1].center);
				for (int i = 0; i < MonPath.size(); i++)
				{
					cout << MonPath[i].x << ' ' << MonPath[i].y << endl;
				}
				SetTimer(hWnd, 1, 100, NULL);
			}
			InvalidateRgn(hWnd, NULL, TRUE);
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_TIMER:
	{
		if (MonPath[time / 64 + 1].y == MonSter.center.y)
		{
			if (MonPath[time / 64 + 1].x > MonSter.center.x)
				MonSter.center.x += 16;
			else MonSter.center.x -= 16;
		}
		else if (MonPath[time / 64 + 1].x == MonSter.center.x)
		{
			if (MonPath[time / 64 + 1].y > MonSter.center.y)
				MonSter.center.y += 16;
			else MonSter.center.y -= 16;
		}
		time += 16;
		InvalidateRgn(hWnd, NULL, TRUE);
	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RenderSet(hdc);
			RenderNode(hdc);
			DrawMon(hdc);
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
			Map[i][j].hIL = ImageList_LoadImage(NULL, _T("Resource/MapNodeTmp.bmp"), 64, 1, CLR_NONE, IMAGE_BITMAP, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		}
	for (i = 12 - STAGE; i < MAPSIZE; i++)
		for (j = 12 - STAGE; j < MAPSIZE; j++)
			Map[i][j].state = Node_ground;
	Map[12 - STAGE][12 - STAGE].state = Node_start;
	Map[12 - STAGE][12 - STAGE].G = 0;
	startPoint.x = Map[12 - STAGE][12 - STAGE].center.x / NODESIZE - 1;
	startPoint.y = Map[12 - STAGE][12 - STAGE].center.y / NODESIZE - 1;
	Map[MAPSIZE - 1][MAPSIZE - 1].state = Node_end;
	endPoint.x = Map[MAPSIZE - 1][MAPSIZE - 1].center.x / NODESIZE - 1;
	endPoint.y = Map[MAPSIZE - 1][MAPSIZE - 1].center.y / NODESIZE - 1;
	Map[0][MAPSIZE - 1].state = Node_tile;
	Map[1][MAPSIZE - 2].state = Node_tile;
	Map[2][MAPSIZE - 3].state = Node_tile;


	oc.clear();
	make_heap(oc.begin(), oc.end());
	oc.push_back(Map[12 - STAGE][12 - STAGE]);
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
bool AStarPathFinding(const POINT & startPoint, const POINT & endPoint)
{
	static POINT curPoint;
	pop_heap(oc.begin(), oc.end());
	curPoint.x = oc.back().center.x / NODESIZE - 1;
	curPoint.y = oc.back().center.y / NODESIZE - 1;
	oc.pop_back();
	AStarCalculateNode(startPoint, endPoint, curPoint);
	if (Map[curPoint.y][curPoint.x].state != Node_start)
		Map[curPoint.y][curPoint.x].state = Node_close;
	return true;
}
void AStarCalculateNode(const POINT & startPoint, const POINT & endPoint, const POINT & curPoint)
{
	POINT tmpPoint;

	for (int i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0:
			tmpPoint.x = curPoint.x - 1;
			tmpPoint.y = curPoint.y;
			break;
		case 1:
			tmpPoint.x = curPoint.x;
			tmpPoint.y = curPoint.y - 1;
			break;
		case 2:
			tmpPoint.x = curPoint.x + 1;
			tmpPoint.y = curPoint.y;
			break;
		case 3:
			tmpPoint.x = curPoint.x;
			tmpPoint.y = curPoint.y + 1;
			break;
		}
		if (tmpPoint == endPoint)
		{
			Map[tmpPoint.y][tmpPoint.x].closest_Node = &Map[curPoint.y][curPoint.x];
			Map[tmpPoint.y][tmpPoint.x].state = Node_end;
			oc.clear();
			break;
		}
		if (tmpPoint.x < 0 || tmpPoint.x == MAPSIZE || tmpPoint.y < 0 || tmpPoint.y == MAPSIZE ||
			Map[tmpPoint.y][tmpPoint.x].state == Node_close ||
			Map[tmpPoint.y][tmpPoint.x].state == Node_tile ||
			Map[tmpPoint.y][tmpPoint.x].state == Node_fog ||
			Map[tmpPoint.y][tmpPoint.x].state == Node_tower ||
			Map[tmpPoint.y][tmpPoint.x].state == Node_start)
			continue;
		int Gval = Map[tmpPoint.y][tmpPoint.x].G + 1;
		int Hval = (endPoint.y - curPoint.y) + (endPoint.x - curPoint.x);

		if (Map[tmpPoint.y][tmpPoint.x].G > Gval)
			Map[tmpPoint.y][tmpPoint.x].G = Gval;
		else
			Gval = Map[tmpPoint.y][tmpPoint.x].G;

		if (Map[tmpPoint.y][tmpPoint.x].H > Hval)
			Map[tmpPoint.y][tmpPoint.x].H = Hval;
		else
			Hval = Map[tmpPoint.y][tmpPoint.x].H;

		if (Map[tmpPoint.y][tmpPoint.x].F > Gval + Hval)
		{
			Map[tmpPoint.y][tmpPoint.x].F = Gval + Hval;
			Map[tmpPoint.y][tmpPoint.x].closest_Node = &Map[curPoint.y][curPoint.x];
		}

		if (Map[tmpPoint.y][tmpPoint.x].state != Node_calculated)
		{
			Map[tmpPoint.y][tmpPoint.x].state = Node_calculated;
			oc.push_back(Map[tmpPoint.y][tmpPoint.x]);
			push_heap(oc.begin(), oc.end());
		}
	}
}
void AStarPathMaking(const POINT & endPoint)
{
	Node * cur_Node = Map[endPoint.y][endPoint.x].closest_Node;
	while (cur_Node->state != Node_start)
	{
		MonPath.push_back(cur_Node->center);
		cur_Node->state = Node_path;
		cur_Node = cur_Node->closest_Node;
	}
}
void DrawMon(HDC hdc)
{
	static HBRUSH MonBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, MonBrush);
	const int left = MonSter.center.x - NODESIZE / 2;
	const int top = MonSter.center.y - NODESIZE / 2;
	Rectangle(hdc, left, top, left + NODESIZE, top + NODESIZE);
	SelectObject(hdc, oldBrush);
}