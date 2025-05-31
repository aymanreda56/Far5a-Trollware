#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <shlobj.h>
#include <tlhelp32.h>

/*
    This file is just a demo that:
    1) Changes folder icons to a hardcoded .ico file
    2) Changes icons of some program extenstions

    Just to test changing all the icons at once.
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























int create_LOC_MACHINE_Key(LPCWSTR subkey_path, LPCWSTR value_name, LPCWSTR data)
{
    HKEY hKey;

    // Create or open the key
    LONG result = RegCreateKeyExW(
        HKEY_LOCAL_MACHINE,   // Root key
        subkey_path,               // Subkey path
        0,                    // Reserved
        NULL,                 // Class
        REG_OPTION_NON_VOLATILE, // Options
        KEY_WRITE | KEY_WOW64_64KEY , // Access rights
        NULL,                 // Security attributes
        &hKey,                // Handle to opened key
        NULL                  // Disposition (created or opened)
    );

    if (result != ERROR_SUCCESS) {
        std::wcerr << L"Failed to create or open the key. Error code: " << result << std::endl;
        return 1;
    }



    result = RegSetValueExW(
        hKey,
        value_name,
        0,
        REG_EXPAND_SZ,
        reinterpret_cast<const BYTE*>(data),
        (DWORD)((wcslen(data) + 1) * sizeof(wchar_t))
    );

    if (result != ERROR_SUCCESS) {
        std::wcerr << L"Failed to set value 1. Error code: " << result << std::endl;
        RegCloseKey(hKey);
        return 1;
    }


    RegCloseKey(hKey);

    std::cout << "Registry key and values created successfully." << std::endl;
    return 1;
}


// find process ID by process name
int findMyProc(const char *procname) {

  HANDLE hSnapshot;
  PROCESSENTRY32 pe;
  int pid = 0;
  BOOL hResult;

  // snapshot of all processes in the system
  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

  // initializing size: needed for using Process32First
  pe.dwSize = sizeof(PROCESSENTRY32);

  // info about first process encountered in a system snapshot
  hResult = Process32First(hSnapshot, &pe);

  // retrieve information about the processes
  // and exit if unsuccessful
  while (hResult) {
    // if we find the process: return process ID
    if (strcmp(procname, pe.szExeFile) == 0) {
      pid = pe.th32ProcessID;
      break;
    }
    hResult = Process32Next(hSnapshot, &pe);
  }

  // closes an open handle (CreateToolhelp32Snapshot)
  CloseHandle(hSnapshot);
  return pid;
}







void FuckUpFolders(const std::wstring path_to_ico, const bool unfuck)
{

    if(unfuck)
    {
        HKEY hkey = nullptr;


        if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer", 0, KEY_ALL_ACCESS, &hkey) != ERROR_SUCCESS)
        {
            std::cout<<"Failed to open the key to unfuck up the folder... reverting everything back"<<std::endl;
            return;
        }

        if(RegDeleteTreeW(hkey, L"Shell Icons") != ERROR_SUCCESS){
            std::cout<<"error when unfucking the folder icons, to restore it manually, jump to the registry 'Computer\\HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons' and just delete the key 'Shell Icons' then restart the windows explorer "<<std::endl;
            return;
        }
    }


    else
    {
        LPCWSTR subkey_path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons";

        // Set two expandable string values
        LPCWSTR valueName1 = L"3";

        create_LOC_MACHINE_Key(subkey_path, valueName1, path_to_ico.c_str());

        LPCWSTR valueName2 = L"4";

        create_LOC_MACHINE_Key(subkey_path, valueName2, path_to_ico.c_str());
    }



    //Restarting (basically only killing) Windows File Explorer
    int pid = 0; // process ID

    pid = findMyProc("explorer.exe");
    if (pid) {
        printf("\n\nPID of windows file explorer = %d\n\n\n", pid);


        //finally killing the process
        HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (NULL != handle) {   
            TerminateProcess(handle, 0);
            CloseHandle(handle);
        }
    }
    
}





int main() {
    

    std::vector<std::wstring> extensions = {L".txt", L".exe",  L".dll", L".doc", L".docx",
                                    L".docm", L".pdf", L".psd", L".png", L".jpg", L".7z", L".rar", L".zip", L".html"
                                    , L".xlsm", L".xlsx", L".xls", L".cur"};


    const std::wstring path_to_ico =  L"D:\\CLASSSWORKKK\\far5a\\roast.ico";//L"revert";//


    FuckUpFolders(path_to_ico, false);
    FuckUpIcons(path_to_ico, extensions, false);
    


    while(true)
    {
        if(GetAsyncKeyState(VK_F10)) //scanning for f10 press
        {
            break;
        }
    }

    
    FuckUpIcons(path_to_ico, extensions, true);
    FuckUpFolders(path_to_ico, true);

    
    

    return 0;
}
