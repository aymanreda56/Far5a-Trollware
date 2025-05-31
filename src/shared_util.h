#include <windows.h>
#include <iostream>
#include <string>

/*
    This file contains some shared functions across different code files, So I just put them here to mark that they are used
    multiple times.
*/



std::wstring GetCurrentBackground()
{
    /*
        Returns the current background image path as a wide string.
        Reads data of value "Wallpaper" in the key HKEY_CURRENT_USER/Conrol Panel/Desktop
    */

    HKEY hExtKey = nullptr;

    // Open the extension key (e.g., .txt → txtfile)
    std::wstring extKeyPath = L"Control Panel\\Desktop";
    if (RegOpenKeyExW(HKEY_CURRENT_USER, extKeyPath.c_str(), 0, KEY_READ | KEY_WRITE, &hExtKey) != ERROR_SUCCESS) {
        std::wcerr << L"Failed to open extension key: " << extKeyPath << std::endl;
        return L"Not Found";
    }

    wchar_t BackGroundPath[256];
    DWORD bufferSize = sizeof(BackGroundPath);
    DWORD type = 0;
    if (RegQueryValueExW(hExtKey, L"WallPaper", NULL, &type, reinterpret_cast<LPBYTE>(BackGroundPath), &bufferSize) != ERROR_SUCCESS || type != REG_SZ) {
        std::wcerr << L"Failed to read Background path"<< std::endl;
        RegCloseKey(hExtKey);
        return L"Not Found";
    }

    RegCloseKey(hExtKey);

    return std::wstring(BackGroundPath);
}