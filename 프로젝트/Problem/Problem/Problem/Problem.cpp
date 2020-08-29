#include "stdafx.h"
#include "Problem.h"
#include <string>
// << : JSON
#define _CRT_SECURE_NO_WARNINGS    // fopen ���� ���� ���� ������ ���� ����
#include <stdio.h>				   // ���� ó�� �Լ��� ����� ��� ����
#include <stdlib.h>				   // malloc, free, atof �Լ��� ����� ��� ����
#include <stdbool.h>			   // bool, true, false�� ���ǵ� ��� ����
#include <string.h>				   // strchr, strcmp, memset, memcpy �Լ��� ����� ��� ����
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
// ��ū ���� ������
typedef enum MY_TOKEN_TYPE {
	TOKEN_STRING = 0,    // ���ڿ� ��ū
	TOKEN_NUMBER,    // ���� ��ū
} MY_TOKEN_TYPE;
// ��ū ����ü
typedef struct _TOKEN {
	MY_TOKEN_TYPE type;   // ��ū ����
	union {            // �� ���� �� �� ������ ������ ���̹Ƿ� ����ü�� ����
		char *string;     // ���ڿ� ������
		double number;    // �Ǽ��� ����
	};
	bool isArray;      // ���� ��ū�� �迭���� ǥ��
} TOKEN;
#define TOKEN_COUNT 400    // ��ū�� �ִ� ����
// JSON ����ü
typedef struct _JSON {
	TOKEN tokens[TOKEN_COUNT]; // ��ū �迭
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
			hBtTileSet[i] = CreateWindow(TEXT("BUTTON"), TEXT("���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				992 + 96 * i, 96, 64, 64, hWnd, hBtMenuTileSet[i], hInst, NULL);
		}		
		for (int i = 0; i < TOWERSET; i++)
		{
			hBtMenuTowerSet[i] = (HMENU)(i + TILESET);
			hBtTowerSet[i] = CreateWindow(TEXT("BUTTON"), TEXT("���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				992 - (int)(i / 6) * 580 + 96 * i, 224 + 96 * (int)(i / 6), 64, 64, hWnd, hBtMenuTowerSet[i], hInst, NULL);
		}
		for (int i = 0; i < 4; i++)
		{
			//string str = "������";
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
char *readFile(char *filename, int *readSize)    // ������ �о ������ ��ȯ�ϴ� �Լ�
{
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
		return NULL;

	int size;
	char *buffer;

	// ���� ũ�� ���ϱ�
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ���� ũ�� + NULL ������ŭ �޸𸮸� �Ҵ��ϰ� 0���� �ʱ�ȭ
	buffer = (char *)malloc(size + 1);
	memset(buffer, 0, size + 1);

	// ���� ���� �б�
	if (fread(buffer, size, 1, fp) < 1)
	{
		*readSize = 0;
		free(buffer);
		fclose(fp);
		return NULL;
	}

	// ���� ũ�⸦ �Ѱ���
	*readSize = size;
	// ���� ������ �ݱ�
	fclose(fp);    

	return buffer;
}
void parseJSON(char *doc, int size, JSON *json)    // JSON �Ľ� �Լ�
{
	int tokenIndex = 0;    // ��ū �ε���
	int pos = 0;           // ���� �˻� ��ġ�� �����ϴ� ����

	if (doc[pos] != '[')   // ������ ������ [ ���� �˻�
		return;

	pos++;    // ���� ���ڷ�

	if (doc[pos] != '{')   // ���� ���ڰ� { ���� �˻�
		return;

	while (pos < size)       // ���� ũ�⸸ŭ �ݺ�
	{
		switch (doc[pos])    // ������ ������ ���� �б�
		{
		case '"':            // ���ڰ� "�̸� ���ڿ�
		{
			// ���ڿ��� ���� ��ġ�� ����. �� ���� "�� �����ϱ� ���� + 1
			char *begin = doc + pos + 1;

			// ���ڿ��� �� ��ġ�� ����. ���� "�� ��ġ
			char *end = strchr(begin, '"');
			if (end == NULL)    // "�� ������ �߸��� �����̹Ƿ� 
				break;          // �ݺ��� ����

			int stringLength = end - begin;    // ���ڿ��� ���� ���̴� �� ��ġ - ���� ��ġ

											   // ��ū �迭�� ���ڿ� ����, ��ū ������ ���ڿ�
			json->tokens[tokenIndex].type = (MY_TOKEN_TYPE)TOKEN_STRING;
			// ���ڿ� ���� + NULL ������ŭ �޸� �Ҵ�
			json->tokens[tokenIndex].string = (char*)malloc(stringLength + 1);
			// �Ҵ��� �޸𸮸� 0���� �ʱ�ȭ
			memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

			// �������� ���ڿ��� ��ū�� ����
			// ���ڿ� ���� ��ġ���� ���ڿ� ���̸�ŭ�� ����
			memcpy(json->tokens[tokenIndex].string, begin, stringLength);

			tokenIndex++; // ��ū �ε��� ����

			pos = pos + stringLength + 1;    // ���� ��ġ + ���ڿ� ���� + "(+ 1)
		}
		break;

		case '0': case '1': case '2': case '3': case '4': case '5':    // ���ڰ� �����̸�
		case '6': case '7': case '8': case '9': case '-':              // -�� ������ ��
		{
			// ���ڿ��� ���� ��ġ�� ����
			char *begin = doc + pos;
			char *end;
			char *buffer;

			// ���ڿ��� �� ��ġ�� ����. ,�� �����ų�
			end = strchr(doc + pos, ',');
			if (end == NULL)
			{
				// } �� ������ ���ڿ��� ����
				end = strchr(doc + pos, '}');
				if (end == NULL)    // }�� ������ �߸��� �����̹Ƿ�
					break;          // �ݺ��� ����
			}

			int stringLength = end - begin;    // ���ڿ��� ���� ���̴� �� ��ġ - ���� ��ġ

											   // ���ڿ� ���� + NULL ������ŭ �޸� �Ҵ�
			buffer = (char*)malloc(stringLength + 1);
			// �Ҵ��� �޸𸮸� 0���� �ʱ�ȭ
			memset(buffer, 0, stringLength + 1);

			// �������� ���ڿ��� ���ۿ� ����
			// ���ڿ� ���� ��ġ���� ���ڿ� ���̸�ŭ�� ����
			memcpy(buffer, begin, stringLength);

			// ��ū ������ ����
			json->tokens[tokenIndex].type = TOKEN_NUMBER;
			// ���ڿ��� ���ڷ� ��ȯ�Ͽ� ��ū�� ����
			json->tokens[tokenIndex].number = atof(buffer);

			free(buffer);    // ���� ����

			tokenIndex++;    // ��ū �ε��� ����

			pos = pos + stringLength + 1;    // ���� ��ġ + ���ڿ� ���� + , �Ǵ� }(+ 1)
		}
		break;
		}
		pos++; // ���� ���ڷ�
	}
}
void freeJSON(JSON *json)    // JSON ���� �Լ�
{
	for (int i = 0; i < TOKEN_COUNT; i++)            // ��ū ������ŭ �ݺ�
	{
		if (json->tokens[i].type == TOKEN_STRING)    // ��ū ������ ���ڿ��̸�
			free(json->tokens[i].string);            // ���� �޸� ����
	}
}
Problem_Data SetProblemData(JSON *json, char *key, int numkey, Problem_Data & pd)    // Ű�� �ش��ϴ� ���ڸ� �������� �Լ�
{
	for (int i = 0; i < TOKEN_COUNT; i++)    // ��ū ������ŭ �ݺ�
	{
		// ��ū ������ �����̸鼭 ��ū�� ���ڿ��� Ű�� ��ġ�Ѵٸ�
		if (json->tokens[i].type == TOKEN_STRING &&
			strcmp(json->tokens[i].string, key) == 0)
		{
			// �ٷ� ���� ��ū(i + 1)�� �����̸�
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
	return pd;    // Ű�� ã�� �������� 0.0�� ��ȯ
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