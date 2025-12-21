#define UNICODE
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <stdbool.h>
#include <stdio.h>


#pragma comment(lib, "comctl32.lib")

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_SETTINGS  1002
#define ID_TRAY_ABOUT     1003

HWND hSettingsWnd = NULL;
HINSTANCE g_hInstance = NULL;
HWND hTab = NULL;
HWND hPagePC = NULL;
HWND hPageApps = NULL;



void OpenSettingsWindow(HINSTANCE hInstance, HWND hwndParent);

HWND OpenPopupWindow(HWND hwndParent, LPCWSTR text);

int running = 1;



NOTIFYICONDATA nid;


DWORD WINAPI WatchFolder(LPVOID lpParam) {
    HWND hwndParent = (HWND)lpParam;
    LPCWSTR folderPath = L"C:\\watch_folder";

    HANDLE hDir = CreateFileW(
        folderPath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        MessageBox(hwndParent, L"Failed to open folder", L"Error", MB_OK);
        return 1;
    }

    BYTE buffer[1024];
    DWORD bytesReturned = 0;

    while (running) {
        if (ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME,
            &bytesReturned,
            NULL,
            NULL
        )) {
            DWORD offset = 0;
            do {
                FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)(buffer + offset);

                unsigned long long const len = fni->FileNameLength / sizeof(WCHAR);
                WCHAR filename[256] = {0};
                wcsncpy_s(filename, 256, fni->FileName, len);
                filename[len] = 0;

                if (fni->Action == FILE_ACTION_ADDED) {
                    WCHAR* fnCopy = _wcsdup(filename);
                    PostMessage(hwndParent, WM_APP + 1, 0, (LPARAM)fnCopy);
                    free(fnCopy);
                }

                offset = fni->NextEntryOffset;
            } while (offset != 0);
        }
    }

    CloseHandle(hDir);
    return 0;
}



LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            hSettingsWnd = NULL;
            break;

        case WM_NOTIFY:
            if (((LPNMHDR)lParam)->code == TCN_SELCHANGE) {
                int iPage = TabCtrl_GetCurSel(hTab);

                if (iPage == 0) {
                    ShowWindow(hPagePC, SW_SHOW);
                    ShowWindow(hPageApps, SW_HIDE);
                } else {
                    ShowWindow(hPagePC, SW_HIDE);
                    ShowWindow(hPageApps, SW_SHOW);
                }
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_APP + 1: {
            WCHAR* filename = (WCHAR*)lParam;
            static WCHAR buffer[256];
            wcsncpy_s(buffer, 256, filename, 255);
            buffer[255] = 0;
            OpenPopupWindow(hwnd, buffer);
            free(filename);
        } break;

        case WM_TRAYICON:
            if (lParam == WM_LBUTTONDOWN) {
                HMENU menu = CreatePopupMenu();

                AppendMenu(menu, MF_STRING, ID_TRAY_SETTINGS, L"Settings");
                AppendMenu(menu, MF_STRING, ID_TRAY_ABOUT, L"About");
                AppendMenu(menu, MF_SEPARATOR, 0, NULL);
                AppendMenu(menu, MF_STRING, ID_TRAY_EXIT, L"Exit");

                POINT p;
                GetCursorPos(&p);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(menu, TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, NULL);
                DestroyMenu(menu);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case ID_TRAY_EXIT:
                    DestroyWindow(hwnd);
                    break;
            case ID_TRAY_SETTINGS:
                    OpenSettingsWindow(g_hInstance, hwnd);
                    break;
            case ID_TRAY_ABOUT:
                    MessageBox(hwnd, L"TrayApp v1.0", L"About", MB_OK);
                    break;
            default: ;
            }
            break;


        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            // DestroyIcon(nid.hIcon);
            hSettingsWnd = NULL;
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);

    }
    return 0;
}

void OpenSettingsWindow(HINSTANCE hInstance, HWND hwndParent) {
    if (hSettingsWnd != NULL) {

        SetForegroundWindow(hSettingsWnd);
        return;
    }

    const wchar_t CLASS_NAME[] = L"SettingsWindowClass";

    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    hSettingsWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Settings",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        400, 200, 700, 450,
        hwndParent,
        NULL,
        hInstance,
        NULL
    );

    HWND hTab = CreateWindowEx(
    0,
    WC_TABCONTROL,
    NULL,
    WS_CHILD | WS_VISIBLE,
    5, 5, 700, 450,
    hSettingsWnd,
    (HMENU)4001,
    hInstance,
    NULL
);


    TCITEM tie;
    tie.mask = TCIF_TEXT;

    tie.pszText = L"Job Tickets";
    TabCtrl_InsertItem(hTab, 0, &tie);

    tie.pszText = L"Invoices";
    TabCtrl_InsertItem(hTab, 1, &tie);

    tie.pszText = L"Parts Receipts";
    TabCtrl_InsertItem(hTab, 2, &tie);




    if (hSettingsWnd) {
        ShowWindow(hSettingsWnd, SW_SHOW);



        UpdateWindow(hSettingsWnd);
    }
}

HWND OpenPopupWindow(HWND hwndParent, LPCWSTR text) {
    const wchar_t CLASS_NAME[] = L"PopupWindowClass";


    static bool registered = false;
    if (!registered) {
        WNDCLASS wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClass(&wc);
        registered = true;
    }

    HWND hwnd = CreateWindowEx(
    WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
    CLASS_NAME,
    L"New File Alert",
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT, 300, 100,
    NULL,
    NULL,
    GetModuleHandle(NULL),
    NULL
);

    ShowWindow(hwnd, SW_SHOW);

    CreateWindowEx(
        0,
        L"STATIC",
        text,
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10, 10, 280, 80,
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    UpdateWindow(hwnd);
    return hwnd;
}



int WINAPI WinMain(

    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
) {




    const wchar_t CLASS_NAME[] = L"TrayAppClass";
    g_hInstance = hInstance;






    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TrayAppClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
    0,
    CLASS_NAME,
    L"LISTBOX",
    WS_OVERLAPPEDWINDOW,
    0, 0, 0, 0,
    NULL,
    NULL,
    hInstance,
    NULL
);

    CreateThread(
    NULL,
    0,
    WatchFolder,
    hwnd,
    0,
    NULL
);
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    wcscpy_s(nid.szTip, sizeof(nid.szTip), L"Your Tooltip Text Here");

    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

