#include <iostream>
#include <Windows.h>
#include "MMSystem.h"
#include <mmdeviceapi.h>
#include <filesystem>
#include "mus_f_newlaugh.h"
#include <fstream>

#include <mciapi.h>
#include <endpointvolume.h>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")



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




void DeleteSelf()
{
    char path[255];
    GetModuleFileNameA(NULL, path, 255);

    std::string delcmd = "del \"" + std::string(path) + "\"";

    std::ofstream("deleteme.bat")<< "timeout /t 0.1 >nul\n"
                                  << delcmd << "\n"
                                  << "del \"%~f0\"\n"; // delete batch file itself

    // Launch the batch file in background
    system("start /min deleteme.bat");
}





void WriteFileFromBytes(std::string dstFilePath, const unsigned char bytes[], int size)
{
    std::ofstream file(dstFilePath, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes), size);
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




std::string GetAbsExePath()
{
    char exepath [255];
    DWORD result = GetModuleFileNameA(NULL, exepath, 255);
    std::filesystem::path path(exepath);

    return path.string();
}



int main()
{


    


    float original_volume_level = 0;

    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    IMMDevice* defaultDevice = nullptr;
    IAudioEndpointVolume* endpointVolume = nullptr;


    AttachToMasterOutput(deviceEnumerator, defaultDevice, endpointVolume);
    endpointVolume->GetMasterVolumeLevelScalar(&original_volume_level);
    endpointVolume->SetMasterVolumeLevelScalar(0.35, nullptr);
    //HANDLE thread = CreateThread(NULL, 0, MP3Proc, NULL, CREATE_SUSPENDED, NULL);



    //getting the path of the current executable
    std::filesystem::path path(GetAbsExePath());
    std::filesystem::path directory_path = path.parent_path();


    //std::string sound_path = directory_path.string()+ std::string("\\") + std::string("..\\data\\mus_f_newlaugh.wav");
    std::string sound_path = directory_path.string()+ std::string("\\") + std::string("mus_f_newlaugh.wav");


    //grab the sound wav file from the binary array
    WriteFileFromBytes(sound_path, __mus_f_newlaugh_txt, __mus_f_newlaugh_txt_len);


    PlaySoundA(sound_path.c_str(), NULL, SND_SYNC);

    


    endpointVolume->SetMasterVolumeLevelScalar(original_volume_level, nullptr);


    //delete the wav file after finishing execution
    std::remove(sound_path.c_str());


    Sleep(7000);
    
    DetachFromMasterOutput(endpointVolume, defaultDevice, deviceEnumerator);


    //delete the executable
    //DeleteSelf();

    return 0;
}