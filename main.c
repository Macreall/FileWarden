#define UNICODE
#include <windows.h>
#include <commctrl.h>
#include <stdbool.h>
#include <stdio.h>


#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_SETTINGS  1002
#define ID_TRAY_ABOUT     1003
#define IDC_COMBOBOX_DATES 101
#define IDC_SAVE_BUTTON 105



HWND hPopupTab = NULL;
HWND hPopupWnd = NULL;
HWND hSettingsWnd = NULL;
HWND hSettingsTab = NULL;

HINSTANCE g_hInstance = NULL;
HWND monthComboBox = NULL;
HWND yearComboBox = NULL;
HWND companiesComboBox = NULL;

HWND hLabel1 = NULL;
HWND hLabel2 = NULL;
HWND hMonthLabel = NULL;
HWND hYearLabel = NULL;
HWND nameBox = NULL;
HWND poBox = NULL;
HWND hButton = NULL;





void OpenSettingsWindow(HINSTANCE hInstance, HWND hwndParent);
HWND OpenPopupWindow(HWND hwndParent, LPCWSTR text);

int running = 1;

int g_CurrentPage = 0;
WCHAR g_CurrentFilename[MAX_PATH];
int PAGE_COUNT = 3;

int LoadTabCount(void)
{
    return GetPrivateProfileIntW(
        L"Tabs",
        L"Count",
        0,
        L"C:\\watchFolder\\settings.ini"
    );
}

void LoadTabsFromIni(HWND hTab)
{
    TabCtrl_DeleteAllItems(hTab);

    int count = LoadTabCount();

    for (int i = 0; i < count; i++)
    {
        wchar_t section[16];
        wchar_t name[64];

        swprintf_s(section, 16, L"Tab%d", i);

        GetPrivateProfileStringW(
            section,
            L"Name",
            L"Unnamed",
            name,
            64,
            L"C:\\watchFolder\\settings.ini"
        );

        TCITEM tie = {0};
        tie.mask = TCIF_TEXT;
        tie.pszText = name;

        TabCtrl_InsertItem(hTab, i, &tie);
    }

    PAGE_COUNT = count;
}



void LoadFromIni(HWND comboBox, LPCWSTR text) {
    SendMessage(comboBox, CB_RESETCONTENT, 0, 0);

    WCHAR value[64];
    WCHAR key[8];
    int index = 1;

    while (1) {
        swprintf_s(key, 8, L"%d", index);
        GetPrivateProfileStringW(
            text,
            key,
            L"",
            value,
            64,
            L"C:\\watchFolder\\settings.ini"
        );

        if (value[0] == L'\0')
            break;

        SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)value);
        index++;
    }
}



