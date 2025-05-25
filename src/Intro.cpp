#include <shlObj.h>
#include <windows.h>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <chrono>

#include "MMSystem.h"
#include <mmdeviceapi.h>

#include <mciapi.h>
#include <endpointvolume.h>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")





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





int GetFilesCountInDirectory(std::string path){
    auto dirIter = std::filesystem::directory_iterator(path);
    int fileCount = 0;

    for (auto& entry : dirIter)
    {
        if (entry.is_regular_file())
        {
            ++fileCount;
        }
    }
    return fileCount;
}









int main()
{

    //OLD CODE
    /*
    std::cout<<"hello"<<std::endl;
    SHELLSTATE ss = { 0 };
    LPSHELLSTATEA pss = &ss;
    std::cout<<"world"<<std::endl;
    SHGetSetSettings(pss, SSF_HIDEICONS, false);

    std::cout<<pss->fHideIcons<<std::endl;
    std::cout<<"lol"<<std::endl;

    pss->fHideIcons = TRUE;


    SHGetSetSettings(pss, SSF_HIDEICONS, TRUE);
    std::cout<<pss->fHideIcons<<std::endl;
    std::cout<<"lmao"<<std::endl;
    */
    


    HWND hWnd = FindWindow("Progman", NULL);
    HWND hDesktopWnd = FindWindowEx(hWnd, 0, "SHELLDLL_DefView", NULL);

    if (!hDesktopWnd)
        hDesktopWnd = FindWindowEx(0, 0, "SHELLDLL_DefView", NULL);

    SendMessage(hDesktopWnd, WM_COMMAND, 0x7402, 0); // 0x7402 toggles desktop icons



    std::wstring BGPath = GetCurrentBackground();


    int counter = 1;
    int max_files = GetFilesCountInDirectory("D:\\CLASSSWORKKK\\far5a\\Introduction video\\intro_frames\\");
    int sound_duration = 83000000;
    int time_step_of_one_frame = sound_duration / max_files;
    bool TimeBenchmark = true;
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();




    LPCSTR LaughSoundPath = "D:\\CLASSSWORKKK\\far5a\\Introduction video\\Far5a_intro_short.wav";//std::filesystem::absolute("eid_far5a_cropped.wav").string().c_str();
    PlaySound(TEXT(LaughSoundPath), NULL, SND_ASYNC);

    while(counter < max_files)
    {
        
        
        if(TimeBenchmark){start = std::chrono::high_resolution_clock::now();}

        if(GetAsyncKeyState(VK_F10)) //scanning for f10 press
        {
            break;
        }

        
        std::string strpath = "D:\\CLASSSWORKKK\\far5a\\Introduction video\\intro_frames\\" + std::to_string(counter);
        strpath = strpath + std::string(".bmp");


        //the following two lines are just to convert from std::string to const wchar_t*
        std::wstring intermediate_path = std::wstring(strpath.begin(), strpath.end());
        const wchar_t* path = intermediate_path.c_str();


        int result;
        result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path, SPIF_UPDATEINIFILE);


        counter++;
        counter = counter%max_files;
        if (counter == 0){counter++;}


        if(TimeBenchmark){
            stop = std::chrono::high_resolution_clock::now(); //TimeBenchmark = false;
            duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();}

        usleep(time_step_of_one_frame - duration*5); //busy waiting, decreasing this number makes the wallpaper change frames faster
        //usleep(118830);
    }




    //revert background back
    std::wstring BGPath_tmp = std::wstring(BGPath.begin(), BGPath.end());
    const wchar_t* BGPath_wchar = BGPath_tmp.c_str();
    int result;
    result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) BGPath_wchar, SPIF_UPDATEINIFILE);
    std::cout << result;



    if (!hDesktopWnd)
        hDesktopWnd = FindWindowEx(0, 0, "SHELLDLL_DefView", NULL);

    SendMessage(hDesktopWnd, WM_COMMAND, 0x7402, 0); // 0x7402 toggles desktop icons




    return 0;
}