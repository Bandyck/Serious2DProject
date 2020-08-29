#include "stdafx.h"
#include "UI.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
// >> : UI Start
#define LINETOLINESIZE 64
#define LINENUMBER 14
// >> : UI End
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_UI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {        return FALSE;    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UI));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {   TranslateMessage(&msg);
            DispatchMessage(&msg);        }
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_UI);
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			for (int i = 0; i <= LINENUMBER; i++)
			{	// 바둑판 격자
				MoveToEx(hdc, 32, 32 + LINETOLINESIZE * i, NULL);
				LineTo(hdc, 32 + LINETOLINESIZE * LINENUMBER, 32 + LINETOLINESIZE * i);
				MoveToEx(hdc, 32 + LINETOLINESIZE * i, 32, NULL);
				LineTo(hdc, 32 + LINETOLINESIZE * i, 32 + LINETOLINESIZE * LINENUMBER);
			}
			{
				RECT rect;
				rect.left = 32;
				rect.top = 32;
				rect.right = 32+64;
				rect.bottom = 32+64;
				DrawText(hdc, _T("출발"), _tcslen(_T("출발")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			{
				RECT rect;
				rect.left = 928 - 64;
				rect.top = 928 - 64;
				rect.right = 928;
				rect.bottom = 928;
				DrawText(hdc, _T("도착"), _tcslen(_T("도착")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			{
				RECT rect;
				rect.left = 928;
				rect.top = 32;
				rect.right = 1536+64;
				rect.bottom = 32 + 64;
				DrawText(hdc, _T("타일"), _tcslen(_T("타일")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			for (int i = 0; i < 6; ++i)
			{
				Rectangle(hdc, 992 + 96 * i, 96, 1056 + 96 * i, 160);
				RECT rect;
				rect.left = 992 + 96 * i;
				rect.top = 96;
				rect.right = 1056 + 96 * i;
				rect.bottom = 160;
				DrawText(hdc, _T("타일"), _tcslen(_T("타일")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			{
				RECT rect;
				rect.left = 928;
				rect.top = 32+64+64;
				rect.right = 1536+64;
				rect.bottom = 32 + 64+64+64;
				DrawText(hdc, _T("타워"), _tcslen(_T("타워")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			for (int i = 0; i < 6; ++i)
			{
				Rectangle(hdc, 992 + 96 * i, 224, 1056 + 96 * i, 288);
				RECT rect;
				rect.left = 992 + 96 * i;
				rect.top = 224;
				rect.right = 1056 + 96 * i;
				rect.bottom = 288;
				DrawText(hdc, _T("타워"), _tcslen(_T("타워")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			for (int i = 0; i < 6; ++i)
			{
				Rectangle(hdc, 992 + 96 * i, 324, 1056 + 96 * i, 388);
				RECT rect;
				rect.left = 992 + 96 * i;
				rect.top = 324;
				rect.right = 1056 + 96 * i;
				rect.bottom = 388;
				DrawText(hdc, _T("타워"), _tcslen(_T("타워")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			{
				Rectangle(hdc, 992, 416, 1536, 700 + 64);
				RECT rect;
				rect.left = 992;
				rect.top = 416;
				rect.right = 1536;
				rect.bottom = 700 + 64;
				DrawText(hdc, _T("문제 풀이"), _tcslen(_T("문제 풀이")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			{
				Rectangle(hdc, 992, 736+64, 1408 - 36, 864);
				RECT rect;
				rect.left = 992;
				rect.top = 736+64;
				rect.right = 1408 - 36;
				rect.bottom = 864;
				DrawText(hdc, _T("스테이지 1"), _tcslen(_T("스테이지 1")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			{
				Rectangle(hdc, 992, 864, 1056, 928);
				RECT rect;
				rect.left = 992;
				rect.top = 864;
				rect.right = 1056;
				rect.bottom = 928;
				DrawText(hdc, _T("\n몬스터\n이미지"), _tcslen(_T("\n몬스터\n이미지")), &rect, DT_VCENTER | DT_CENTER);
			}
			{
				Rectangle(hdc, 1056, 864, 1120, 928);
				RECT rect;
				rect.left = 1056;
				rect.top = 864;
				rect.right = 1120;
				rect.bottom = 928;
				DrawText(hdc, _T("×10"), _tcslen(_T("×10")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
			{
				Rectangle(hdc, 1120, 864, 1408 - 36, 864 + 32);
				RECT rect;
				rect.left = 1120+10;
				rect.top = 864;
				rect.right = 1408 - 36;
				rect.bottom = rect.top + 32;
				DrawText(hdc, _T("이름 : "), _tcslen(_T("이름 :")), &rect, DT_SINGLELINE | DT_VCENTER);
			}
			{
				Rectangle(hdc, 1120, 864+32, 1408 - 36, 864 + 32+32);
				RECT rect;
				rect.left = 1120+10;
				rect.top = 864 +32;
				rect.right = 1408 - 36;
				rect.bottom = rect.top + 32;
				DrawText(hdc, _T("HP : "), _tcslen(_T("HP :")), &rect, DT_SINGLELINE | DT_VCENTER);
			}
			{
				Rectangle(hdc, 1408, 736+64, 1536, 928);
				RECT rect;
				rect.left = 1408;
				rect.top = 736+64;
				rect.right = 1536;
				rect.bottom = 928;
				DrawText(hdc, _T("START"), _tcslen(_T("START")), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
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