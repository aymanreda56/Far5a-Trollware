#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <filesystem>

#include "MMSystem.h"
#include <mmdeviceapi.h>

#include <mciapi.h>
#include <endpointvolume.h>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")






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




















int changeAllCursors(const char* curPath)
{
    HCURSOR cursorimg = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );


    //Changing all cursor types to chicken lolll
    if (!SetSystemCursor(cursorimg, 32512)) {
        std::cerr << "Failed to set system cursor of Arrow. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg2 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg2, 32513)) {
        std::cerr << "Failed to set system cursor of text index. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg3 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg3, 32514)) {
        std::cerr << "Failed to set system cursor of hour glass. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg4 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg4, 32515)) {
        std::cerr << "Failed to set system cursor of cross hair. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg5 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg5, 32649)) {
        std::cerr << "Failed to set system cursor of hand. Error: " << GetLastError() << std::endl;
        return 1;
    }


    return 0;
}



int changeCursortoDefault()
{
    if (!SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0)) {
        std::cerr << "Failed to restore default cursors. Error: " << GetLastError() << std::endl;
        return 1;
    }
    return 0;
}


std::wstring GetCurrentBackground()
{
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








// Sound functions
DWORD WINAPI MP3Proc(_In_ LPVOID lpParameter) //lpParameter can be a pointer to a structure that store data that you cannot access outside of this function. You can prepare this structure before `CreateThread` and give it's address in the `lpParameter`
{
    //Data *data = (Data*)lpParameter; //If you call this structure Data, but you can call it whatever you want.
    while (true)
    {
        mciSendString("play mp3 from 0 wait", NULL, 0, NULL);
        //Do here what you want to do when the mp3 playback is over
        SuspendThread(GetCurrentThread()); //or the handle of this thread that you keep in a static variable instead
    }
}

int AttachToMasterOutput(IMMDeviceEnumerator* &deviceEnumerator, IMMDevice* &defaultDevice, IAudioEndpointVolume* &endpointVolume)
{
    CoInitialize(nullptr);
    
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  __uuidof(IMMDeviceEnumerator),
                                  (void**)&deviceEnumerator);
    if (FAILED(hr)) {
        std::cerr << "Failed to create device enumerator\n";
        return 1;
    }

    
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    if (FAILED(hr)) {
        std::cerr << "Failed to get default audio endpoint\n";
        deviceEnumerator->Release();
        return 1;
    }

    
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
                                 CLSCTX_INPROC_SERVER, nullptr,
                                 (void**)&endpointVolume);
    if (FAILED(hr)) {
        std::cerr << "Failed to activate endpoint volume\n";
        defaultDevice->Release();
        deviceEnumerator->Release();
        return 1;
    }


    return 0;
}

int DetachFromMasterOutput(IAudioEndpointVolume* &endpointVolume, IMMDevice* &defaultDevice, IMMDeviceEnumerator* &deviceEnumerator)
{
    endpointVolume->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
    return 0;
}





int main(int argc, char* argv []) {


    // Play the sound
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    IMMDevice* defaultDevice = nullptr;
    IAudioEndpointVolume* endpointVolume = nullptr;

    //Increase the volume
    AttachToMasterOutput(deviceEnumerator, defaultDevice, endpointVolume);
    endpointVolume->SetMasterVolumeLevelScalar(0.8, nullptr);


    LPCSTR LaughSoundPath = "../data/mus_f_newlaugh.wav";//std::filesystem::absolute("eid_far5a_cropped.wav").string().c_str();
    PlaySound(TEXT(LaughSoundPath), NULL, SND_SYNC);



    
    

    std::vector<std::wstring> extensions = {L".txt", L".exe",  L".dll", L".doc", L".docx",
                                    L".docm", L".pdf", L".psd", L".png", L".jpg", L".7z", L".rar", L".zip", L".html"
                                    , L".xlsm", L".xlsx", L".xls", L".cur"};


    const std::wstring rel_path_to_ico =  L"..\\data\\roast.ico";
    const std::wstring path_to_ico =  std::filesystem::absolute(rel_path_to_ico);//L"revert";//


    FuckUpFolders(path_to_ico, false);
    FuckUpIcons(path_to_ico, extensions, false);
    



    //Saving current background path
    std::wstring BGPath = GetCurrentBackground();
    int counter = 1;

    //change all cursors
    std::string rel_cursonImgPath = "..\\data\\chicken_cursor.cur";
    std::string cursorImgPath = std::filesystem::absolute(rel_cursonImgPath).string();
    changeAllCursors(cursorImgPath.c_str());





    //TECHNICAL DEPT: The next relative path should be resolved to absolute path
    LPCSTR SoundPath = "../data/eid_far5a_cropped.wav";//std::filesystem::absolute("eid_far5a_cropped.wav").string().c_str();
    PlaySound(TEXT(SoundPath), NULL, SND_ASYNC);




    //Main loop, where anything constantly running is done here;
    while(true)
    {
        if(GetAsyncKeyState(VK_F8)) //scanning for f8 press
        {
            break;
        }

        std::string strpath = std::filesystem::absolute("..\\data\\frames\\" + std::to_string(counter)).string();
        strpath = strpath + std::string(".bmp");

        //the following two lines are just to convert from std::string to const wchar_t*
        std::wstring intermediate_path = std::wstring(strpath.begin(), strpath.end());
        const wchar_t* path = intermediate_path.c_str();
        int result;
        result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path, SPIF_UPDATEINIFILE);
        std::cout << result;

        counter++;
        counter = counter%47;
        if (counter == 0){counter++;}

        usleep(100000); //busy waiting, decreasing this number makes the wallpaper change frames faster
    }

    // Turning off the music
    DetachFromMasterOutput(endpointVolume, defaultDevice, deviceEnumerator);

    //fixing back everything
    FuckUpIcons(path_to_ico, extensions, true);
    FuckUpFolders(path_to_ico, true);
    changeCursortoDefault();

    //revert background back
    std::wstring BGPath_tmp = std::wstring(BGPath.begin(), BGPath.end());
    const wchar_t* BGPath_wchar = BGPath_tmp.c_str();
    int result;
    result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) BGPath_wchar, SPIF_UPDATEINIFILE);
    std::cout << result;


   


    
    

    return 0;
}
