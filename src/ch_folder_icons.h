#include <windows.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>

/*
    This file is responsible for changing all folder icons to a .ico image
    it achieves such a goal by making two new Keys in the windows registry with "3" and "4" values
*/


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
    /*
        This function changes all folder icons to a .ico image
        We can revert all changes by passing unfuck flag
        It works by introducing two new values "3" & "4" to the key HLM/Software/Microsoft/Windows/CurrentVersion/Explorer/Shell Icons
        The data of those values are just the absolute path of the .ico image

        Reverting everything back is done simply by deleting the key Shell Icons.
    */

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





// int main() {
    
//     LPCWSTR subkey_path = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Icons";

//     // Set two expandable string values
//     LPCWSTR valueName1 = L"3";
//     LPCWSTR data1 = L"D:\\CLASSSWORKKK\\far5a\\roast.ico";

//     create_LOC_MACHINE_Key(subkey_path, valueName1, data1);

//     LPCWSTR valueName2 = L"4";
//     LPCWSTR data2 = L"D:\\CLASSSWORKKK\\far5a\\roast.ico";

//     create_LOC_MACHINE_Key(subkey_path, valueName2, data2);


//     //Restarting (basically only killing) Windows File Explorer
//     int pid = 0; // process ID

//     pid = findMyProc("explorer.exe");
//     if (pid) {
//         printf("\n\nPID of windows file explorer = %d\n\n\n", pid);


//         //finally killing the process
//         HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
//         if (NULL != handle) {   
//             TerminateProcess(handle, 0);
//             CloseHandle(handle);
//         }
//     }

//     return 0;
// }