void SaveFile(int currentPage, wchar_t* nameText, wchar_t* poText, wchar_t* monthText, wchar_t* yearText, wchar_t* companiesText) {
    wchar_t oldFilename[MAX_PATH];
    swprintf_s(
        oldFilename,
        MAX_PATH,
        L"C:\\watchFolder\\%s",
        g_CurrentFilename
    );

    CharUpperBuffW(oldFilename, wcslen(oldFilename));


    wchar_t destination[MAX_PATH] = L"C:\\Users\\12096\\DropBox\\Harrison's DropBox\\";

    wchar_t filename[128] = L"";






    switch (currentPage) {
        case 0:
            if (nameBox)
                GetWindowText(nameBox, nameText, 256);

            if (poBox)
                GetWindowText(poBox, poText, 256);

            int sel = (int)SendMessage(yearComboBox, CB_GETCURSEL, 0, 0);
            if (sel != CB_ERR)
                SendMessage(yearComboBox, CB_GETLBTEXT, sel, (LPARAM)yearText);

            if (yearComboBox)
                GetWindowText(yearComboBox, yearText, 256);



            wcscat_s(destination, MAX_PATH, yearText);
            wcscat_s(destination, MAX_PATH, L"\\Job Tickets ");
            wcscat_s(destination, MAX_PATH, yearText);

            wcscat_s(filename, 128, nameText);
            wcscat_s(filename, 128, L" #");
            wcscat_s(filename, 128, poText);
            wcscat_s(filename, 128, L".pdf");

            wcscat_s(destination, MAX_PATH, L"\\");
            wcscat_s(destination, MAX_PATH, filename);

            if (GetFileAttributesW(oldFilename) == INVALID_FILE_ATTRIBUTES) {
                MessageBox(NULL, L"Source file not found!", L"Error", MB_OK | MB_ICONERROR);
                return;
            }


            if (!MoveFileExW(oldFilename, destination, MOVEFILE_REPLACE_EXISTING)) {
                DWORD err = GetLastError();
                wchar_t buf[256];
                swprintf_s(buf, 256, L"Failed to move file to: %ls (Error %lu)", destination, err);
                MessageBox(NULL, buf, L"Error", MB_OK | MB_ICONERROR);
                return;
            }

            g_CurrentFilename[0] = L'\0';

            break;





        case 1:
            if (nameBox)
                GetWindowText(nameBox, nameText, 256);

            int sel2 = (int)SendMessage(yearComboBox, CB_GETCURSEL, 0, 0);
            int sel3 = (int)SendMessage(monthComboBox, CB_GETCURSEL, 0, 0);

            if (sel2 != CB_ERR)
                SendMessage(yearComboBox, CB_GETLBTEXT, sel2, (LPARAM)yearText);
            if (sel3 != CB_ERR)
                SendMessage(monthComboBox, CB_GETLBTEXT, sel3, (LPARAM)monthText);

            if (monthComboBox)
                GetWindowText(monthComboBox, monthText, 256);

            if (yearComboBox)
                GetWindowText(yearComboBox, yearText, 256);

            wcscat_s(destination, MAX_PATH, yearText);
            wcscat_s(destination, MAX_PATH, L"\\Invoices ");
            wcscat_s(destination, MAX_PATH, yearText);
            wcscat_s(destination, MAX_PATH, L"\\");
            wcscat_s(destination, MAX_PATH, monthText);


            wcscat_s(filename, 128, nameText);
            wcscat_s(filename, 128, L".pdf");

            wcscat_s(destination, MAX_PATH, L"\\");
            wcscat_s(destination, MAX_PATH, filename);

            if (GetFileAttributesW(oldFilename) == INVALID_FILE_ATTRIBUTES) {
                MessageBox(NULL, L"Source file not found!", L"Error", MB_OK | MB_ICONERROR);
            }




            if (!MoveFileExW(oldFilename, destination, MOVEFILE_REPLACE_EXISTING)) {
                DWORD err = GetLastError();
                wchar_t buf[256];
                swprintf_s(buf, 256, L"Failed to move file to: %ls (Error %lu)", destination, err);
                MessageBox(NULL, buf, L"Error", MB_OK | MB_ICONERROR);
                return;
            }

            g_CurrentFilename[0] = L'\0';


            break;



        case 2:
            if (monthComboBox)
                GetWindowText(monthComboBox, monthText, 256);

            if (yearComboBox)
                GetWindowText(yearComboBox, yearText, 256);

            if (companiesComboBox)
                GetWindowText(companiesComboBox, companiesText, 256);



            wcscat_s(destination, MAX_PATH, yearText);
            wcscat_s(destination, MAX_PATH, L"\\Parts Receipts ");
            wcscat_s(destination, MAX_PATH, yearText);
            wcscat_s(destination, MAX_PATH, L"\\");
            wcscat_s(destination, MAX_PATH, monthText);


            wcscat_s(filename, 128, companiesText);
            wcscat_s(filename, 128, L".pdf");

            wcscat_s(destination, MAX_PATH, L"\\");
            wcscat_s(destination, MAX_PATH, filename);

            if (GetFileAttributesW(oldFilename) == INVALID_FILE_ATTRIBUTES) {
                MessageBox(NULL, L"Source file not found!", L"Error", MB_OK | MB_ICONERROR);
            }


            if (!MoveFileExW(oldFilename, destination, MOVEFILE_REPLACE_EXISTING)) {
                DWORD err = GetLastError();
                wchar_t buf[256];
                swprintf_s(buf, 256, L"Failed to move file to: %ls (Error %lu)", destination, err);
                MessageBox(NULL, buf, L"Error", MB_OK | MB_ICONERROR);
                return;
            }

            g_CurrentFilename[0] = L'\0';

            break;

        default: ;
    }
}

