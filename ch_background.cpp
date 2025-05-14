#include <windows.h>
#include <iostream>
#include <unistd.h>








int changeAllCursors(const char* curPath)
{
    HCURSOR cursorimg = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );


    //Changing all cursor types to chicken lolll
    if (!SetSystemCursor(cursorimg, 32512)) {
        std::cerr << "Failed to set system cursor of Arrow. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg2 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg2, 32513)) {
        std::cerr << "Failed to set system cursor of text index. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg3 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg3, 32514)) {
        std::cerr << "Failed to set system cursor of hour glass. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg4 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg4, 32515)) {
        std::cerr << "Failed to set system cursor of cross hair. Error: " << GetLastError() << std::endl;
        return 1;
    }

    HCURSOR cursorimg5 = (HCURSOR)LoadImageA(NULL, curPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE );
    if (!SetSystemCursor(cursorimg5, 32649)) {
        std::cerr << "Failed to set system cursor of hand. Error: " << GetLastError() << std::endl;
        return 1;
    }


    return 0;
}



int changeCursortoDefault()
{
    if (!SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0)) {
        std::cerr << "Failed to restore default cursors. Error: " << GetLastError() << std::endl;
        return 1;
    }
}







int main() {

    int counter = 1;

    //change all cursors
    std::string cursorImgPath = "D:\\CLASSSWORKKK\\far5a\\chicken_cursor.cur";
    changeAllCursors(cursorImgPath.c_str());

    while(true)
    {
        if(GetAsyncKeyState(VK_F10)) //scanning for f10 press
        {
            break;
        }

        
        std::string strpath = "D:\\CLASSSWORKKK\\far5a\\Far5a-Trollware\\frames\\" + std::to_string(counter);
        strpath = strpath + std::string(".bmp");


        //the following two lines are just to convert from std::string to const wchar_t*
        std::wstring intermediate_path = std::wstring(strpath.begin(), strpath.end());
        const wchar_t* path = intermediate_path.c_str();


        int result;
        result = SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (void *) path, SPIF_UPDATEINIFILE);
        std::cout << result;


        counter++;
        counter = counter%47;
        if (counter == 0){counter++;}

        usleep(100000); //busy waiting, decreasing this number makes the wallpaper change frames faster
    }



    //fixing back everything
    changeCursortoDefault();






    
            
    return 0;
}