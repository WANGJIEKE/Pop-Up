#pragma once

#include <string>
#include <optional>
#include <Windows.h>

void GetRandomPos(int* x, int* y);

static void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hWnd,
    LONG idObject,
    LONG idChild,
    DWORD dwEventThread,
    DWORD dwmsEventTime
);

int MessageBoxRandPos(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);

std::optional<std::wstring> GetErrorStr(DWORD dwLastError);