bool IsFileSendReady(int currentPage) {
    int monthIndex = -1;
    int yearIndex = -1;
    TCHAR nameText[256] = {0};
    TCHAR companiesNameText[256] = {0};
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
            if (nameBox)
                GetWindowText(nameBox, nameText, 256);

            if (monthComboBox)
                monthIndex = (int)SendMessage(monthComboBox, CB_GETCURSEL, 0, 0);
            if (yearComboBox)
                yearIndex = (int)SendMessage(yearComboBox, CB_GETCURSEL, 0, 0);

            return (wcslen(nameText) > 0) &&
                   (monthIndex != CB_ERR) &&
                   (yearIndex != CB_ERR);
        case 2:
            if (companiesComboBox)
                GetWindowText(companiesComboBox, companiesNameText, 256);

            if (monthComboBox)
                monthIndex = (int)SendMessage(monthComboBox, CB_GETCURSEL, 0, 0);
            if (yearComboBox)
                yearIndex = (int)SendMessage(yearComboBox, CB_GETCURSEL, 0, 0);

            return (wcslen(companiesNameText) > 0) &&
                   (monthIndex != CB_ERR) &&
                   (yearIndex != CB_ERR);

        default:
            return false;
    }
}


void SetPage(int newPage)
{
    if (newPage < 0 || newPage >= PAGE_COUNT)
        return;

    g_CurrentPage = newPage;

    if (hPopupTab)
        TabCtrl_SetCurSel(hPopupTab, newPage);

    ShowWindow(nameBox, SW_HIDE);
    ShowWindow(poBox, SW_HIDE);
    ShowWindow(monthComboBox, SW_HIDE);
    ShowWindow(yearComboBox, SW_HIDE);
    ShowWindow(hMonthLabel, SW_HIDE);
    ShowWindow(hYearLabel, SW_HIDE);
    ShowWindow(hLabel2, SW_HIDE);
    ShowWindow(companiesComboBox, SW_HIDE);


    switch (newPage)
    {
        case 0:
            ShowWindow(nameBox, SW_SHOW);
            ShowWindow(poBox, SW_SHOW);
            ShowWindow(hYearLabel, SW_SHOW);
            ShowWindow(yearComboBox, SW_SHOW);


            SetWindowText(hLabel1, L"Customer's Name:");
            SetWindowText(hLabel2, L"PO Number:");
            ShowWindow(hLabel2, SW_SHOW);
            ShowWindow(companiesComboBox, SW_HIDE);


            LoadFromIni(yearComboBox, L"Years");


            ShowWindow(hButton, SW_SHOW);



            break;

        case 1:
            ShowWindow(nameBox, SW_SHOW);
            ShowWindow(monthComboBox, SW_SHOW);
            ShowWindow(yearComboBox, SW_SHOW);
            ShowWindow(hMonthLabel, SW_SHOW);
            ShowWindow(hYearLabel, SW_SHOW);
            ShowWindow(companiesComboBox, SW_HIDE);

            SetWindowText(hLabel1, L"Invoice Date");

            LoadFromIni(yearComboBox, L"Years");
            LoadFromIni(monthComboBox, L"Months");



            ShowWindow(hButton, SW_SHOW);



            break;

        case 2:
            ShowWindow(monthComboBox, SW_SHOW);
            ShowWindow(nameBox, SW_HIDE);
            ShowWindow(yearComboBox, SW_SHOW);
            ShowWindow(hMonthLabel, SW_SHOW);
            ShowWindow(hYearLabel, SW_SHOW);
            ShowWindow(companiesComboBox, SW_SHOW);
            SetWindowText(hLabel1, L"Companies Name:");

            LoadFromIni(companiesComboBox, L"Companies");
            LoadFromIni(yearComboBox, L"Years");
            LoadFromIni(monthComboBox, L"Months");







            ShowWindow(hButton, SW_SHOW);



            break;
        default: ;
    }
}









NOTIFYICONDATA nid;


