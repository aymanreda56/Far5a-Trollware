#include <windows.h>
#include <iostream>
#include <string>
#include <shlobj.h>

bool SetFileExtensionIcon(const std::wstring& extension, const std::wstring& iconPath, bool revert = false) {
    HKEY hExtKey = nullptr;
    HKEY hIconKey = nullptr;
    std::wstring progId;

    // Open the extension key (e.g., .txt → txtfile)
    std::wstring extKeyPath = L"SOFTWARE\\Classes\\" + extension;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, extKeyPath.c_str(), 0, KEY_READ | KEY_WRITE, &hExtKey) != ERROR_SUCCESS) {
        std::wcerr << L"Failed to open extension key: " << extKeyPath << std::endl;
        return false;
    }

    wchar_t progIdBuffer[256];
    DWORD bufferSize = sizeof(progIdBuffer);
    DWORD type = 0;
    if (RegQueryValueExW(hExtKey, NULL, NULL, &type, reinterpret_cast<LPBYTE>(progIdBuffer), &bufferSize) != ERROR_SUCCESS || type != REG_SZ) {
        std::wcerr << L"Failed to read ProgID for extension: " << extension << std::endl;
        RegCloseKey(hExtKey);
        return false;
    }

    progId = progIdBuffer;
    RegCloseKey(hExtKey);

    // Prepare the DefaultIcon key
    std::wstring iconKeyPath = L"SOFTWARE\\Classes\\" + progId + L"\\DefaultIcon";
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, iconKeyPath.c_str(), 0, NULL, 0, KEY_WRITE | KEY_READ, NULL, &hIconKey, NULL) != ERROR_SUCCESS) {
        std::wcerr << L"Failed to create/open icon key: " << iconKeyPath << std::endl;
        return false;
    }

    if (revert) {
        // Restore from backup value
        wchar_t backupBuffer[512];
        DWORD backupSize = sizeof(backupBuffer);
        if (RegQueryValueExW(hIconKey, L"Backup", NULL, &type, reinterpret_cast<LPBYTE>(backupBuffer), &backupSize) == ERROR_SUCCESS && type == REG_SZ) {
            // Write the original icon path back
            if(RegSetValueExW(hIconKey, NULL, 0, REG_SZ, reinterpret_cast<const BYTE*>(backupBuffer), (wcslen(backupBuffer) + 1) * sizeof(wchar_t)) != ERROR_SUCCESS)
            {
                std::wcerr << L"Registry Reverting Backup Failed at: " <<hIconKey << " the default value is "<<backupBuffer<< std::endl;
                RegCloseKey(hIconKey);
                return false;
            }
            RegDeleteValueW(hIconKey, L"Backup");
            std::wcout << L"Restored original icon for " << extension << std::endl;
        } else {
            std::wcerr << L"No backup found to restore." << std::endl;
        }
    } else {
        // Backup current default icon
        wchar_t currentIcon[512];
        DWORD size = sizeof(currentIcon);

        //first check if the backup key already exists, to avoid overwriting on the backup key
        if(RegQueryValueExW(hIconKey, L"Backup", NULL, &type, reinterpret_cast<LPBYTE>(currentIcon), &size) == ERROR_FILE_NOT_FOUND)
        {
            std::wcout << L"Creating Backup..." << std::endl;


            if (RegQueryValueExW(hIconKey, NULL, NULL, &type, reinterpret_cast<LPBYTE>(currentIcon), &size) == ERROR_SUCCESS && type == REG_SZ) {
                if(RegSetValueExW(hIconKey, L"Backup", 0, REG_SZ, reinterpret_cast<const BYTE*>(currentIcon), (wcslen(currentIcon) + 1) * sizeof(wchar_t)) != ERROR_SUCCESS)
                {
                    std::wcerr << L"Registry Backup Failed at: " <<hIconKey << std::endl;
                    RegCloseKey(hIconKey);
                    return false;
                }
            
            }
        }

        

        // Set new icon
        RegSetValueExW(hIconKey, NULL, 0, REG_SZ, reinterpret_cast<const BYTE*>(iconPath.c_str()), (iconPath.length() + 1) * sizeof(wchar_t));
        std::wcout << L"Set custom icon for " << extension << L" files." << std::endl;
    }

    RegCloseKey(hIconKey);
    // Refresh Explorer icon cache
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    return true;
}

int main() {
    

    std::wstring extension = L".txt";
    std::wstring path_to_ico =  L"revert";//L"D:\\CLASSSWORKKK\\far5a\\roast.ico";//
    if (extension[0] != L'.') {
        std::wcerr << L"Extension must start with '.'" << std::endl;
        return 1;
    }

    if (std::wstring(path_to_ico) == L"revert") {
        SetFileExtensionIcon(extension, L"", true);
    } else {
        std::wstring iconPath = path_to_ico;
        SetFileExtensionIcon(extension, iconPath, false);
    }

    return 0;
}
