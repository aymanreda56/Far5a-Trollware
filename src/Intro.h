#include "ChangeResolution.h"
#include <shlObj.h>

#include <unistd.h>
#include <filesystem>
#include <chrono>


#include <future>
#include <mutex>

#include "MMSystem.h"
#include <mmdeviceapi.h>

#include <mciapi.h>
#include <endpointvolume.h>
#include "shared_util.h"

//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")


/*
    This file contains code that displays an interactive video on the desktop background with some audio and other creepy stuff
*/





int GetFilesCountInDirectory(std::string path){
    /*
        Takes string of the folder path (preferrably absolute path)
        Counts files ONLY, returns their count  
    */
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
    /*
        Toggles desktop icons On->Off and Off->On if you call again
        Exauhstively search for SHELLDLL_DefView window then send it the toggling signal
    */


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
    /*
        This function waits till its time to do an interactive thing with the Intro video
        Waiting period is either of two values determined by which Intro we will use (long intro or short one)
        Then shows the desktop icons for some time (they are already hidden) then hides them again.
    */

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
    /*
        This function waits till its time to do an interactive thing with the Intro video
        Waiting period is either of two values determined by which Intro we will use (long intro or short one)
        Then Flares the desktop icons a lot of times to scare the victim.
    */

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
    /*
        This function waits till its time to do an interactive thing with the Intro video
        Waiting period is either of two values determined by which Intro we will use (long intro or short one)
        Then Changes the orientation of the screen rapidly, giving the sense of being inside a DishWasher.
    */

    if(isShortIntro){Sleep(84000);}
    else {Sleep(225000);}
    
    WashingMachineResolution();
}




/*
    Here is an example on how you can display a video on the desktop background without reliance on third party software:
    1) get a video and split it to frames using ffmpeg via: ffmpeg -i "<video.mp4>" -r <fps> -vf "scale=<scale>" "<target_path>\%d.bmp"
        Note that desktop background is changed to .bmp only via the windows API function

    2) Hold the background path in a certain variable to be able to revert back to it.

    3) Looping through each bmp picture, changing the background with Sleep delays between

    4) it is nice to add a listener to a certain key press to break out of the loop and revert everything back

    5) If you want to add sound, just play the sound asynchronously but have fun trying to synchronize the sound with the frames
        sound synchronization is done via: Sleep_Period_between_frames = Duration_of_sound / Number_of_frames
        keep in mind that changing the background does not happen instantaneously, so you'd want to account for execution time
        the synchronization updates to: Sleep_Period_between_frames = (Duration_of_sound / Number_of_frames) - exec_time_per_frame


        if you wanna extract sound from a video, use: ffmpeg -i "<video.mp4>" -vn -acodec pcm_s16le -ar 44100 -ac 2 "<outputsound.wav>"
*/



void Intro(bool isShortIntro=1) //We choose the short intro by default, the long one is kinda boring
{
    //Some absolute path variables to free my head from the relative path issues
    char patth [255] = {0};
    DWORD tmpglbvar = GetModuleFileNameA(NULL, patth, 255);
    std::filesystem::path exePath(patth);
    std::string DirectoryPath = exePath.parent_path().string();


    std::string shortIntro;
    if(isShortIntro) {shortIntro = "short";}
    else {shortIntro = "long";}
    
    //Hide the desktop icons
    ToggleDesktopIcons();

    //Store the original background to rever back to it after execution.
    std::wstring BGPath = GetCurrentBackground();


    int counter = 1;
    //Getting the total number of frames
    int max_files = GetFilesCountInDirectory(DirectoryPath + "\\" + "..\\data\\"+ shortIntro +"_intro_frames");
    int sound_duration;
    if(isShortIntro) {sound_duration = 83000000;}
    else {sound_duration = 224000000;}          //This magic value is just the duration of the intro wav file, in microseconds.. we will use this value to compute the sleeping period
    
    //Trying to do synchronization between audio and desktop frames
    int time_step_of_one_frame = sound_duration / max_files;    //duration of the entire wav file / num_frames = duration of each single frame,,,, roughly
    bool TimeBenchmark = true;  //useless? maybe
    
    auto start = std::chrono::high_resolution_clock::now(); // measuring how much it takes to execute the background change, for each frame.
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

    //Playing the audio sound in a separate thread
    std::string LaughSoundPath = DirectoryPath + "\\" + "..\\data\\"+ shortIntro +"_intro_audio.wav";
    PlaySoundA(LaughSoundPath.c_str(), NULL, SND_ASYNC);
    

    // Interactive toggling of icons and orientation at certain fixed times of the Intro (see the implementation to see the timestamps)
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

        //Changing desktop background by the new frame counter.bmp
        int result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path, SPIF_UPDATEINIFILE);

        counter++;

        if(TimeBenchmark){
            stop = std::chrono::high_resolution_clock::now(); //TimeBenchmark = false;
            duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();}
        
        //This is my bad way to perform synchronization
        usleep(time_step_of_one_frame - duration*4); //busy waiting, dynamic to accomodate different execution speeds, bad way to do synchronization between sound and video frames
        //usleep(118830);
    }





    //Show desktop Icons back
    ToggleDesktopIcons();

    //Wait till all asynchronous threads have been terminated
    Sleep(8000);

    
    //revert everything back
    //reverting background
    std::wstring BGPath_tmp = std::wstring(BGPath.begin(), BGPath.end());
    const wchar_t* BGPath_wchar = BGPath_tmp.c_str();
    int result;
    result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) BGPath_wchar, SPIF_UPDATEINIFILE);
    std::cout << result;
}