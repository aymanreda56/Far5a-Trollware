#include <windows.h>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

/*
    This file has some helper functions regarding manipulation of the screen resolution and orientation
*/


bool SaveOriginalResolution(DEVMODE& originalMode) {
    /*
        Saves the original screen resolution/orientation parameters to be able to revert back to it later.
        Values are written to originalMode passed by ref.
    */

    originalMode.dmSize = sizeof(originalMode);
    return EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &originalMode);
}



int RevertResolutionToDefault(DEVMODE& originalMode)
{
    /*
        Reverts everything back to the passed Mode, which will be the original mode
        returns the error code or 0 if successful
    */

    LONG result = ChangeDisplaySettingsExA(NULL, &originalMode, NULL, CDS_RESET, NULL);
    switch(result)
    {
        case DISP_CHANGE_SUCCESSFUL:
            std::cerr << "resolution changed successfully!" << std::endl; return result;
        case DISP_CHANGE_BADDUALVIEW:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADDUALVIEW!" << std::endl; return result;
        case DISP_CHANGE_BADFLAGS:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADFLAGS!" << std::endl; return result;
        case DISP_CHANGE_BADMODE:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADMODE!" << std::endl; return result;
        case DISP_CHANGE_BADPARAM:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADPARAM!" << std::endl; return result;
        case DISP_CHANGE_FAILED:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_FAILED!" << std::endl; return result;
        case DISP_CHANGE_NOTUPDATED:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_NOTUPDATED!" << std::endl; return result;
        case DISP_CHANGE_RESTART:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_RESTART!" << std::endl; return result;
        default:
            std::cerr << "error in resolution change attempt, ERROR CODE: "<<result << std::endl; return result;
    }

}




LONG ChangeResolution(int width, int height, DWORD orientation)
{
    /*
        Changes resoltion and orientation to the target parameters
        returns error code or 0 if successful
    */

    DEVMODE mode = {0};

    mode.dmSize = sizeof(mode);
    mode.dmPelsWidth = width;
    mode.dmPelsHeight = height;
    mode.dmDisplayOrientation = orientation;        //orientation is 1 for no
    mode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH | DM_DISPLAYORIENTATION;
    LONG result = ChangeDisplaySettingsExA(NULL, &mode, NULL, CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET, NULL); //Choose the default display device, with the desired mode, with those flags, and no video parameters
    
    
    switch(result)
    {
        case DISP_CHANGE_SUCCESSFUL:
            std::cerr << "resolution changed successfully!" << std::endl; return result;
        case DISP_CHANGE_BADDUALVIEW:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADDUALVIEW!" << std::endl; return result;
        case DISP_CHANGE_BADFLAGS:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADFLAGS!" << std::endl; return result;
        case DISP_CHANGE_BADMODE:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADMODE!" << std::endl; return result;
        case DISP_CHANGE_BADPARAM:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_BADPARAM!" << std::endl; return result;
        case DISP_CHANGE_FAILED:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_FAILED!" << std::endl; return result;
        case DISP_CHANGE_NOTUPDATED:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_NOTUPDATED!" << std::endl; return result;
        case DISP_CHANGE_RESTART:
            std::cerr << "error in resolution change attempt: DISP_CHANGE_RESTART!" << std::endl; return result;
        default:
            std::cerr << "error in resolution change attempt, ERROR CODE: "<<result << std::endl; return result;

    }
    
    return result;
}




std::vector<std::pair<int,int>> GetAvailableResolutions()
{
    /*
        Not all display monitors support all combinations of resolutions
        So this function returns a vector of all viable combinations of resolutions that are supported by your display monitor
        returns a vector of pairs, sorted ascendingly, unique elements
    */

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);

    std::set<std::pair<int, int>> uniqueModes;
    int modeNum = 0;

    std::cout << "Available landscape resolutions:\n";

    while (EnumDisplaySettings(NULL, modeNum++, &devMode)) {
        int width = devMode.dmPelsWidth;
        int height = devMode.dmPelsHeight;
        uniqueModes.insert(std::pair<int, int>{width, height});
        std::cout<<width<<"x"<<height<<std::endl;
    }

    
    std::vector<std::pair<int, int>> resolutions;
    resolutions.assign( uniqueModes.begin(), uniqueModes.end() );
    std::sort(resolutions.begin(), resolutions.end(), [](auto &left, auto &right) {
    return left.second < right.second;
    }); 
    //resolutions.erase( unique( resolutions.begin(), resolutions.end() ), resolutions.end() );

    for (auto& res :resolutions){std::cout<<res.first<<"x"<<res.second<<std::endl;}
    return resolutions;
}



