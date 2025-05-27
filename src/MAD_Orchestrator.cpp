#include <windows.h>
#include <iostream>
#include <filesystem>


char path [255];
int dummy = GetModuleFileNameA(NULL, path, 255);
std::string DirectoryPath = std::filesystem::path(path).parent_path().string();


int main()
{

    std::string PathToMAD = DirectoryPath + "\\" + "MAD.exe";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcessA(NULL, const_cast<char*>(PathToMAD.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    std::cout<<"PID of first MAD is: "<<GetProcessId(pi.hProcess)<<std::endl;
    CloseHandle(pi.hProcess);

    Sleep(400);

    
    STARTUPINFOA si2;
    PROCESS_INFORMATION pi2;
    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    ZeroMemory(&pi2, sizeof(pi2));
    CreateProcessA(NULL, const_cast<char*>(PathToMAD.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2);
    std::cout<<"PID of second MAD is: "<<GetProcessId(pi2.hProcess)<<std::endl;
    CloseHandle(pi2.hProcess);

    return 0;
}