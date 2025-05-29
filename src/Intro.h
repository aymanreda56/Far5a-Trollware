#include "ChangeResolution.h"
#include <shlObj.h>
#include <windows.h>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <chrono>


#include <future>
#include <mutex>

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



void ToggleDesktopIcons()
{
    // HWND hWnd = FindWindow("Progman", NULL);
    // HWND hDesktopWnd = FindWindowEx(hWnd, 0, "SHELLDLL_DefView", NULL);

    // if (!hDesktopWnd)
    //     hDesktopWnd = FindWindowEx(0, 0, "SHELLDLL_DefView", NULL);

    // SendMessage(hDesktopWnd, WM_COMMAND, 0x7402, 0); // 0x7402 toggles desktop icons

    HWND hDesktopWnd = NULL;

    // Try finding the SHELLDLL_DefView window directly under Progman
    HWND hProgman = FindWindow("Progman", NULL);
    hDesktopWnd = FindWindowEx(hProgman, NULL, "SHELLDLL_DefView", NULL);

    // If not found, search through WorkerW windows
    if (!hDesktopWnd)
    {
        HWND hWorkerW = NULL;
        do {
            hWorkerW = FindWindowEx(NULL, hWorkerW, "WorkerW", NULL);
            hDesktopWnd = FindWindowEx(hWorkerW, NULL, "SHELLDLL_DefView", NULL);
        } while (hWorkerW != NULL && hDesktopWnd == NULL);
    }

    if (hDesktopWnd)
    {
        SendMessage(hDesktopWnd, WM_COMMAND, 0x7402, 0); // Toggle icons
    }
}







void AsyncWaitThenShowIconsForSomeTime(bool isShortIntro=1)
{
    if(isShortIntro){Sleep(31000);}
    else {Sleep(170000);}
    //showing icons
    ToggleDesktopIcons();
    Sleep(4000);

    //Hiding them again
    ToggleDesktopIcons();
}



void AsyncWaitingThenContinuouslyTogglingIcons(bool isShortIntro=1)
{

    if(isShortIntro){Sleep(80000);}
    else {Sleep(209000);}
    
    for (int i = 0; i < 7; i++)
    {   // I split the toggling into showing and hiding with different sleeping periods, because the time it takes to hide icons is more than the time it takes to show them
        //So i split the delays trying to balance the visibility of both effects the hiding and showing of icons.

        //showing them
        ToggleDesktopIcons();
        Sleep(400);

        //Hiding them
        ToggleDesktopIcons();
        Sleep(200);
    }
}


void DishWasher(bool isShortIntro=1)
{

    if(isShortIntro){Sleep(84000);}
    else {Sleep(225000);}
    
    WashingMachineResolution();
}






void Intro(bool isShortIntro=1)
{


    
    //Some path variables to free my head from the relative path issues
    char patth [255] = {0};
    DWORD tmpglbvar = GetModuleFileNameA(NULL, patth, 255);
    std::filesystem::path exePath(patth);
    std::string DirectoryPath = exePath.parent_path().string();




    std::string shortIntro;
    if(isShortIntro) {shortIntro = "short";}
    else {shortIntro = "long";}
    
    ToggleDesktopIcons();



    std::wstring BGPath = GetCurrentBackground();



    int counter = 1;
    int max_files = GetFilesCountInDirectory(DirectoryPath + "\\" + "..\\data\\"+ shortIntro +"_intro_frames");
    int sound_duration;
    if(isShortIntro) {sound_duration = 83000000;}
    else {sound_duration = 224000000;}          //This magic value is just the duration of the intro wav file, in microseconds.. we will use this value to compute the sleeping period
    
    int time_step_of_one_frame = sound_duration / max_files;    //duration of the entire wav file / num_frames = duration of each single frame,,,, roughly
    bool TimeBenchmark = true;  //useless? till now
    auto start = std::chrono::high_resolution_clock::now(); // measuring how much it takes to execute the background change, for each frame.
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();




    std::string LaughSoundPath = DirectoryPath + "\\" + "..\\data\\"+ shortIntro +"_intro_audio.wav";
    PlaySoundA(LaughSoundPath.c_str(), NULL, SND_ASYNC);


    
    // Toggling icons asynchronously in fixed time stamps (see the implementation to see the timestamps)
    auto s1 = std::async(std::launch::async, &AsyncWaitThenShowIconsForSomeTime, isShortIntro);
    auto s2 = std::async(std::launch::async, &AsyncWaitingThenContinuouslyTogglingIcons, isShortIntro);
    auto s3 = std::async(std::launch::async, &DishWasher, isShortIntro);

    while(counter < max_files)
    {
        
        
        if(TimeBenchmark){start = std::chrono::high_resolution_clock::now();}

        if(GetAsyncKeyState(VK_F8)) //scanning for f8 press to break the cycle and revert everything back
        {
            break;
        }

        

        
        std::string strpath = DirectoryPath + "\\" + "..\\data\\"+ shortIntro +"_intro_frames\\" + std::to_string(counter) + ".bmp";


        //the following two lines are just to convert from std::string to const wchar_t*
        std::wstring intermediate_path = std::wstring(strpath.begin(), strpath.end());
        const wchar_t* path = intermediate_path.c_str();


        int result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path, SPIF_UPDATEINIFILE);


        counter++;

        if(TimeBenchmark){
            stop = std::chrono::high_resolution_clock::now(); //TimeBenchmark = false;
            duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();}

        usleep(time_step_of_one_frame - duration*4); //busy waiting, dynamic to accomodate different execution speeds, bad way to do synchronization between sound and video frames
        //usleep(118830);
    }






    ToggleDesktopIcons();

    Sleep(8000);

    
    //revert everything back
    //reverting background
    std::wstring BGPath_tmp = std::wstring(BGPath.begin(), BGPath.end());
    const wchar_t* BGPath_wchar = BGPath_tmp.c_str();
    int result;
    result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) BGPath_wchar, SPIF_UPDATEINIFILE);
    std::cout << result;
}