void AscendingResolutionRollerCoaster()
{
    /*
        This rollercoaster changes the victim's resolution slowly from the lowest possible resolution to the highest
        then changes it again from the highest resolution slowly to the lowest
        and so on.
        Then reverts to the default original resolution.
    */

    DEVMODE OriginalMode = {0}; //preserving original resolution
    if(!SaveOriginalResolution(OriginalMode)){std::cout<<"Error is preserving original resolution"<<std::endl;}

    std::vector<std::pair<int, int>> sample_resolutions = GetAvailableResolutions();
    
    std::reverse(sample_resolutions.begin(), sample_resolutions.end());

    for (auto& res :sample_resolutions)
    {
        ChangeResolution(res.first, res.second, DMDFO_DEFAULT);
        Sleep(1200);
    }

    std::reverse(sample_resolutions.begin(), sample_resolutions.end());
    for (auto& res :sample_resolutions)
    {
        ChangeResolution(res.first, res.second, DMDFO_DEFAULT);
        Sleep(1200);
    }


    RevertResolutionToDefault(OriginalMode);
}





void AlternatingResolution()
{
    /*
        This function alternates the resolution of the victim's machine between the highest and lowest resolutions multiple times.
        Then finally reverting back to the original resolution.
    */

    DEVMODE OriginalMode = {0}; //preserving original resolution
    if(!SaveOriginalResolution(OriginalMode)){std::cout<<"Error is preserving original resolution"<<std::endl;}

    std::vector<std::pair<int, int>> sample_resolutions = GetAvailableResolutions();

    for(int i =0; i<10;i++)
    {
        ChangeResolution(sample_resolutions[0].first, sample_resolutions[0].second, DMDFO_DEFAULT);
        Sleep(1200);
        ChangeResolution(sample_resolutions.back().first, sample_resolutions.back().second, DMDFO_DEFAULT);
        Sleep(1200);
    }

    RevertResolutionToDefault(OriginalMode);
}




void WashingMachineResolution()
{
    /*
        This function changes the orientation of the victim's monitor rapidly giving him the sense of being inside a washing machine
        Then finally reverting to the original orientation.
    */

    DEVMODE OriginalMode = {0}; //preserving original resolution
    if(!SaveOriginalResolution(OriginalMode)){std::cout<<"Error is preserving original resolution"<<std::endl;}

    int orig_width = OriginalMode.dmPelsWidth;
    int orig_height = OriginalMode.dmPelsHeight;


    std::vector<int> orientations = {DMDO_90, DMDO_180, DMDO_270, DMDO_DEFAULT};
    int w,h;
    for(int i =0; i<2;i++)
    {
        for (auto& orient : orientations)
        {
            if(!orient%2){w=orig_width; h=orig_height;}
            else{w=orig_height; h=orig_width;}
            ChangeResolution(w, h, orient);
            Sleep(200);
        }

        for (auto& orient : orientations)
        {
            if(!orient%2){w=orig_width; h=orig_height;}
            else{w=orig_height; h=orig_width;}
            ChangeResolution(w, h, orient);
            Sleep(200);
        }

        std::reverse(orientations.begin(), orientations.end());
        for (auto& orient : orientations)
        {
            if(!orient%2){w=orig_width; h=orig_height;}
            else{w=orig_height; h=orig_width;}
            ChangeResolution(w, h, orient);
            Sleep(200);
        }
        for (auto& orient : orientations)
        {
            if(!orient%2){w=orig_width; h=orig_height;}
            else{w=orig_height; h=orig_width;}
            ChangeResolution(w, h, orient);
            Sleep(200);
        }   
    }


    RevertResolutionToDefault(OriginalMode);
}


/*
    This is just a demo that showcases the above functions
*/

// int main()
// {


//     // AscendingResolutionRollerCoaster();
    
//     // RevertResolutionToDefault(OriginalMode);

    
//     // AlternatingResolution();

//     // RevertResolutionToDefault(OriginalMode);

    
//     WashingMachineResolution();
    
//     return 0;
// }
