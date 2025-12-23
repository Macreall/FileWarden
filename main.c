#define UNICODE
#include <windows.h>
#include <commctrl.h>
#include <stdbool.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")
#include <io.h>


#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_SETTINGS  1002
#define ID_TRAY_ABOUT     1003

#define IDC_COMBOBOX_DATES 101
#define IDC_SAVE_BUTTON 105


HWND hSettingsWnd = NULL;
HINSTANCE g_hInstance = NULL;
HWND hTab = NULL;
HWND hPagePC = NULL;
HWND hPageApps = NULL;
HWND monthComboBox = NULL;
HWND yearComboBox = NULL;
HWND hLabel1 = NULL;
HWND hLabel2 = NULL;
HWND hMonthLabel = NULL;
HWND hYearLabel = NULL;
HWND nameBox = NULL;
HWND poBox = NULL;
HWND hButton = NULL;

TCHAR* months[] = {
    TEXT("January"), TEXT("February"), TEXT("March"), TEXT("April"),
    TEXT("May"), TEXT("June"), TEXT("July"), TEXT("August"), TEXT("September"),
    TEXT("October"), TEXT("November"), TEXT("December"),
};

TCHAR* years[] = {
    TEXT("2020"), TEXT("2021"), TEXT("2022"), TEXT("2023"),
    TEXT("2024"), TEXT("2025"), TEXT("2026"), TEXT("2027"), TEXT("2028"),
    TEXT("2029"), TEXT("2030"),
};



void AttachConsoleAndRedirectIO() {
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
}


void OpenSettingsWindow(HINSTANCE hInstance, HWND hwndParent);
HWND OpenPopupWindow(HWND hwndParent, LPCWSTR text);

int running = 1;

int g_CurrentPage = 0;
int PAGE_COUNT = 3;


bool IsFileSendReady(int currentPage) {
    int monthIndex = -1;
    int yearIndex = -1;
    TCHAR nameText[256] = {0};
    TCHAR poText[256] = {0};

    switch (currentPage) {
        case 0:
            if (nameBox)
                GetWindowText(nameBox, nameText, 256);
            if (poBox)
                GetWindowText(poBox, poText, 256);

            if (yearComboBox)
                yearIndex = (int)SendMessage(yearComboBox, CB_GETCURSEL, 0, 0);

            return (wcslen(nameText) > 0) &&
                   (wcslen(poText) > 0) &&
                   (yearIndex != CB_ERR);

        case 1:
        case 2:
            if (nameBox)
                GetWindowText(nameBox, nameText, 256);

            if (monthComboBox)
                monthIndex = (int)SendMessage(monthComboBox, CB_GETCURSEL, 0, 0);
            if (yearComboBox)
                yearIndex = (int)SendMessage(yearComboBox, CB_GETCURSEL, 0, 0);

            return (wcslen(nameText) > 0) &&
                   (monthIndex != CB_ERR) &&
                   (yearIndex != CB_ERR);

        default:
            return false;
    }
}

void GetAllPageValues(TCHAR* nameText, TCHAR* poText, TCHAR* monthText, TCHAR* yearText)
{
    if (nameBox)
        GetWindowText(nameBox, nameText, 256);

    if (poBox)
        GetWindowText(poBox, poText, 256);

    if (monthComboBox)
        GetWindowText(monthComboBox, monthText, 256);

    if (yearComboBox)
        GetWindowText(yearComboBox, yearText, 256);
}