DWORD WINAPI WatchFolder(LPVOID lpParam) {
    HWND hwndParent = (HWND)lpParam;
    LPCWSTR folderPath = L"C:\\watchFolder";

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
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            hSettingsWnd = NULL;
            PostQuitMessage(0);
            break;

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
            hButton = CreateWindow(
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

            companiesComboBox = CreateWindow(
                WC_COMBOBOX,
                TEXT(""),
                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VSCROLL | WS_TABSTOP,
                50,
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
                WS_EX_CLIENTEDGE,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP,
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

            if (!filename)
                break;

            size_t len = wcslen(filename);

            if (len < 4 || _wcsicmp(filename + len - 4, L".pdf") != 0) {
                free(filename);
                break;
            }

            wcsncpy_s(g_CurrentFilename, MAX_PATH, filename, _TRUNCATE);
            OpenPopupWindow(hwnd, g_CurrentFilename);
            LoadFromIni(yearComboBox, L"Years");
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


    if (hSettingsWnd) {
        ShowWindow(hSettingsWnd, SW_SHOW);



        UpdateWindow(hSettingsWnd);
    }
}

LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int currentPage = g_CurrentPage;
    switch(msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            hPopupWnd = NULL;
            break;
        case WM_KEYDOWN:
        {
            HWND hTab =
                (hwnd == hSettingsWnd) ? hSettingsTab :
                (hwnd == hPopupWnd)    ? hPopupTab    :
                NULL;

            if (!hTab)
                break;

            int page = TabCtrl_GetCurSel(hTab);

            switch (wParam) {
                case VK_LEFT:
                    if (page > 0) {
                        TabCtrl_SetCurSel(hTab, page - 1);
                        SetPage(page - 1);
                    }
                    return 0;

                case VK_RIGHT:
                    if (page < PAGE_COUNT - 1) {
                        TabCtrl_SetCurSel(hTab, page + 1);
                        SetPage(page + 1);
                    }
                    return 0;

                case VK_ESCAPE:
                    DestroyWindow(hwnd);
                    return 0;
                default: ;
            }

            break;
        }

        case WM_NOTIFY:
        {
            LPNMHDR pnmh = (LPNMHDR)lParam;

            if (pnmh->hwndFrom == hPopupTab && pnmh->code == TCN_SELCHANGE)
            {
                int newPage = TabCtrl_GetCurSel(hPopupTab);
                SetPage(newPage);
            }
        }
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
                    TCHAR companiesText[256] = {0};

                    SaveFile(currentPage, name, po, month, year, companiesText);



                    wchar_t buf[256];
                    swprintf_s(buf, 256, L"File saved successfully!");
                    MessageBox(hwnd, buf, L"Success", MB_OK);

                    DestroyWindow(hwnd);


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
            hButton = CreateWindow(
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

            companiesComboBox = CreateWindow(
                WC_COMBOBOX,
                TEXT(""),
                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VSCROLL | WS_TABSTOP,
                50,
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
                WS_EX_CLIENTEDGE,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
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
                WS_EX_CLIENTEDGE,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
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



        }
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
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClass(&wc);
        registered = true;
    }

    HWND hwnd = CreateWindowEx(
    WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
    CLASS_NAME,
    L"New File Alert",
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
    400, 200, 700, 450,
    hwndParent,
    NULL,
    g_hInstance,
    NULL
);

    if (!hwnd) {
        return NULL;
    }

    hPopupWnd = hwnd;


    hPopupTab = CreateWindowEx(
    0,
    WC_TABCONTROL,
    NULL,
    WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_BORDER,
    10, 10, 672, 405,
    hwnd,
    (HMENU)4001,
    NULL,
    NULL
);


    LoadTabsFromIni(hPopupTab);
    SetPage(0);






    if (PopupWndProc) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

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
    wcscpy_s(nid.szTip, sizeof(nid.szTip), L"File Warden");

    Shell_NotifyIcon(NIM_ADD, &nid);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        HWND ownerWnd = NULL;
        HWND activeTab = NULL;

        if (hSettingsWnd && msg.hwnd &&
            (msg.hwnd == hSettingsWnd || IsChild(hSettingsWnd, msg.hwnd)))
        {
            ownerWnd = hSettingsWnd;
            activeTab = hSettingsTab;
        }
        else if (hPopupWnd && msg.hwnd &&
            (msg.hwnd == hPopupWnd || IsChild(hPopupWnd, msg.hwnd)))
        {
            ownerWnd = hPopupWnd;
            activeTab = hPopupTab;
        }


        if (ownerWnd && msg.message == WM_KEYDOWN)
        {
            int page = TabCtrl_GetCurSel(activeTab);

            switch (msg.wParam)
            {
                case VK_LEFT:
                    if (page > 0) {
                        TabCtrl_SetCurSel(activeTab, page - 1);
                        SetPage(page - 1);
                    }
                    continue;

                case VK_RIGHT:
                    if (page < 2) {
                        TabCtrl_SetCurSel(activeTab, page + 1);
                        SetPage(page + 1);
                    }
                    continue;

                case VK_ESCAPE:
                    DestroyWindow(ownerWnd);
                    continue;
                default: ;
            }
        }



        HWND hDlg = ownerWnd ? ownerWnd : msg.hwnd;
        if (!IsDialogMessage(hDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
