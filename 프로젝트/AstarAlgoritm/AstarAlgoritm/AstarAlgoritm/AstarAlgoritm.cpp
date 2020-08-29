#include "stdafx.h"
#include "AstarAlgoritm.h"
#include <stdio.h>
#include <iostream>
#include <stack>
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
    LoadStringW(hInstance, IDC_ASTARALGORITM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {	return FALSE;    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTARALGORITM));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {   TranslateMessage(&msg);
            DispatchMessage(&msg);      }
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTARALGORITM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ASTARALGORITM);
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
   {      return FALSE;   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}
// >> : A* Algorithm Start
#define INF INT_MAX-1
#define NODESIZE 64
#define MAPSIZE 14
enum Node_state : int
{
	Node_open = 0, Node_close, Node_start, Node_end, Node_block, Node_path, Node_calculated
};
class Node
{
public:
	POINT center;
	Node * closest_Node;
	int state;
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
vector<Node> oc;
Node Map[MAPSIZE][MAPSIZE];
void MapInit();
void DrawMap(HDC hdc);
void DrawNode(HDC hdc);
void DrawMon(HDC hdc);
bool PtInMap(const POINT & p);
bool PtInTileSet(const POINT & p);
bool PtInTowerSet(const POINT & p);
static POINT startPoint;
static POINT endPoint;
static vector<POINT> MonPath;
static Node MonSter;
bool AStarPathFinding(const POINT & startPoint, const POINT & endPoint);
void AStarCalculateNode(const POINT & startPoint, const POINT & endPoint, const POINT & curPoint);
void AStarPathMaking(const POINT & endPoint);
// >> : A* Algorithm End
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int time = 0;
    switch (message)
    {
	case WM_CREATE:
		MapInit();
		MonSter.center.x = Map[0][0].center.x;
		MonSter.center.y = Map[0][0].center.y;
		// 콘솔 출력
		AllocConsole();
		freopen("CONOUT$", "wt", stdout);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		DrawMap(hdc);
		DrawNode(hdc);
		DrawMon(hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		FreeConsole();
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	{	
		POINT TowerXY;
		TowerXY.x = LOWORD(lParam);
		TowerXY.y = HIWORD(lParam);
		if (PtInMap(TowerXY))
		{
			int i = (TowerXY.y - 32) / NODESIZE;
			int j = (TowerXY.x - 32) / NODESIZE;
			if(Map[i][j].state != Node_start &&Map[i][j].state != Node_end)
				Map[i][j].state = Node_block;
		}
		if (1408 < TowerXY.x&&TowerXY.x << 1536 && 800 < TowerXY.y&&TowerXY.y < 928)
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

	}
	InvalidateRgn(hWnd, NULL, TRUE);
	break;
	case WM_RBUTTONDOWN:
	{

	}
	InvalidateRgn(hWnd, NULL, TRUE);

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

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void MapInit()
{
	for (int i = 0; i < MAPSIZE; i++)
		for (int j = 0; j < MAPSIZE; j++)
		{
			Map[i][j].InitNode();
			Map[i][j].center.x = 32 + NODESIZE / 2 + NODESIZE*j;
			Map[i][j].center.y = 32 + NODESIZE / 2 + NODESIZE*i;
			Map[i][j].state = Node_open;
		}
	Map[0][0].state = Node_start;
	Map[0][0].G = 0;
	startPoint.x = Map[0][0].center.x / NODESIZE - 1;
	startPoint.y = Map[0][0].center.y / NODESIZE - 1;
	Map[MAPSIZE-1][MAPSIZE-1].state = Node_end;
	endPoint.x = Map[MAPSIZE - 1][MAPSIZE - 1].center.x / NODESIZE - 1;
	endPoint.y = Map[MAPSIZE - 1][MAPSIZE - 1].center.y / NODESIZE - 1;
	oc.clear();
	make_heap(oc.begin(), oc.end());
	oc.push_back(Map[0][0]);
}
void DrawMap(HDC hdc)
{
	static HBRUSH MapBrush = CreateSolidBrush(RGB(195, 195, 195));
	static HBRUSH StartButtonBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, StartButtonBrush);
	Rectangle(hdc, 1408, 800, 1408 + NODESIZE * 2, 800 + NODESIZE * 2);
	SelectObject(hdc, StartButtonBrush);
	oldBrush = (HBRUSH)SelectObject(hdc, MapBrush);
	Rectangle(hdc, 32, 32, NODESIZE*MAPSIZE, NODESIZE*MAPSIZE);
	SelectObject(hdc, oldBrush);
}
void DrawNode(HDC hdc)
{
	static HBRUSH StartBrush = CreateSolidBrush(RGB(255, 255, 255));
	static HBRUSH EndBrush = CreateSolidBrush(RGB(255, 0, 0));
	static HBRUSH TileBrush = CreateSolidBrush(RGB(128, 255, 128));
	static HBRUSH TowerBrush = CreateSolidBrush(RGB(0, 0, 255));
	static HBRUSH PathBrush = CreateSolidBrush(RGB(136, 0, 21));
	static HBRUSH CalBrush = CreateSolidBrush(RGB(200, 191, 231));
	static HBRUSH CloseBrush = CreateSolidBrush(RGB(255, 127, 127));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, StartBrush);
	for (int i = 0; i < MAPSIZE; i++)
	{
		for (int j = 0; j < MAPSIZE; j++)
		{
			switch (Map[i][j].state)
			{

			case Node_open:
				SelectObject(hdc, TileBrush);
				break;
			case Node_start:
				SelectObject(hdc, StartBrush);
				break;
			case Node_end:
				SelectObject(hdc, EndBrush);
				break;
			case Node_block:
				SelectObject(hdc, TowerBrush);
				break;
			//case Node_calculated:
			//	SelectObject(hdc, CalBrush);
			//	break;
			//case Node_close:
			//	SelectObject(hdc, CloseBrush);
			//	break;
			case Node_path:
				SelectObject(hdc, PathBrush);
				break;
			default:
				SelectObject(hdc, TileBrush);
				break;
			}
			const int left = Map[i][j].center.x - NODESIZE / 2;
			const int top = Map[i][j].center.y - NODESIZE / 2;
			Rectangle(hdc, left, top, left + NODESIZE, top + NODESIZE);
		}
	}
	SelectObject(hdc, oldBrush);
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
bool PtInMap(const POINT & p)
{
	if (32 < p.x && p.x < 32 + NODESIZE*MAPSIZE && 32 < p.y  && p.y < 32 + NODESIZE*MAPSIZE)
		return true;
	else return false;
}
bool PtInTileSet(const POINT & p);
bool PtInTowerSet(const POINT & p);
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
			Map[tmpPoint.y][tmpPoint.x].state == Node_block || 
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
			push_heap(oc.begin(),oc.end());
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