void SetPage(int newPage)
{
    if (newPage < 0 || newPage >= PAGE_COUNT)
        return;

    g_CurrentPage = newPage;

    if (hTab)
        TabCtrl_SetCurSel(hTab, newPage);

    ShowWindow(hPagePC, SW_HIDE);
    ShowWindow(hPageApps, SW_HIDE);
    ShowWindow(nameBox, SW_HIDE);
    ShowWindow(poBox, SW_HIDE);
    ShowWindow(monthComboBox, SW_HIDE);
    ShowWindow(yearComboBox, SW_HIDE);
    ShowWindow(hMonthLabel, SW_HIDE);
    ShowWindow(hYearLabel, SW_HIDE);
    ShowWindow(hLabel2, SW_HIDE);

    switch (newPage)
    {
        case 0:
            ShowWindow(hPagePC, SW_SHOW);
            ShowWindow(nameBox, SW_SHOW);
            ShowWindow(poBox, SW_SHOW);
            ShowWindow(hYearLabel, SW_SHOW);
            ShowWindow(yearComboBox, SW_SHOW);
            SetWindowText(hLabel1, L"Customer's Name:");
            SetWindowText(hLabel2, L"PO Number:");
            ShowWindow(hLabel2, SW_SHOW);
            SendMessage(monthComboBox, CB_RESETCONTENT, 0, 0);

            SendMessage(yearComboBox, CB_RESETCONTENT, 0, 0);
            for (int i = 0; i < _countof(years); i++) {
                SendMessage(yearComboBox, CB_ADDSTRING, 0, (LPARAM)years[i]);
            }
            ShowWindow(hButton, SW_SHOW);


            break;

        case 1:
            ShowWindow(hPageApps, SW_SHOW);
            ShowWindow(nameBox, SW_SHOW);
            ShowWindow(monthComboBox, SW_SHOW);
            ShowWindow(yearComboBox, SW_SHOW);
            ShowWindow(hMonthLabel, SW_SHOW);
            ShowWindow(hYearLabel, SW_SHOW);
            SetWindowText(hLabel1, L"Invoice Date");


            SendMessage(monthComboBox, CB_RESETCONTENT, 0, 0);
            for (int i = 0; i < _countof(months); i++) {
                SendMessage(monthComboBox, CB_ADDSTRING, 0, (LPARAM)months[i]);
            }
            SendMessage(yearComboBox, CB_RESETCONTENT, 0, 0);
            for (int i = 0; i < _countof(years); i++) {
                SendMessage(yearComboBox, CB_ADDSTRING, 0, (LPARAM)years[i]);
            }
            ShowWindow(hButton, SW_SHOW);


            break;

        case 2:
            ShowWindow(hPageApps, SW_SHOW);
            ShowWindow(monthComboBox, SW_SHOW);
            ShowWindow(nameBox, SW_SHOW);
            ShowWindow(yearComboBox, SW_SHOW);
            ShowWindow(hMonthLabel, SW_SHOW);
            ShowWindow(hYearLabel, SW_SHOW);
            SetWindowText(hLabel1, L"Companies Name:");


            SendMessage(monthComboBox, CB_RESETCONTENT, 0, 0);
            for (int i = 0; i < _countof(months); i++) {
                SendMessage(monthComboBox, CB_ADDSTRING, 0, (LPARAM)months[i]);
            }
            SendMessage(yearComboBox, CB_RESETCONTENT, 0, 0);
            for (int i = 0; i < _countof(years); i++) {
                SendMessage(yearComboBox, CB_ADDSTRING, 0, (LPARAM)years[i]);
            }
            ShowWindow(hButton, SW_SHOW);


            break;
        default: ;
    }
}









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
                    if (fnCopy != NULL) {
                        PostMessage(hwndParent, WM_APP + 1, 0, (LPARAM)fnCopy);
                    }
                }

                offset = fni->NextEntryOffset;
            } while (offset != 0);
        }
    }

    CloseHandle(hDir);
    return 0;
}



LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int currentPage = g_CurrentPage;

    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            hSettingsWnd = NULL;
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_SAVE_BUTTON:


                    if (!IsFileSendReady(currentPage)) {
                        MessageBox(hwnd, L"Please fill in all required fields before sending.", L"Warning", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    TCHAR name[256] = {0};
                    TCHAR po[256] = {0};
                    TCHAR month[256] = {0};
                    TCHAR year[256] = {0};

                    GetAllPageValues(name, po, month, year);


                    wchar_t buf[1024];
                    swprintf_s(buf, 1024, L"Name: %s\nPO: %s\nMonth: %s\nYear: %s", name, po, month, year);
                    MessageBox(hwnd, buf, L"Current Values", MB_OK);

                    break;
            default: ;
            }


        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH hBrush = CreateSolidBrush(RGB(148, 148, 148));
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);
            return 1;
        }

        case WM_CREATE:
        {
            HWND hButton = CreateWindow(
                       L"BUTTON",
                       L"Send",
                       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                       550,
                       70,
                       60,
                       30,
                       hwnd,
                       (HMENU)IDC_SAVE_BUTTON,
                       (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                       NULL);



            monthComboBox = CreateWindow(
                WC_COMBOBOX,
                TEXT(""),
                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
                350,
                120,
                120,
                300,
                hwnd,
                (HMENU)IDC_COMBOBOX_DATES,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL

            );

            yearComboBox = CreateWindow(
                WC_COMBOBOX,
                TEXT(""),
                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
                350,
                70,
                120,
                300,
                hwnd,
                (HMENU)IDC_COMBOBOX_DATES,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL

            );


            hLabel1 = CreateWindowEx(
                0,
                L"STATIC",
                L"Customer's Name:",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 50,
                150, 20,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL);

            hMonthLabel = CreateWindowEx(
                0,
                L"STATIC",
                L"Current Month",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                350, 100,
                150, 20,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL);

            hYearLabel = CreateWindowEx(
                0,
                L"STATIC",
                L"Current Year",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                350, 50,
                150, 20,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL);

            nameBox = CreateWindowEx(
                0,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | SS_LEFT | WS_TABSTOP,
                50, 70,
                220, 20,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL);

            hLabel2 = CreateWindowEx(
                0,
                L"STATIC",
                L"PO Number:",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                50, 100,
                150, 20,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL);

            poBox = CreateWindowEx(
                0,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | SS_LEFT | WS_TABSTOP,
                50, 120,
                220, 20,
                hwnd,
                NULL,
                GetModuleHandle(NULL),
                NULL);



            ShowWindow(monthComboBox, SW_HIDE);
            ShowWindow(hMonthLabel, SW_HIDE);

            ShowWindow(yearComboBox, SW_SHOW);
            ShowWindow(hYearLabel, SW_SHOW);

            ShowWindow(hButton, SW_SHOW);



            SendMessage(yearComboBox, CB_RESETCONTENT, 0, 0);
            for (int i = 0; i < _countof(years); i++) {
                SendMessage(yearComboBox, CB_ADDSTRING, 0, (LPARAM)years[i]);
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
            if (filename != NULL) {
                OpenPopupWindow(hwnd, buffer);
                free(filename);
            }
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
    wc.hbrBackground = CreateSolidBrush(RGB(148, 148, 148));
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

    hTab = CreateWindowEx(
    0,
    WC_TABCONTROL,
    NULL,
    WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_BORDER,
    10, 10, 672, 405,
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

LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND OpenPopupWindow(HWND hwndParent, LPCWSTR text) {
    const wchar_t CLASS_NAME[] = L"PopupWindowClass";


    static bool registered = false;
    if (!registered) {
        WNDCLASS wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.lpfnWndProc = PopupWndProc;
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
        WS_CHILD | WS_VISIBLE | SS_CENTER | WS_TABSTOP,
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


    AttachConsoleAndRedirectIO();

    printf("Console attached!\n");

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
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (hSettingsWnd && msg.hwnd &&
            IsChild(hSettingsWnd, msg.hwnd) &&
            msg.message == WM_KEYDOWN)
        {
            int iPage = TabCtrl_GetCurSel(hTab);





            switch (msg.wParam)
            {
                case VK_LEFT:
                    if (iPage > 0)
                        TabCtrl_SetCurSel(hTab, iPage - 1);
                        SetPage(g_CurrentPage - 1);
                    continue;

                case VK_RIGHT:
                    if (iPage < 2)
                        TabCtrl_SetCurSel(hTab, iPage + 1);
                        SetPage(g_CurrentPage + 1);
                    continue;

                case VK_ESCAPE:
                    DestroyWindow(hSettingsWnd);
                    continue;


                default: ;
            }
        }

        HWND hDlg = hSettingsWnd ? hSettingsWnd : msg.hwnd;
        if (!IsDialogMessage(hDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

