#include "PopUp.h"

#include <random>
#include <tuple>
#include <iostream>

std::tuple<int, int> GetRandomPos()
{
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);

    std::random_device rand;
    std::default_random_engine xGenerator(rand()), yGenerator(rand());

    // leave space between the message box and the border
    std::uniform_int_distribution xDistribution(200, width - 200);
    std::uniform_int_distribution yDistribution(90, height - 90);

    return { xDistribution(xGenerator), yDistribution(yGenerator) };
}

static void CALLBACK WinEventProc(
    [[maybe_unused]] HWINEVENTHOOK hWinEventHook,
    [[maybe_unused]] DWORD event,
    HWND hWnd,
    [[maybe_unused]] LONG idObject,
    [[maybe_unused]] LONG idChild,
    [[maybe_unused]] DWORD dwEventThread,
    [[maybe_unused]] DWORD dwmsEventTime
)
{
    int x, y;
    std::tie(x, y) = GetRandomPos();
    SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
}

int MessageBoxRandPos(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    HWINEVENTHOOK hHook = SetWinEventHook(
        EVENT_SYSTEM_DIALOGSTART,
        EVENT_SYSTEM_DIALOGSTART,
        NULL,
        &WinEventProc,
        GetCurrentProcessId(),
        GetCurrentThreadId(),
        WINEVENT_OUTOFCONTEXT
    );

    if (hHook == 0)
    {
        std::optional<std::wstring> optErrStr;
        MessageBox(NULL,
            (optErrStr = GetErrorStr(GetLastError())) ? optErrStr->c_str() : TEXT("GetLastError returned unknown error code"),
            TEXT("SetWinEventHook failed"),
            MB_ICONERROR | MB_OK
        );
        exit(1);
    }

    int result = MessageBox(hWnd, lpText, lpCaption, uType);

    if (hHook) UnhookWinEvent(hHook);

    return result;
}

std::optional<std::wstring> GetErrorStr(DWORD dwError)
{
    HLOCAL hlocal = NULL;  // Buffer that gets the error message string

    // Use the default system locale since we look for Windows messages.
    // Note: this MAKELANGID combination has 0 as value
    DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

    // Get the error code's textual description
    BOOL fOk = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        dwError,
        systemLocale,
        (PTSTR)&hlocal,
        0,
        NULL);

    if (!fOk) {
        // Is it a network-related error?
        HMODULE hDll = LoadLibraryEx(
            TEXT("netmsg.dll"),
            NULL,
            DONT_RESOLVE_DLL_REFERENCES
        );

        if (hDll != NULL) {
            fOk = FormatMessage(
                FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                hDll,
                dwError,
                systemLocale,
                (PTSTR)&hlocal,
                0,
                NULL
            );
            FreeLibrary(hDll);
        }
    }

    if (fOk && (hlocal != NULL)) {
        std::wstring ret((PCTSTR)LocalLock(hlocal));
        LocalFree(hlocal);
        return ret;
    }
    else {
        return std::nullopt;
    }
}
