#include <windows.h>
#include <iostream>
#include <vector>

#include <future>
#include <mutex>
#include <random>

// Global variables for positioning
HHOOK g_hHook = NULL;


std::vector<std::string> Creepy_Messages = {"Ha! Got your browsing history\nI'm gonna tell your mom that you watch Corn!",
                                "Nice search history you got there...\nbe a shame if your grandma saw it",
                                "Wow… 47 Google searches for 'how to flirt over text'?\nbold of you to assume I wouldn't find that!",
                            "Searched 'how to delete embarrassing questions from ChatGPT'\ntoo late bro",
                        "I found 12 tabs open to 'motivational quotes' and 1 to 'how to quit everything and live in the woods'.\nYou okay?",
                    "You locked your door, right?\n...Are you sure?",
                "I saw what you did.\nYou can pretend you forgot\n\tbut I won't...",
            "Your camera blinked twice. You aren't on a video call.",
        "You thought you were alone in the house all night. You weren't.",
    "You deleted the file, but it's still here. Watching.",
"That whisper you heard last night? That wasn't your imagination.",
"You typed that password too fast. Someone else remembered it too.",
"What if you're the version that wasn't supposed to wake up?",
"They told you that voice in your head was \"you\". They lied.",
"You looked away from the screen. When you look back...\n\tit won't be the same.",
""};






void SpawnDelayedMessageBox(int delayPeriod, std::string text, std::string caption, bool stop = false)
{

    Sleep(delayPeriod);
    MessageBoxA(NULL, text.c_str(), caption.c_str(), MB_OK | MB_ICONWARNING | MB_DEFBUTTON1 | MB_APPLMODAL | MB_TOPMOST);

    if(!stop)
    {
        auto a1 = std::async(std::launch::async, &SpawnDelayedMessageBox, 10000, "How to stop the Laughing Audio?\n1) Open a cmd\n2) Type taskkill /f /t /im MAD.exe   then press enter\n3) Type sc stop MADOrch then press enter\n4) Type sc delete MADOrch then press enter\n 5) Type sc stop LaughService then press enter\n6) Type sc delete LaughService then press enter\n\t:)\n   Happy Hacking!!", "How to stop the malware?", 1);
    }

}



// Hook procedure to intercept window creation
LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam) {

    
    int g_nextX = 0;
    int g_nextY = 0;
    bool g_positionNext = false;


    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Generate random position
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> xDist(0, std::max(0, screenWidth - 400));
    std::uniform_int_distribution<> yDist(0, std::max(0, screenHeight - 200));
    
    // Set the position for the next message box
    g_nextX = xDist(gen);
    g_nextY = yDist(gen);
    g_positionNext = true;


    if (nCode == HCBT_ACTIVATE && g_positionNext) {
        HWND hwnd = (HWND)wParam;
        
        // Check if this is a message box by checking class name
        char className[256];
        GetClassNameA(hwnd, className, sizeof(className));
        
        if (strcmp(className, "#32770") == 0) { // Message box class
            SetWindowPos(hwnd, HWND_TOPMOST, g_nextX, g_nextY, 0, 0, 
                        SWP_NOSIZE | SWP_NOZORDER);
            g_positionNext = false;
        }
    }
    
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}



void SpawnMessageBoxAtRandomLoc(std::string text, std::string caption)
{

    
    g_hHook = SetWindowsHookEx(WH_CBT, CBTProc, GetModuleHandle(NULL), GetCurrentThreadId());

    if (!g_hHook) {
        std::cout << "Failed to install hook!" << std::endl;
        return;
    }

    // Show message box (will be repositioned by hook)
    MessageBoxA(NULL, text.c_str(), caption.c_str(), MB_OK | MB_ICONQUESTION | MB_DEFBUTTON1 | MB_APPLMODAL | MB_TOPMOST);
}




int main()
{
    srand(static_cast<unsigned int>(time(0)));    

    Sleep(10000);

    while(true)
    {
        Sleep(3000);
        if(GetAsyncKeyState(VK_F8)) //scanning for f8 press
        {
            break;
        }

        auto a1 = std::async(std::launch::async, &SpawnMessageBoxAtRandomLoc, Creepy_Messages[rand()%Creepy_Messages.size()], "MWahahahaha!!");
        
    }

    return 0;
}