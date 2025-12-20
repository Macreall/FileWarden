#define UNICODE

#include <windows.h>
#include <stdio.h>
#include <shellapi.h>

// these two locations can be changed later
#define WATCH_DIR L"C:\\watchfolder"
#define APP_PATH  L"C:\\FileWarden.exe"


int WINAPI WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    HANDLE hDir = CreateFileW(
        WATCH_DIR,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE)
        return 1;

    BYTE buffer[1024];
    DWORD bytesReturned;

    while (1)
    {
        if (ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME,
            &bytesReturned,
            NULL,
            NULL))
        {
            FILE_NOTIFY_INFORMATION* fni =
                (FILE_NOTIFY_INFORMATION*)buffer;

            if (fni->Action == FILE_ACTION_ADDED)
            {
                Sleep(500);

                ShellExecuteW(
                    NULL,          
                    L"open",       
                    APP_PATH,  
                    NULL, 
                    NULL,
                    SW_SHOWNORMAL   
                );


                WCHAR filename[MAX_PATH];
                wcsncpy(filename, fni->FileName,
                         fni->FileNameLength / sizeof(WCHAR));
                filename[fni->FileNameLength / sizeof(WCHAR)] = 0;

                WCHAR fullPath[MAX_PATH];
                wsprintfW(fullPath, L"%s\\%s", WATCH_DIR, filename);

                WCHAR cmdLine[MAX_PATH * 2];
                wsprintfW(cmdLine, L"\"%s\" \"%s\"", APP_PATH, fullPath);

                STARTUPINFOW si = { sizeof(si) };
                PROCESS_INFORMATION pi;

                CreateProcessW(
                    NULL,
                    cmdLine,
                    NULL,
                    NULL,
                    FALSE,
                    0,
                    NULL,
                    NULL,
                    &si,
                    &pi
                );

                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }
    }
}
