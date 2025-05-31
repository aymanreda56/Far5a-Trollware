#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>

/*
  This file is a demo to test the possibility of killing windows file explorer then starting it again
  it is already integrated into Far5a.cpp, functions are already implemented there
*/



// find process ID by process name, returning the PID
int findMyProc(const char *procname) {

  HANDLE hSnapshot;
  PROCESSENTRY32 pe;
  int pid = 0;
  BOOL hResult;

  // snapshot of all processes in the system
  hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

  // initializing size: needed for using Process32First
  pe.dwSize = sizeof(PROCESSENTRY32);

  // info about first process encountered in a system snapshot
  hResult = Process32First(hSnapshot, &pe);

  // retrieve information about the processes
  // and exit if unsuccessful
  while (hResult) {
    // if we find the process: return process ID
    if (strcmp(procname, pe.szExeFile) == 0) {
      pid = pe.th32ProcessID;
      break;
    }
    hResult = Process32Next(hSnapshot, &pe);
  }

  // closes an open handle (CreateToolhelp32Snapshot)
  CloseHandle(hSnapshot);
  return pid;
}



int main() {
  int pid = 0;

  pid = findMyProc("explorer.exe");
  if (pid) {
    printf("\n\nPID = %d\n\n\n", pid);

    //finally killing the process
    HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (NULL != handle) {   
        TerminateProcess(handle, 0);
        CloseHandle(handle);
    }

  }
  return 0;
}