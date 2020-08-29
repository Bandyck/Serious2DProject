#include "stdafx.h"
#include "Problem.h"
#include <string>
// << : JSON
#define _CRT_SECURE_NO_WARNINGS    // fopen 보안 경고로 인한 컴파일 에러 방지
#include <stdio.h>				   // 파일 처리 함수가 선언된 헤더 파일
#include <stdlib.h>				   // malloc, free, atof 함수가 선언된 헤더 파일
#include <stdbool.h>			   // bool, true, false가 정의된 헤더 파일
#include <string.h>				   // strchr, strcmp, memset, memcpy 함수가 선언된 헤더 파일
// << : JSON
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
    LoadStringW(hInstance, IDC_PROBLEM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {	return FALSE;    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROBLEM));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {   if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROBLEM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROBLEM);
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
// >> : START
#define TILESET 6
#define TOWERSET 12
static int STAGE;
HBITMAP hProblemImage;
BITMAP bitProblem;
// >> : END
// >> : JSON
wstring s2ws(const std::string& s);
// 토큰 종류 열거형
typedef enum MY_TOKEN_TYPE {
	TOKEN_STRING = 0,    // 문자열 토큰
	TOKEN_NUMBER,    // 숫자 토큰
} MY_TOKEN_TYPE;
// 토큰 구조체
typedef struct _TOKEN {
	MY_TOKEN_TYPE type;   // 토큰 종류
	union {            // 두 종류 중 한 종류만 저장할 것이므로 공용체로 만듦
		char *string;     // 문자열 포인터
		double number;    // 실수형 숫자
	};
	bool isArray;      // 현재 토큰이 배열인지 표시
} TOKEN;
#define TOKEN_COUNT 400    // 토큰의 최대 개수
// JSON 구조체
typedef struct _JSON {
	TOKEN tokens[TOKEN_COUNT]; // 토큰 배열
} JSON;
typedef struct _Problem_Data {
	int Magic;
	string ProblemNo;
	int AnswerNumber;
	string Choice[4];
}Problem_Data;
char *doc;
JSON json;
char *readFile(char *filename, int *readSize);
void parseJSON(char *doc, int size, JSON *json);
void freeJSON(JSON *json);
Problem_Data SetProblemData(JSON *json, char *key, int numkey, Problem_Data & pd);
wstring SetProblemPath(const Problem_Data & pd);
// >> : JSON
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hBtTileSet[TILESET];
	HMENU hBtMenuTileSet[TILESET];
	HWND hBtTowerSet[TOWERSET];
	HMENU hBtMenuTowerSet[TOWERSET];
	HWND hBtChoice[4];
	HMENU hBtMenuChoice[4];
	int size;
    switch (message)
    {
	case WM_CREATE:
		STAGE = 1;
		for (int i = 0; i < TILESET; i++)
		{
			hBtMenuTileSet[i] = (HMENU)i;
			hBtTileSet[i] = CreateWindow(TEXT("BUTTON"), TEXT("잠금"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				992 + 96 * i, 96, 64, 64, hWnd, hBtMenuTileSet[i], hInst, NULL);
		}		
		for (int i = 0; i < TOWERSET; i++)
		{
			hBtMenuTowerSet[i] = (HMENU)(i + TILESET);
			hBtTowerSet[i] = CreateWindow(TEXT("BUTTON"), TEXT("잠금"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				992 - (int)(i / 6) * 580 + 96 * i, 224 + 96 * (int)(i / 6), 64, 64, hWnd, hBtMenuTowerSet[i], hInst, NULL);
		}
		for (int i = 0; i < 4; i++)
		{
			//string str = "객관식";
			//str += i + '0'+1;
			//wstring stemp = s2ws(str);
			//LPCWSTR result = stemp.c_str();
			hBtMenuChoice[i] = (HMENU)(i + 100);
			hBtChoice[i] = CreateWindow(TEXT("BUTTON"), NULL, WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON,
				992+136*i, 704, 136, 64, hWnd, hBtMenuChoice[i], hInst, NULL);
		}
		InvalidateRgn(hWnd, NULL, TRUE);
		doc = readFile("Problem.json", &size);
		parseJSON(doc, size, &json);
		break;
    case WM_COMMAND:
        {
		enum Magic
		{
			Geo1 = 0, Geo2, Geo3, Geo4, Geo5, Geo6, Ari11, Ari12, Ari21, Ari22, Ari31, Ari32, Ari41, Ari42, Ari51, Ari52, Ari61, Ari62
		};
		enum Choice
		{
			Cho1=100, Cho2, Cho3, Cho4
		};
		Problem_Data ProblemData;
		ProblemData.Magic = 0;
		ProblemData.ProblemNo = "";
		ProblemData.AnswerNumber = 0;
		for (int i = 0; i<4; i++)
			ProblemData.Choice[i] = "";
        int wmId = LOWORD(wParam);
		ProblemData = SetProblemData(&json, "ID", wmId, ProblemData);
        switch (wmId)
        {
		case Geo1:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Geo2:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Geo3:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Geo4:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Geo5:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
			break;
		case Geo6:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari11:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
			break;
		case Ari12:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari21:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari22:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari31:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari32:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari41:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari42:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari51:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari52:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari61:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Ari62:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
		break;
		case Cho1:
		{
			wstring result = SetProblemPath(ProblemData);
			LPCWSTR ProblemPath = result.c_str();
			hProblemImage = (HBITMAP)LoadImage(NULL, ProblemPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			GetObject(hProblemImage, sizeof(BITMAP), &bitProblem);
			InvalidateRgn(hWnd, NULL, TRUE);
		}
			break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			HDC hMenDC;
			HBITMAP hOldBitmap;
			int bx, by;
            PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			hMenDC = CreateCompatibleDC(hdc);
			hOldBitmap = (HBITMAP)SelectObject(hMenDC, hProblemImage);
			bx = bitProblem.bmWidth;
			by = bitProblem.bmHeight;
			Rectangle(hdc, 992, 416, 992 + 544, 416 + 288);
			BitBlt(hdc, 993, 417, bx, by, hMenDC, 0, 0, SRCCOPY);
			SelectObject(hMenDC, hOldBitmap);
			DeleteDC(hMenDC);
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
wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring r(buf);
	delete[] buf;
	return r;
}
char *readFile(char *filename, int *readSize)    // 파일을 읽어서 내용을 반환하는 함수
{
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
		return NULL;

	int size;
	char *buffer;

	// 파일 크기 구하기
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// 파일 크기 + NULL 공간만큼 메모리를 할당하고 0으로 초기화
	buffer = (char *)malloc(size + 1);
	memset(buffer, 0, size + 1);

	// 파일 내용 읽기
	if (fread(buffer, size, 1, fp) < 1)
	{
		*readSize = 0;
		free(buffer);
		fclose(fp);
		return NULL;
	}

	// 파일 크기를 넘겨줌
	*readSize = size;
	// 파일 포인터 닫기
	fclose(fp);    

	return buffer;
}
void parseJSON(char *doc, int size, JSON *json)    // JSON 파싱 함수
{
	int tokenIndex = 0;    // 토큰 인덱스
	int pos = 0;           // 문자 검색 위치를 저장하는 변수

	if (doc[pos] != '[')   // 문서의 시작이 [ 인지 검사
		return;

	pos++;    // 다음 문자로

	if (doc[pos] != '{')   // 다음 문자가 { 인지 검사
		return;

	while (pos < size)       // 문서 크기만큼 반복
	{
		switch (doc[pos])    // 문자의 종류에 따라 분기
		{
		case '"':            // 문자가 "이면 문자열
		{
			// 문자열의 시작 위치를 구함. 맨 앞의 "를 제외하기 위해 + 1
			char *begin = doc + pos + 1;

			// 문자열의 끝 위치를 구함. 다음 "의 위치
			char *end = strchr(begin, '"');
			if (end == NULL)    // "가 없으면 잘못된 문법이므로 
				break;          // 반복을 종료

			int stringLength = end - begin;    // 문자열의 실제 길이는 끝 위치 - 시작 위치

											   // 토큰 배열에 문자열 저장, 토큰 종류는 문자열
			json->tokens[tokenIndex].type = (MY_TOKEN_TYPE)TOKEN_STRING;
			// 문자열 길이 + NULL 공간만큼 메모리 할당
			json->tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
			// 할당한 메모리를 0으로 초기화
			memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

			// 문서에서 문자열을 토큰에 저장
			// 문자열 시작 위치에서 문자열 길이만큼만 복사
			memcpy(json->tokens[tokenIndex].string, begin, stringLength);

			tokenIndex++; // 토큰 인덱스 증가

			pos = pos + stringLength + 1;    // 현재 위치 + 문자열 길이 + "(+ 1)
		}
		break;

		case '0': case '1': case '2': case '3': case '4': case '5':    // 문자가 숫자이면
		case '6': case '7': case '8': case '9': case '-':              // -는 음수일 때
		{
			// 문자열의 시작 위치를 구함
			char *begin = doc + pos;
			char *end;
			char *buffer;

			// 문자열의 끝 위치를 구함. ,가 나오거나
			end = strchr(doc + pos, ',');
			if (end == NULL)
			{
				// } 가 나오면 문자열이 끝남
				end = strchr(doc + pos, '}');
				if (end == NULL)    // }가 없으면 잘못된 문법이므로
					break;          // 반복을 종료
			}

			int stringLength = end - begin;    // 문자열의 실제 길이는 끝 위치 - 시작 위치

											   // 문자열 길이 + NULL 공간만큼 메모리 할당
			buffer = (char*)malloc(stringLength + 1);
			// 할당한 메모리를 0으로 초기화
			memset(buffer, 0, stringLength + 1);

			// 문서에서 문자열을 버퍼에 저장
			// 문자열 시작 위치에서 문자열 길이만큼만 복사
			memcpy(buffer, begin, stringLength);

			// 토큰 종류는 숫자
			json->tokens[tokenIndex].type = TOKEN_NUMBER;
			// 문자열을 숫자로 변환하여 토큰에 저장
			json->tokens[tokenIndex].number = atof(buffer);

			free(buffer);    // 버퍼 해제

			tokenIndex++;    // 토큰 인덱스 증가

			pos = pos + stringLength + 1;    // 현재 위치 + 문자열 길이 + , 또는 }(+ 1)
		}
		break;
		}
		pos++; // 다음 문자로
	}
}
void freeJSON(JSON *json)    // JSON 해제 함수
{
	for (int i = 0; i < TOKEN_COUNT; i++)            // 토큰 개수만큼 반복
	{
		if (json->tokens[i].type == TOKEN_STRING)    // 토큰 종류가 문자열이면
			free(json->tokens[i].string);            // 동적 메모리 해제
	}
}
Problem_Data SetProblemData(JSON *json, char *key, int numkey, Problem_Data & pd)    // 키에 해당하는 숫자를 가져오는 함수
{
	for (int i = 0; i < TOKEN_COUNT; i++)    // 토큰 개수만큼 반복
	{
		// 토큰 종류가 숫자이면서 토큰의 문자열이 키와 일치한다면
		if (json->tokens[i].type == TOKEN_STRING &&
			strcmp(json->tokens[i].string, key) == 0)
		{
			// 바로 뒤의 토큰(i + 1)이 숫자이면
			if (json->tokens[i + 1].type == TOKEN_NUMBER && (int)json->tokens[i + 1].number == numkey)
			{
				pd.Magic = (int)json->tokens[i + 3].number;
				pd.ProblemNo = json->tokens[i + 5].string;
				pd.AnswerNumber = (int)json->tokens[i + 7].number;
				pd.Choice[0] = json->tokens[i + 9].string;
				pd.Choice[1] = json->tokens[i + 11].string;
				pd.Choice[2] = json->tokens[i + 13].string;
				pd.Choice[3] = json->tokens[i + 15].string;
				return pd;
			}
		}
	}
	return pd;    // 키를 찾지 못했으면 0.0을 반환
}
wstring SetProblemPath(const Problem_Data & pd)
{
	string ImagePath;
	ImagePath = "images/";
	if (pd.Magic == 1)
		ImagePath += "Geo/";
	else if(pd.Magic == 2)
		ImagePath += "Ari/";
	ImagePath += pd.ProblemNo;
	ImagePath += ".bmp";
	wstring stemp = s2ws(ImagePath);
	return stemp;
}