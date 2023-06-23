#include "framework.h"
#include "KB_Hook.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <locale>

#define MAX_LOADSTRING 100
#define CMD_KB_LL_START 1001
#define CMD_KB_LL_STOP 1002

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND list;
HHOOK kbLL;
WCHAR txt[100];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
DWORD CALLBACK StartKbHookLL(LPVOID);
DWORD CALLBACK StopKbHookLL(LPVOID);
LRESULT CALLBACK KbHookProcLL(int, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KBHOOK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KBHOOK));
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_KBHOOK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_KBHOOK);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        list = CreateWindowW(L"Listbox", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 100, 10, 200, 300, hWnd, 0, hInst, NULL);
        kbLL = 0;

        CreateWindowW(L"Button", L"LL Start", WS_VISIBLE | WS_CHILD, 10, 20, 75, 23, hWnd, (HMENU)CMD_KB_LL_START, hInst, NULL);
        CreateWindowW(L"Button", L"LL Stop", WS_VISIBLE | WS_CHILD, 10, 50, 75, 23, hWnd, (HMENU)CMD_KB_LL_STOP, hInst, NULL);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId)
        {
        case CMD_KB_LL_START:
            StartKbHookLL(NULL);
            break;
        case CMD_KB_LL_STOP:
            StopKbHookLL(NULL);
            break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

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

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

DWORD CALLBACK StartKbHookLL(LPVOID param)
{
    if (kbLL != 0)
        _snwprintf_s(txt, 100, L"KB LL works");
    else
        kbLL = SetWindowsHookEx(WH_KEYBOARD_LL, KbHookProcLL, GetModuleHandle(NULL), 0);

    if (kbLL != 0)
        _snwprintf_s(txt, 100, L"KB LL activated true");
    else
        _snwprintf_s(txt, 100, L"KB LL activated false");
    SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)txt);

    return 0;
}

DWORD CALLBACK StopKbHookLL(LPVOID param)
{
    if (kbLL != 0)
    {
        UnhookWindowsHookEx(kbLL);

        kbLL = 0;

        _snwprintf_s(txt, 100, L"KB LL true");
    }
    else
        _snwprintf_s(txt, 100, L"KB LL false");
    SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)txt);

    return 0;
}

LRESULT CALLBACK KbHookProcLL(int nCode, WPARAM wParam, LPARAM lParam)
{
    std::ofstream file("C:\\Users\\scrin\\Desktop\\KB_Hook\\KB_Hook\\CAPTURED.txt", std::ios_base::app);
    static char buf[100];

    if (!file.is_open())
        SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)L"Can not open file");

    if (nCode == HC_ACTION)
    {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            KBDLLHOOKSTRUCT keyInfo = *((KBDLLHOOKSTRUCT*)lParam);
            UINT sym = MapVirtualKeyExW(keyInfo.vkCode, MAPVK_VK_TO_CHAR, GetKeyboardLayout(0));

            _snwprintf_s(txt, 100, L"%d (%d): %c", keyInfo.vkCode, keyInfo.scanCode, (char)sym);
            SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)txt);

            buf[std::strlen(buf)] = static_cast<char>(sym);
            buf[std::strlen(buf) + 1] = '\0';

            std::locale::global(std::locale("")); // Установка глобальной локали

            file << buf << std::endl; // Запись данных в файл
        }
    }

    file.close();
    return CallNextHookEx(kbLL, nCode, wParam, lParam);
}