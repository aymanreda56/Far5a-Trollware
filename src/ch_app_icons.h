#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <shlobj.h>
#include <tlhelp32.h>

/*
    This file contains relevant code to change the default icon of some files based on their extension
    the general rule works as follows:
    1) Open the registry key HKLM/Software/Classes
    2) Search for the extension (.txt or .png or whatever)
    3) Get the default value of the key, for example <FileType>
    4) Now Search for the registry key HKLM/Software/Classes/<FileType>
    5) Modify the DefaultIcon value in this key to the .ico image
    6) Don't forget to store the original default icon in a new value, we'll call it "Backup" to be able to revert everything back together

    Considerations:
    - Some files can be opened by multiple default apps, those are hard to find their FileType
    - Changes to default icons are made for specific file extensions, we have to manually specify those extensions
    - File extensions having a key hard to find, introduce irrevertible bugs during execution, so I prefer to avoid them.
    - You cannot change all icons of all extensions at once, you have to manually change the registry keys to test the elligibility of automating such a task
*/

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





void FuckUpIcons(const std::wstring & path_to_ico, std::vector<std::wstring> & extensions, bool unfuck)
{
    /*
        This function takes the absolute path to the .ico image and the extension of the target files for example ".txt"
        if you wanna revert everything back (ie; reading and reverting the key "backup" which we have previously created to DefaultIcon value), just pass the flag unfuck
    */
    for (auto& extension : extensions)
    {
        std::cout<<"Changing icons for extension: " << std::string(extension.begin(), extension.end()) << std::endl << std::endl;

        if (extension[0] != L'.') {
                std::wcerr << L"Extension must start with '.'" << std::endl;
                return;
            }

        if (unfuck) {
            SetFileExtensionIcon(extension, L"", true);
        } else {
            std::wstring iconPath = path_to_ico;
            SetFileExtensionIcon(extension, iconPath, false);
        }
    }
}


// int main() {
//     std::vector<std::wstring> extensions = {L".txt", L".exe",  L".dll", L".doc", L".docx",
//                                     L".docm", L".pdf", L".psd", L".png", L".jpg", L".7z", L".rar", L".zip", L".html"
//                                     , L".xlsm", L".xlsx", L".xls", L".cur"};


//     const std::wstring path_to_ico =  L"D:\\CLASSSWORKKK\\far5a\\roast.ico";//L"revert";//

//     FuckUpIcons(path_to_ico, extensions, false);
    
//     while(true)
//     {
//         if(GetAsyncKeyState(VK_F10)) //scanning for f10 press
//         {
//             break;
//         }
//     }

//     FuckUpIcons(path_to_ico, extensions, true);
//     return 0;
// }
