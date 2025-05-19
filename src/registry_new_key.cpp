#include <windows.h>
#include <iostream>



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
        KEY_WRITE | KEY_WOW64_64KEY, // Access rights
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
}









int main() {
    
    LPCWSTR subkey_path = L"Computer\\HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Icons";

    // Set two expandable string values
    LPCWSTR valueName1 = L"3";
    LPCWSTR data1 = L"D:\\CLASSSWORKKK\\far5a\\roast.ico";

    create_LOC_MACHINE_Key(subkey_path, valueName1, data1);

    LPCWSTR valueName2 = L"4";
    LPCWSTR data2 = L"D:\\CLASSSWORKKK\\far5a\\roast.ico";

    create_LOC_MACHINE_Key(subkey_path, valueName2, data2);


    

    
    return 0;
}