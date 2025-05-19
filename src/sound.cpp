#include <iostream>
#include <Windows.h>
#include "MMSystem.h"
#include <mmdeviceapi.h>

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




int main()
{

    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    IMMDevice* defaultDevice = nullptr;
    IAudioEndpointVolume* endpointVolume = nullptr;

    AttachToMasterOutput(deviceEnumerator, defaultDevice, endpointVolume);
    endpointVolume->SetMasterVolumeLevelScalar(0.8, nullptr);
    //HANDLE thread = CreateThread(NULL, 0, MP3Proc, NULL, CREATE_SUSPENDED, NULL);
    PlaySound(TEXT("eid_far5a.wav"), NULL, SND_SYNC);


    system("pause");
    DetachFromMasterOutput(endpointVolume, defaultDevice, deviceEnumerator);

    return 0;
}