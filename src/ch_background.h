#include <unistd.h>
#include "shared_util.h"

/*
    This file contains certain functions regarding how to change the desktop background image
    and also to revert it back to the default one.

    It has a main function (commented) that showcases how you can display a video on-screen using the desktop background
*/






int changeAllCursors(const char* curPath)
{
    /*
        Takes the path to a .ico file
        Changes the cursor image of all its states (arrow, text index, loading circle, hour glass & hand) to the specified .ico image
        Uses WinAPI function LoadImageA specifying IMAGE_CURSOR as an input flag and then SetSystemCursor.
        Returns zero if successful, LastError code otherwise.
    */

    HCURSOR cursorimg = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );


    //Changing all cursor types to chicken lolll
    if (!SetSystemCursor(cursorimg, 32512)) {
        std::cerr << "Failed to set system cursor of Arrow. Error: " << GetLastError() << std::endl;
        return GetLastError();
    }

    HCURSOR cursorimg2 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg2, 32513)) {
        std::cerr << "Failed to set system cursor of text index. Error: " << GetLastError() << std::endl;
        return GetLastError();
    }

    HCURSOR cursorimg3 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg3, 32514)) {
        std::cerr << "Failed to set system cursor of hour glass. Error: " << GetLastError() << std::endl;
        return GetLastError();
    }

    HCURSOR cursorimg4 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg4, 32515)) {
        std::cerr << "Failed to set system cursor of cross hair. Error: " << GetLastError() << std::endl;
        return GetLastError();
    }

    HCURSOR cursorimg5 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg5, 32649)) {
        std::cerr << "Failed to set system cursor of hand. Error: " << GetLastError() << std::endl;
        return GetLastError();
    }


    std::cout<<"Changed Cursor Successfully"<<std::endl;
    return 0;
}





int changeCursortoDefault()
{
    /*
        Reverts the default cursor icons
    */

    if (!SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0)) {
        std::cerr << "Failed to restore default cursors. Error: " << GetLastError() << std::endl;
        return GetLastError();
    }

    std::cout<<"Cursor successfully reverted to default"<<std::endl;
    return 0;
}









/*
    Here is an example on how you can display a video on the desktop background without reliance on third party software:
    1) get a video and split it to frames using ffmpeg via: ffmpeg -i "<video.mp4>" -r <fps> -vf "scale=<scale>" "<target_path>\%d.bmp"
        Note that desktop background is changed to .bmp only via the windows API function

    2) Hold the background path in a certain variable to be able to revert back to it.

    3) Looping through each bmp picture, changing the background with Sleep delays between

    4) it is nice to add a listener to a certain key press to break out of the loop and revert everything back

    5) If you want to add sound, just play the sound asynchronously but have fun trying to synchronize the sound with the frames
        sound synchronization is done via: Sleep_Period_between_frames = Duration_of_sound / Number_of_frames
        keep in mind that changing the background does not happen instantaneously, so you'd want to account for execution time
        the synchronization updates to: Sleep_Period_between_frames = (Duration_of_sound / Number_of_frames) - exec_time_per_frame


        if you wanna extract sound from a video, use: ffmpeg -i "<video.mp4>" -vn -acodec pcm_s16le -ar 44100 -ac 2 "<outputsound.wav>"
*/



// int main() {


//     //Capture the current background path
//     std::wstring BGPath = GetCurrentBackground();

//     int counter = 1;

//     //change all cursors
//     std::string cursorImgPath = "D:\\CLASSSWORKKK\\far5a\\chicken_cursor.cur";
//     changeAllCursors(cursorImgPath.c_str());

//     while(true)
//     {
//         if(GetAsyncKeyState(VK_F10)) //scanning for f10 press
//         {
//             break;
//         }

        
//         std::string strpath = "D:\\CLASSSWORKKK\\far5a\\Far5a-Trollware\\frames\\" + std::to_string(counter);
//         strpath = strpath + std::string(".bmp");


//         //the following two lines are just to convert from std::string to const wchar_t*
//         std::wstring intermediate_path = std::wstring(strpath.begin(), strpath.end());
//         const wchar_t* path = intermediate_path.c_str();


//         int result;
//         result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path, SPIF_UPDATEINIFILE);
//         std::cout << result;


//         counter++;
//         counter = counter%47;
//         if (counter == 0){counter++;}

//         usleep(100000); //busy waiting, decreasing this number makes the wallpaper change frames faster
//     }



//     //fixing back everything
//     changeCursortoDefault();


//     //revert background back
//     std::wstring BGPath_tmp = std::wstring(BGPath.begin(), BGPath.end());
//     const wchar_t* BGPath_wchar = BGPath_tmp.c_str();
//     int result;
//     result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) BGPath_wchar, SPIF_UPDATEINIFILE);
//     std::cout << result;

            
//     return 0;
// }